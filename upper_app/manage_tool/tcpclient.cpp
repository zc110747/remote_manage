
#include "tcpclient.h"


static uint8_t rx_buffer[BUFF_CACHE_SIZE];
static uint8_t tx_buffer[BUFF_CACHE_SIZE];
static CTcpSocketThreadInfo *pCTcpSocketThreadInfo;
static SSendBuffer SendBufferInfo;

CTcpSocketThreadInfo::CTcpSocketThreadInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer, int nMaxBufSize):
    CProtocolInfo(pRxBuffer, pTxBuffer, nMaxBufSize)
{
}

CTcpSocketThreadInfo::~CTcpSocketThreadInfo()
{
    m_pTcpSocket->deleteLater();
    delete  m_pServerIp;
}

void CTcpSocketThreadInfo::slotConnected()
{
    qDebug()<<"socket connect\n";
}

void CTcpSocketThreadInfo::slotDisconnected()
{
    qDebug()<<"socket disconnected\n";
}

//接收数据
void CTcpSocketThreadInfo::dataReceived()
{
    if(this->CheckReceiveData() == RT_OK)
    {
       emit send_edit_test(byteArrayToHexString("Recv Buf:",
            m_pRxBuffer, m_RxBufSize, "\n"));
       if(SendBufferInfo.m_pFunc != nullptr)
       {
            emit send_edit_recv(SendBufferInfo.m_pFunc(m_pRxBuffer, m_RxBufSize));
       }
    }
}

void CTcpSocketThreadInfo::run()
{
    bool is_connect;
    int nLen;
    int nStatus;

    m_pTcpSocket = new QTcpSocket();
    m_pServerIp = new QHostAddress();
    connect(m_pTcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

    for(;;)
    {
        if(m_nIsStop)
            return;

        nStatus  = m_pQueue->QueuePend(&SendBufferInfo);
        if(nStatus == QUEUE_INFO_OK)
        {
            m_pTcpSocket->abort();
            m_pTcpSocket->connectToHost(*m_pServerIp, m_nPort);
            nLen = this->CreateSendBuffer(this->GetId(), SendBufferInfo.m_nSize,
                                                           SendBufferInfo.m_pBuffer, SendBufferInfo.m_IsWriteThrough);
            is_connect = m_pTcpSocket->waitForConnected(300);
            if(is_connect)
            {
                emit send_edit_test(QString("socket client ok"));
                this->DeviceWrite(tx_buffer, nLen);

                //通知主线程更新窗口
                emit send_edit_test(byteArrayToHexString("Sendbuf:", tx_buffer, nLen, "\n"));

                //等待发送和接收完成
                m_pTcpSocket->waitForBytesWritten();
                m_pTcpSocket->waitForReadyRead();

            }
            else
            {
                emit send_edit_test(QString("socket client fail\n"));
            }
            qDebug()<<"thread queue test OK\n";
        }
    }
}

void TcpClientSocketInit(void)
{
    pCTcpSocketThreadInfo = new CTcpSocketThreadInfo(rx_buffer, tx_buffer, BUFF_CACHE_SIZE);
}

CTcpSocketThreadInfo *GetTcpClientSocket()
{
    return pCTcpSocketThreadInfo;
}
