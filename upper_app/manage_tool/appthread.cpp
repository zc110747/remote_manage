
#include "appthread.h"
#include "uartclient.h"
#include "tcpclient.h"
#include "commandinfo.h"
#include <QFile>

static CUdpSocketInfo *pCUdpSocketThreadInfo;
static CTcpSocketInfo *pCTcpSocketThreadInfo;
static CUartProtocolInfo *pCUartProtocolTreadInfo;

static CAppThreadInfo *pAppThreadInfo;
static SSendBuffer SendBufferInfo;

/*!
    应用执行的主线程函数
*/
void CAppThreadInfo::run()
{
    int nStatus;

    pCUdpSocketThreadInfo = GetUdpClientSocketInfo();
    pCUdpSocketThreadInfo->UdpClientSocketInitForThread();
    pCUartProtocolTreadInfo = GetUartProtocolInfo();
    pCTcpSocketThreadInfo = GetTcpClientSocketInfo();
    pCTcpSocketThreadInfo->TcpClientSocketInitForThread();

    for(;;)
    {
        if(m_nIsStop)
            return;
        nStatus = m_pQueue->QueuePend(&SendBufferInfo);
        if(nStatus == QUEUE_INFO_OK)
        {
            if(SendBufferInfo.m_nCommand != SYSTEM_UPDATE_CMD)
            {
                if(SendBufferInfo.m_nProtocolStatus == PROTOCOL_UART)
                {
                    pCUartProtocolTreadInfo->UartLoopThread(&SendBufferInfo);
                }
                else if(SendBufferInfo.m_nProtocolStatus == PROTOCOL_TCP)
                {
                    pCTcpSocketThreadInfo->TcpClientSocketLoopThread(&SendBufferInfo);
                }
                else if(SendBufferInfo.m_nProtocolStatus == PROTOCOL_UDP)
                {
                    pCUdpSocketThreadInfo->UdpClientSocketLoopThread(&SendBufferInfo);
                }
                else
                {
                    qDebug()<<"Invalid Protocol Type";
                }
            }
            else
            {
                //处理升级的整个流程实现
                QFile file(SendBufferInfo.m_qPathInfo);
                if(file.open(QIODevice::ReadOnly))
                {

                }
            }
            qDebug()<<"Thread Queue Test Ok";
        }
    }
}

/*!
    主应用线程初始化
*/
void AppThreadInit(void)
{
    pAppThreadInfo = new CAppThreadInfo();
}

/*!
    获取主应用线程的信息数据结构
*/
CAppThreadInfo *GetAppThreadInfo()
{
    return pAppThreadInfo;
}
