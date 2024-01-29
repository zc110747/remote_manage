/*!
    Uart通讯的线程处理和回调执行实现
*/
#include "uartclient.h"
#include "mainwindow.h"

static CUartProtocolInfo *pUartProtocolInfo;
static uint8_t rx_buffer[BUFF_CACHE_SIZE];
static uint8_t tx_buffer[BUFF_CACHE_SIZE];

/*!
    uart线程循环的应用执行
*/
int CUartProtocolInfo::UartLoopThread(SSendBuffer *pSendbuffer)
{
    int nLen;

    if (m_bComStatus)
    {
        nLen = this->CreateSendBuffer(this->GetId(), pSendbuffer->m_nSize,
                                                   pSendbuffer->m_pBuffer, pSendbuffer->m_IsWriteThrough);
        this->DeviceWrite(tx_buffer, nLen);
        emit send_edit_test(byteArrayToHexString("Sendbuf:", tx_buffer, nLen, "\n"));

        if (this->CheckReceiveData(false) == RT_OK)
        {
           emit send_edit_test(byteArrayToHexString("Recv Buf:",
                m_pRxBuffer, m_RxBufSize, "\n"));
            if (pSendbuffer->m_pFunc != nullptr)
            {
                 emit send_edit_recv(pSendbuffer->m_pFunc(m_pRxDataBuffer, m_RxBufSize-RECV_DATA_HEAD));
            }
        }
        else
        {
           emit send_edit_test(QString("Receive Failed"));
           return RT_FAIL;
        }
    }

    qDebug()<<"uart thread queue test OK";
    return RT_OK;
}

/*!
    Uart应用线程初始化
*/
void UartThreadInit(void)
{
    pUartProtocolInfo = new CUartProtocolInfo(rx_buffer, tx_buffer, BUFF_CACHE_SIZE);
}

/*!
    获取Uart的信息数据结构
*/
CUartProtocolInfo *GetUartProtocolInfo(void)
{
    return pUartProtocolInfo;
}
