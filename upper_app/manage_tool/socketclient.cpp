
#include "socketclient.h"


static uint8_t rx_buffer[BUFF_CACHE_SIZE];
static uint8_t tx_buffer[BUFF_CACHE_SIZE];
static CTcpClientSocketInfo *pCTcpClientSocketInfo;

CTcpClientSocketInfo::CTcpClientSocketInfo(QObject *parent, uint8_t *pRxBuffer, uint8_t *pTxBuffer):QObject(parent), CProtocolInfo(pRxBuffer, pTxBuffer)
{
    status = false;

    serverIP = new QHostAddress();
    m_pSocketQueue = new CProtocolQueue();
    m_pThread = new CClientSocketThread();
}

CTcpClientSocketInfo::~CTcpClientSocketInfo()
{
    tcpSocket->deleteLater();
    delete  m_pSocketQueue;
    delete  m_pThread;
    delete  serverIP;
}

void CTcpClientSocketInfo::slotConnected()
{
    qDebug()<<"socket connect\n";
}

void CTcpClientSocketInfo::slotDisconnected()
{
    qDebug()<<"socket disconnected\n";
}

int CTcpClientSocketInfo::CheckReceiveData(void)
{
  return 0;
}

//接收数据
void CTcpClientSocketInfo::dataReceived()
{
    qDebug()<<"socket recv:"<<"\n";
    while(tcpSocket->bytesAvailable() > 0)
    {
        QByteArray datagram;
        datagram.resize(tcpSocket->bytesAvailable());

        DeviceRead((uint8_t *)datagram.data(), datagram.size());

        QString msg = datagram.data();
    }
    tcpSocket->disconnectFromHost();
}

CClientSocketThread::CClientSocketThread(QObject *parent):QThread(parent)
{
   m_nIsStop = 0;
}

//关闭任务
void CClientSocketThread::CloseThread()
{
    m_nIsStop = 1;
}

void CClientSocketThread::run()
{
    SSendBuffer SendBufferInfo;
    QTcpSocket *pTcpSocket;
    bool is_connect;
    int nLen;
    QString SendBuf;
    int nStatus;

    pCTcpClientSocketInfo->tcpSocket = new QTcpSocket();
    connect(pCTcpClientSocketInfo->tcpSocket, SIGNAL(connected()), pCTcpClientSocketInfo, SLOT(slotConnected()));
    connect(pCTcpClientSocketInfo->tcpSocket, SIGNAL(disconnected()), pCTcpClientSocketInfo, SLOT(slotDisconnected()));
    connect(pCTcpClientSocketInfo->tcpSocket, SIGNAL(readyRead()), pCTcpClientSocketInfo, SLOT(dataReceived()));

    for(;;)
    {
        if(m_nIsStop)
            return;

        nStatus  = pCTcpClientSocketInfo->m_pSocketQueue->QueuePend(&SendBufferInfo);
        if(nStatus == QUEUE_INFO_OK)
        {
            pTcpSocket = pCTcpClientSocketInfo->tcpSocket;
            pTcpSocket->abort();
            pTcpSocket->connectToHost(*pCTcpClientSocketInfo->serverIP, pCTcpClientSocketInfo->m_nPort);
            nLen = pCTcpClientSocketInfo->CreateSendBuffer(pCTcpClientSocketInfo->GetId(), SendBufferInfo.m_nSize,
                                                           SendBufferInfo.m_pBuffer, SendBufferInfo.m_IsWriteThrough);
            is_connect = pTcpSocket->waitForConnected(300);
            if(is_connect)
            {
                emit send_edit_test(QString("socket client ok"));
                pCTcpClientSocketInfo->DeviceWrite(tx_buffer, nLen);

                //通知主线程更新窗口
                SendBuf = byteArrayToHexString("Sendbuf:", tx_buffer, nLen, "\n");
                emit send_edit_test(SendBuf);
                pTcpSocket->waitForBytesWritten();
            }
            else
            {
                emit send_edit_test(QString("socket client fail\n"));
            }
            qDebug()<<"thread queue test OK\n";
        }
    }
}

//socket处理的应用
int TcpClientPostQueue(SSendBuffer *pSendBuffer)
{
    if(pCTcpClientSocketInfo->m_pSocketQueue != nullptr)
    {
        return pCTcpClientSocketInfo->m_pSocketQueue->QueuePost(pSendBuffer);
    }
    return QUEUE_INFO_INVALID;
}


void TcpClientSocketInit(void)
{
    pCTcpClientSocketInfo = new CTcpClientSocketInfo((QObject *)0, rx_buffer, tx_buffer);
}

CTcpClientSocketInfo *GetTcpClientSocket()
{
    return pCTcpClientSocketInfo;
}
