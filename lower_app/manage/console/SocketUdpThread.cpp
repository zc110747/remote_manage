//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SocketTcpThread.cpp
//
//  Purpose:
//      Socket Tcp Thread process workflow.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/31/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "SocketUdpThread.hpp"

static void *SocketUdpLoopThread(void *arg);

UdpThreadManage *UdpThreadManage::pInstance = nullptr;
UdpThreadManage *UdpThreadManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) UdpThreadManage;
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "UdpThreadManage new failed!");
        }
    }
    return pInstance;
}

bool UdpThreadManage::init()
{
    bool ret = true;

#if SOCKET_UDP_MODULE_ON == 1
    pProtocolInfo = new(std::nothrow) CUdpProtocolInfo(RxCacheBuffer, TxCacheBuffer, UDP_BUFFER_SIZE);
    pthread = new(std::nothrow) std::thread(SocketUdpLoopThread, this);
    
    if(pthread == nullptr || pProtocolInfo == nullptr)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Socket udp thread init failed!");
        ret = false;
    }
#endif
    return ret;
}

static void *SocketUdpLoopThread(void *arg)
{
    int socket_fd(-1), is_bind_fail(0);   
    struct sockaddr_in servaddr;       
    const auto pSocketConfig = SystemConfig::getInstance()->getudp();
    auto pUdpThreadInfo = static_cast<UdpThreadManage *>(arg);
    UDP_CLIENT *pClient = pUdpThreadInfo->getClient();
    auto pProtocolInfo = pUdpThreadInfo->getProtocolInfo();

    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Socket udp thread start!");
    /*创建socket接口, SOCK_DGRAM表示无连接的udp接口*/
    socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(socket_fd != -1)
    {
        /*绑定到指定端口*/
        memset(&servaddr, 0, sizeof(servaddr));    
        servaddr.sin_family = AF_INET;     
        servaddr.sin_addr.s_addr = inet_addr(pSocketConfig->ipaddr.c_str());  
        servaddr.sin_port = htons(pSocketConfig->port); 

        do 
        {
            int result;

            result = bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
            if(result == -1)
            {
                if(is_bind_fail == 0)
                {
                    is_bind_fail = 1;
                    PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Socket udp bind %s:%d failed!", pSocketConfig->ipaddr.c_str(), pSocketConfig->port); 
                }
                sleep(1);
            }
            else
            {
                break;
            }
        } while (1); //网络等待socket绑定完成后执行后续

        PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Socket udp bind %s:%d success!", pSocketConfig->ipaddr.c_str(), pSocketConfig->port); 

        for(;;)
        {	   
            int nFlag;
		    nFlag = pProtocolInfo->CheckRxBuffer(socket_fd, true, pClient);
		    if(nFlag == RT_OK)
            {
                pProtocolInfo->ExecuteCommand(socket_fd);
                pProtocolInfo->SendTxBuffer(socket_fd, pClient);
		    }
            else
            {
                sleep(1);
            }
        }
	}
    else
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Socket udp create failed!");
    }
    
    close(socket_fd);
}
