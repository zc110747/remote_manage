//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SocketTcpThread.cpp
//
//  Purpose:
//      TCP协议执行模块，目前设计为同步模式，后续会修改
//
// Author:
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "SocketTcpThread.hpp"

static void *SocketTcpLoopThread(void *arg);
static void *SocketTcpDataProcessThread(void *arg);

TcpThreadManage* TcpThreadManage::pInstance = nullptr;
TcpThreadManage* TcpThreadManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) TcpThreadManage;
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Tcp thread manage new failed!");
        }
    }
    return pInstance;
}

bool TcpThreadManage::init()
{
    bool ret = true;

#if SOCKET_TCP_MODULE_ON == 1
    m_thread = std::move(std::thread(SocketTcpLoopThread, this));
    m_thread.detach();
#endif
    return ret;
}

/**
 * Socket绑定和等待接收的接口实现
 * 
 * @param arg:通过pthread传递的参数
 *  
 * @return NULL
 */
static void *SocketTcpLoopThread(void *arg)
{
    int server_fd(-1), is_bind_fail(0);
    struct sockaddr_in serverip, clientip;
	const SocketSysConfig *pSocketConfig = SystemConfig::getInstance()->gettcp();

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Socket tcp thread Start!");
    memset((char *)&serverip, 0, sizeof(serverip));
    serverip.sin_family = AF_INET;
    serverip.sin_port = htons(pSocketConfig->port);
    serverip.sin_addr.s_addr = inet_addr(pSocketConfig->ipaddr.c_str());

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd != -1)
    {   
        int one = 1;

#ifndef WIN32
        /*Linux平台默认断开后2min内处于Wait Time状态，不允许重新绑定，需要添加配置，允许在该状态下重新绑定*/
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &one, (socklen_t)sizeof(one));
#endif

        do 
        {
            int result;

            result = bind(server_fd, (struct sockaddr *)&serverip, sizeof(serverip));
            if(result == -1)
            {
                static int check;
                if(is_bind_fail == 0)
                {
                    is_bind_fail = 1;
                    PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Socket tcp bind %s:%d failed!", pSocketConfig->ipaddr.c_str(), pSocketConfig->port); 
                    if(errno == EADDRINUSE)
                    {
                        server_fd = socket(AF_INET, SOCK_STREAM, 0);
                    }              
                }
                sleep(1);
            }
            else
            {
                break;
            }
        } while (1); //网络等待socket绑定完成后执行后续
        
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Socket tcp bind %s:%d success!", pSocketConfig->ipaddr.c_str(), pSocketConfig->port); 
        listen(server_fd, 32);
        while(1)
        {
            uint32_t client_size;
            int client_fd;

            client_size = sizeof(clientip);
            client_fd = accept(server_fd, (struct sockaddr *)&clientip, &client_size);
            if(client_fd < 0)
            {
                continue;
            } 
            else
            {
                int nErr;
                std::thread(SocketTcpDataProcessThread, &client_fd).detach();
            }
        }
    }
    else
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Socket tcp create failed!");
    }

    close(server_fd);

    return (void *)arg;
}

/**
 * Socket数据处理的接口
 * 
 * @param NULL
 *  
 * @return NULL
 */
static void *SocketTcpDataProcessThread(void *arg)
{
    int nFlag;
    int size;
    int client_fd = *static_cast<int *>(arg);
    uint8_t is_loop = 0;
    fd_set read_set;
    std::unique_ptr<uint8_t[]> nRxCacheBuffer(new uint8_t[SOCKET_BUFFER_SIZE]);
    std::unique_ptr<uint8_t[]> nTxCacheBuffer(new uint8_t[SOCKET_BUFFER_SIZE]);

    auto pTcpProtocolInfo = new CTcpProtocolInfo(client_fd);

    FD_ZERO(&read_set);
    FD_SET(client_fd ,&read_set);

    for(;;)
    {	   
        if(pTcpProtocolInfo->CheckRxFrame())
        {
            pTcpProtocolInfo->ProcessRxFrame();
        }
        else
        {
            //do nothing
        }
	}

    close(client_fd);
    return (void *)arg;
}
