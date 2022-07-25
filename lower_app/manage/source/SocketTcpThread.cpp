/*
 * File      : SocketTcpThread.cpp
 * Tcp Socket通讯线程处理
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-23      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "SocketTcpThread.hpp"

#if SOCKET_TCP_MODULE_ON == 1
/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/
struct client_udp
{
    struct sockaddr_in clientip;
    uint32_t client_size;
};

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/*TCP通讯应用处理主线程*/
static void *SocketTcpLoopThread(void *arg);

/*TCP通讯数据处理线程*/
static void *SocketTcpDataProcessThread(void *arg);

/**************************************************************************
* Function
***************************************************************************/
/**
 * TCP网络通讯任务和数据初始化
 * 
 * @param NULL
 *  
 * @return NULL
 */
void SocketTcpThreadInit(void)
{
	int nErr;
	pthread_t tid1;

    nErr = pthread_create(&tid1, NULL, SocketTcpLoopThread, NULL);
	if(nErr != 0)
    {
		USR_DEBUG("Tcp Task Thread Create Err:%d\n", nErr);
	}
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
    int server_fd;
    int result;
    struct sockaddr_in serverip, clientip;
    int is_bind_fail = 0;
    struct SSystemConfig *pSystemConfigInfo;
	
    USR_DEBUG("Socket Tcp Thread Start!\n");

	pSystemConfigInfo = GetSSytemConfigInfo();
    memset((char *)&serverip, 0, sizeof(serverip));
    serverip.sin_family = AF_INET;
    serverip.sin_port = htons(pSystemConfigInfo->m_tcp_net_port);
    serverip.sin_addr.s_addr = inet_addr(pSystemConfigInfo->m_tcp_ipaddr.c_str());

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
            result = bind(server_fd, (struct sockaddr *)&serverip, sizeof(serverip));
            if(result == -1)
            {
                static int check;
                if(is_bind_fail == 0)
                {
                    is_bind_fail = 1;
                    SOCKET_DEBUG("Tcp Bind %s Failed!, error:%s\r\n",  pSystemConfigInfo->m_tcp_ipaddr.c_str(), strerror(errno));
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
        
        SOCKET_DEBUG("Tcp Bind ok, ServerIp:%s, NetPort:%d\n", pSystemConfigInfo->m_tcp_ipaddr.c_str(), 
                pSystemConfigInfo->m_tcp_net_port);  
        listen(server_fd, 32);
        while(1)
        {
            uint32_t client_size;
            int client_fd;

            client_size = sizeof(clientip);
            client_fd = accept(server_fd, (struct sockaddr *)&clientip, &client_size);
            if(client_fd < 0)
            {
                SOCKET_DEBUG("Tcp accept failed!\r\n");
                continue;
            } 
            else
            {
                int nErr;
                pthread_t tid1;
                nErr = pthread_create(&tid1, NULL, SocketTcpDataProcessThread, &client_fd);
                if(nErr != 0)
                {
                    SOCKET_DEBUG("Tcp Date Process Failed!\r\n");
                }
            }
        }
    }
    else
    {
        SOCKET_DEBUG("Tcp Init Failed!\r\n");
    }

    close(server_fd);
    pthread_detach(pthread_self()); 
    pthread_exit((void *)0);
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
    CTcpProtocolInfo<int *> *pTcpProtocolInfo;
    std::unique_ptr<uint8_t[]> nRxCacheBuffer(new uint8_t[SOCKET_BUFFER_SIZE]);
    std::unique_ptr<uint8_t[]> nTxCacheBuffer(new uint8_t[SOCKET_BUFFER_SIZE]);
    pTcpProtocolInfo = new CTcpProtocolInfo<int *>(nRxCacheBuffer.get(), 
                            nTxCacheBuffer.get(), SOCKET_BUFFER_SIZE);

    FD_ZERO(&read_set);
    FD_SET(client_fd ,&read_set);

    for(;;)
    {	   
		nFlag = pTcpProtocolInfo->CheckRxBuffer(client_fd, false, &size);
		if(nFlag == RT_OK)
        {
			pTcpProtocolInfo->ExecuteCommand(client_fd);
            pTcpProtocolInfo->SendTxBuffer(client_fd, &size);
            if(!pTcpProtocolInfo->GetFileUploadStatus())
                break;
		}
        else if(nFlag == RT_FAIL)
        {
            /*判断是否有数据可读取, 不可读则退出*/
            struct timeval client_timeout = {0, 100};

            nFlag = select(client_fd, &read_set, NULL, NULL, &client_timeout);
            if(nFlag < 0){
                break;
            }
        }
        else
        {
            break;
        }
	}

    SOCKET_DEBUG("Socket Process Success\r\n");
    close(client_fd);
    pthread_detach(pthread_self());
    pthread_exit((void *)0);
}
#endif
