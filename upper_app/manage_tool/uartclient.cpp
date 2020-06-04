//uart任务处理
#include "uartclient.h"
#include "mainwindow.h"

static CUartProtocolThreadInfo *pUartProtocolInfo;
static uint8_t rx_buffer[BUFF_CACHE_SIZE];
static uint8_t tx_buffer[BUFF_CACHE_SIZE];

//任务执行函数
void CUartProtocolThreadInfo::run()
{
    SSendBuffer SendBufferInfo;
    int nLen;
    int nStatus;

    for(;;)
    {
        if(m_nIsStop)
            return;

        nStatus  = m_pQueue->QueuePend(&SendBufferInfo);
        if(nStatus == QUEUE_INFO_OK)
        {
            if(m_bComStatus)
            {
                nLen = this->CreateSendBuffer(this->GetId(), SendBufferInfo.m_nSize,
                                                           SendBufferInfo.m_pBuffer, SendBufferInfo.m_IsWriteThrough);
                this->DeviceWrite(tx_buffer, nLen);
                emit send_edit_test(byteArrayToHexString("Sendbuf:", tx_buffer, nLen, "\n"));

                if(this->CheckReceiveData() == RT_OK)
                {
                   emit send_edit_test(byteArrayToHexString("Recv Buf:",
                        m_pRxBuffer, m_RxBufSize, "\n"));
                }
            }

            qDebug()<<"uart thread queue test OK";
        }
    }
}

//设备写数据
int CUartProtocolThreadInfo::DeviceWrite(uint8_t *pStart, uint16_t nSize)
{
    m_pSerialPortCom->write((char *)pStart, nSize);
    return nSize;
}

//设备读数据
int CUartProtocolThreadInfo::DeviceRead(uint8_t *pStart, uint16_t nMaxSize)
{
    return m_pSerialPortCom->read((char *)pStart, nMaxSize);
}

//任务初始化
void UartThreadInit(void)
{
    pUartProtocolInfo = new CUartProtocolThreadInfo(rx_buffer, tx_buffer, BUFF_CACHE_SIZE);
}

CUartProtocolThreadInfo *GetUartProtocolInfo(void)
{
    return pUartProtocolInfo;
}
