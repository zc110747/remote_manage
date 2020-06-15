/*!
    tcp客户端的应用实现
*/
#include "tcpclient.h"

static uint8_t rx_buffer[BUFF_CACHE_SIZE];
static uint8_t tx_buffer[BUFF_CACHE_SIZE];
static CTcpSocketInfo *pCTcpSocketInfo;
//static SSendBuffer SendBufferInfo;
static SSendBuffer *pSendBufferInfo;

/*!
    Socket连接时执行的槽函数
*/
void CTcpSocketInfo::slotConnected()
{
    qDebug()<<"Tcpclient.cpp:Socket Connect";
}

/*!
    Socket断开时执行的槽函数
*/
void CTcpSocketInfo::slotDisconnected()
{
    qDebug()<<"Tcpclient.cpp:Socket Disconnected";
}

/*!
    Tcp Socket数据接收时执行的回调函数
*/
void CTcpSocketInfo::dataReceived()
{
    if(this->CheckReceiveData(false) == RT_OK)
    {
       #if TEST_DEBUG == 1
       emit send_edit_test(byteArrayToHexString("Recv Buf:",
            m_pRxBuffer, m_RxBufSize, "\n"));
       #endif
       if(pSendBufferInfo->m_pFunc != nullptr)
       {
            emit send_edit_recv(pSendBufferInfo->m_pFunc(m_pRxDataBuffer, m_RxBufSize-RECV_DATA_HEAD));
       }
    }
    m_pSemphore->release();
}

/*!
    Tcp Socket循环的应用执行
*/
int CTcpSocketInfo::TcpClientSocketLoopThread(SSendBuffer *pSendbuffer)
{
    bool is_connect;
    int nLen;

    pSendBufferInfo = pSendbuffer;

    if(pSendBufferInfo->m_bUploadStatus == false)
    {
        m_pTcpSocket->abort();
        if(m_pTcpSocket->state() != QAbstractSocket::ConnectedState)
        {
            m_pTcpSocket->connectToHost(*m_pServerIp, m_nPort);
            is_connect = m_pTcpSocket->waitForConnected(300);
        }
        else
        {
            is_connect = true;
        }
    }
    else
    {
        is_connect = true;
    }

    nLen = this->CreateSendBuffer(this->GetId(), pSendbuffer->m_nSize,
                                  pSendbuffer->m_pBuffer, pSendbuffer->m_IsWriteThrough);
    if(is_connect)
    {
        #if TEST_DEBUG == 1
        emit send_edit_test(QString("tcp socket client ok"));
        #endif
        this->DeviceWrite(tx_buffer, nLen);

        //通知主线程更新窗口
        #if TEST_DEBUG == 1
        emit send_edit_test(byteArrayToHexString("Sendbuf:", tx_buffer, nLen, "\n"));
        #endif

        //等待发送和接收完成
        m_pTcpSocket->waitForBytesWritten(1000);

        if(pSendBufferInfo->m_bUploadStatus)
        {
            m_pTcpSocket->waitForReadyRead();
        }
        else
        {
            m_pTcpSocket->waitForReadyRead(2000);
        }

        if(m_pSemphore->tryAcquire(1, 10000))
        {
            #if TEST_DEBUG == 1
            qDebug()<<"Tcpclient.cpp:Semphore Read";
            #endif
        }
        else
        {
            #if TEST_DEBUG == 1
            qDebug()<<"Tcpclient.cpp:Semphore Read Failed";
            #endif
        }
    }
    else
    {
        emit send_edit_test(QString("socket client fail\n"));
        return RT_FAIL;
    }
    return RT_OK;
}

/*!
    Tcp Socket应用线程执行初始化
*/
void CTcpSocketInfo::TcpClientSocketInitForThread(void)
{
    m_pTcpSocket = new QTcpSocket();
    m_pServerIp = new QHostAddress();
    connect(m_pTcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

/*!
    Tcp Socket应用初始化
*/
void TcpClientSocketInit(void)
{
    pCTcpSocketInfo = new CTcpSocketInfo(rx_buffer, tx_buffer, BUFF_CACHE_SIZE);
}

/*!
    获取Tcp Socket的信息数据结构
*/
CTcpSocketInfo *GetTcpClientSocketInfo()
{
    return pCTcpSocketInfo;
}
