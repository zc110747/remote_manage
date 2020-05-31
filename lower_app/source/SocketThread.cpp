/*
 * File      : SocketThread.cpp
 * Socket应用执行任务
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
#include "../include/SocketThread.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../include/SystemConfig.h"

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static CSocketProtocolInfo *pSocketProtocolInfo;
static uint8_t 	nRxCacheBuffer[SOCKET_BUFFER_SIZE];
static uint8_t  nTxCacheBuffer[SOCKET_BUFFER_SIZE];

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/**************************************************************************
* Local Function
***************************************************************************/
static void *SocketLoopThread(void *arg);
static void *SocketDataProcessThread(void *arg);

/**************************************************************************
* Function
***************************************************************************/
/**
 * 网络通讯接口初始化
 * 
 * @param NULL
 *  
 * @return NULL
 */
void SocketThreadInit(void)
{
	int nErr;
	pthread_t tid1;

    USR_DEBUG("Socket Thread Start!\n");
    pSocketProtocolInfo = new CSocketProtocolInfo(nRxCacheBuffer, nTxCacheBuffer, 
                        &nRxCacheBuffer[FRAME_HEAD_SIZE], SOCKET_BUFFER_SIZE);
    nErr = pthread_create(&tid1, NULL, SocketLoopThread, NULL);
	if(nErr != 0)
    {
		USR_DEBUG("uart task thread create nErr, %d\n", nErr);
	}
}

/**
 * Socket绑定和等待接收的接口实现
 * 
 * @param arg:通过pthread传递的参数
 *  
 * @return NULL
 */
static void *SocketLoopThread(void *arg)
{
    int server_fd;
    int result;
    struct sockaddr_in serverip, clientip;
    int is_bind_fail = 0;
    struct SSystemConfig *pSystemConfigInfo;
	
	pSystemConfigInfo = GetSSytemConfigInfo();

    memset((char *)&serverip, 0, sizeof(serverip));
    serverip.sin_family = AF_INET;
    serverip.sin_port = htons(pSystemConfigInfo->m_net_port);
    serverip.sin_addr.s_addr = inet_addr(pSystemConfigInfo->m_ipaddr.c_str());

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd != -1)
    {
        
        do 
        {
            result = bind(server_fd, (struct sockaddr *)&serverip, sizeof(serverip));
            if(result == -1)
            {
                if(is_bind_fail == 0)
                {
                    is_bind_fail = 1;
                    SOCKET_DEBUG("socket bind failed!\r\n");
                }
                sleep(1);
            }
            else
            {
                break;
            }
        } while (1); //网络等待socket绑定完成后执行后续
        
        USR_DEBUG("Socket Binding OK, Now Listen...!\n");
        listen(server_fd, 32);
        while(1)
        {
            uint32_t client_size;
            int client_fd;

            client_size = sizeof(clientip);
            client_fd = accept(server_fd, (struct sockaddr *)&clientip, &client_size);
            if(client_fd < 0)
            {
                SOCKET_DEBUG("socket accept failed!\r\n");
                continue;
            } 
            else
            {
                int nErr;
                pthread_t tid1;
                nErr = pthread_create(&tid1, NULL, SocketDataProcessThread, &client_fd);
                if(nErr != 0)
                {
                    SOCKET_DEBUG("socket date process failed!\r\n");
                }
            }
        }
    }
    else
    {
        SOCKET_DEBUG("socket init failed!\r\n");
    }
    return (void *)0;
}

/**
 * Socket数据处理的接口
 * 
 * @param NULL
 *  
 * @return NULL
 */
static void *SocketDataProcessThread(void *arg)
{
    int nFlag;
    int client_fd = *(int *)arg;
    uint8_t is_loop = 0;
    fd_set read_set;

    FD_ZERO(&read_set);
    FD_SET(client_fd ,&read_set);

    for(;;)
    {	   
		nFlag = pSocketProtocolInfo->CheckRxBuffer(client_fd);
		if(nFlag == RT_OK)
        {
			pSocketProtocolInfo->ExecuteCommand(client_fd);
            pSocketProtocolInfo->SendTxBuffer(client_fd);
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

    SOCKET_DEBUG("socket process success\r\n");
    close(client_fd);
    pthread_detach(pthread_self());
    return  (void *)0;
}

