//uart任务处理
#include "uartthread.h"
#include "mainwindow.h"

static CUartProtocolInfo *pUartProtocolInfo;
static uint8_t rx_buffer[BUFF_CACHE_SIZE];
static uint8_t tx_buffer[BUFF_CACHE_SIZE];


CUartThread::CUartThread(QObject *parent):QThread(parent)
{
   m_nIsStop = 0;
}

//关闭任务
void CUartThread::CloseThread()
{
    m_nIsStop = 1;
}

//任务执行函数
void CUartThread::run()
{
    SSendBuffer SendBufferInfo;
    int nLen;
    int nStatus;
    QString Sendbuf = "";

    for(;;)
    {
        if(m_nIsStop)
            return;

        nStatus  = pUartProtocolInfo->m_pUartQueue->QueuePend(&SendBufferInfo);
        if(nStatus == QUEUE_INFO_OK)
        {
            if(pUartProtocolInfo->m_pComInfo->com_status)
            {
                nLen = pUartProtocolInfo->CreateSendBuffer(pUartProtocolInfo->GetId(), SendBufferInfo.m_nSize,
                                                           SendBufferInfo.m_pBuffer, SendBufferInfo.m_IsWriteThrough);
                Sendbuf += byteArrayToHexString("Sendbuf:", tx_buffer, nLen, "\n");
                pUartProtocolInfo->DeviceWrite(tx_buffer, nLen);

                //通知主线程更新窗口
                emit send_edit_test(Sendbuf);
                Sendbuf.clear();


                while(1)
                {
                    nLen = pUartProtocolInfo->DeviceRead(rx_buffer, BUFF_CACHE_SIZE);
                    if(nLen != 0)
                        break;
                }

                Sendbuf += byteArrayToHexString("Recvbuf:", rx_buffer, nLen, "\n");
                //通知主线程更新窗口
                emit send_edit_test(Sendbuf);
                //qDebug()<<Sendbuf;
                Sendbuf.clear();
            }

            qDebug()<<"uart thread queue test OK";
        }
    }
}

//检测接收到的数据
int CUartProtocolInfo::CheckReceiveData(void)
{
  return 0;
}

//设备写数据
int CUartProtocolInfo::DeviceWrite(uint8_t *pStart, uint16_t nSize)
{
    m_pComInfo->com->write((char *)pStart, nSize);
    return nSize;
}

//设备读数据
int CUartProtocolInfo::DeviceRead(uint8_t *pStart, uint16_t nMaxSize)
{
    return m_pComInfo->com->read((char *)pStart, nMaxSize);
}

//任务初始化
void UartThreadInit(void)
{
    CComInfo *pComInfo;
    CProtocolQueue *pUartQueue;
    CUartThread *pUartThread;

    pComInfo = new CComInfo();
    pUartThread = new CUartThread();
    pUartQueue = new CProtocolQueue();
    pUartProtocolInfo = new CUartProtocolInfo(rx_buffer, tx_buffer, pComInfo, pUartQueue,
                                              pUartThread, BUFF_CACHE_SIZE);
}

CUartProtocolInfo *GetUartProtocolInfo(void)
{
    return pUartProtocolInfo;
}

//投递变量到消息队列
int UartPostQueue(SSendBuffer *pSendBuffer)
{
     return pUartProtocolInfo->m_pUartQueue->QueuePost(pSendBuffer);
}
