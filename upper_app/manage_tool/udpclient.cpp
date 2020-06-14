/*!
    udp客户端的应用实现
*/
#include "udpclient.h"
#include "commandinfo.h"

static uint8_t rx_buffer[BUFF_CACHE_SIZE];
static uint8_t tx_buffer[BUFF_CACHE_SIZE];
static CUdpSocketInfo *pCUdpSocketClientInfo;
static SSendBuffer *pSendBufferInfo;

/*!
    Udp Socket数据接收时执行的回调函数
*/
void CUdpSocketInfo::dataReceived()
{
    emit send_edit_test(QString("Udp Socket Recv Ok"));
    if(this->CheckReceiveData(true) == RT_OK)
    {
       emit send_edit_test(byteArrayToHexString("Recv Buf:",
            m_pRxBuffer, m_RxBufSize, "\n"));
       if(pSendBufferInfo->m_pFunc != nullptr)
       {
            emit send_edit_recv(pSendBufferInfo->m_pFunc(m_pRxDataBuffer, m_RxBufSize-RECV_DATA_HEAD));
       }
    }
    m_pSemphore->release();
}

/*!
    Udp Socket应用线程执行初始化
*/
void CUdpSocketInfo::UdpClientSocketInitForThread(void)
{
    m_pUdpSocket = new QUdpSocket();
    m_pServerIp = new QHostAddress();
    m_pLocalIp = new QHostAddress();
    connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

/*!
    Udp Socket循环的应用执行
*/
int CUdpSocketInfo::UdpClientSocketLoopThread(SSendBuffer *pSendbuffer)
{
    quint16 nLen;

    if(pSendbuffer->m_nCommand != ABORT_CMD)
    {
        pSendBufferInfo = pSendbuffer;
        nLen = CreateSendBuffer(this->GetId(), pSendbuffer->m_nSize,
                               pSendbuffer->m_pBuffer, pSendbuffer->m_IsWriteThrough);

        //绑定到指定的UDP端口,用于选择发送的网卡
        if(m_pUdpSocket->state() != QAbstractSocket::BoundState)
        {
            /*绑定本地端口失败*/
            if(m_pUdpSocket->bind(*m_pLocalIp, UDP_DEFAULT_PORT) != true)
            {
                qDebug()<<*m_pLocalIp<<UDP_DEFAULT_PORT<<m_pUdpSocket->state();
                emit send_edit_test(QString("Udp Bind Socket failed"));
                //return RT_FAIL;
            }
        }

        qDebug()<<this->DeviceWrite(tx_buffer, nLen);

        emit send_edit_test(QString("Udp Socket Send Ok"));
        //通知主线程更新窗口
        emit send_edit_test(byteArrayToHexString("Sendbuf:", tx_buffer, nLen, "\n"));

        //等待发送和接收完成
        m_pUdpSocket->waitForBytesWritten(100);

        if(pSendBufferInfo->m_bUploadStatus)
        {
            m_pUdpSocket->waitForReadyRead();
        }
        else
        {
            m_pUdpSocket->waitForReadyRead(2000);
        }

        if(m_pSemphore->tryAcquire(1, 10000))
        {
            qDebug()<<"Udpclient.cpp:Semphore Read";
        }
        else
        {
            qDebug()<<"Udpclient.cpp:Semphore Read Failed";
        }
    }
    else
    {
        emit send_edit_test(QString("Udp Socket Close"));
        if(m_pUdpSocket->state() != QAbstractSocket::UnconnectedState)
        {
            qDebug()<<"Udp Socket abort";
            m_pUdpSocket->abort();
        }
    }
    return RT_OK;
}
/*!
    Udp Socket应用线程初始化
*/
void UdpSocketInfoInit(void)
{
    pCUdpSocketClientInfo = new CUdpSocketInfo(rx_buffer, tx_buffer, BUFF_CACHE_SIZE);
}

/*!
    获取Udp Socket的信息数据结构
*/
CUdpSocketInfo *GetUdpClientSocketInfo(void)
{
    return pCUdpSocketClientInfo;
}
