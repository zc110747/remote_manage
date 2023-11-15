#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"

class CTcpSocketInfo:public QWidget, public protocol_info
{
    Q_OBJECT

public:
    CTcpSocketInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer, int nMaxBufSize):
        protocol_info(pRxBuffer, pTxBuffer, nMaxBufSize){
        m_pSemphore = new QSemaphore(0);
    }
    ~CTcpSocketInfo()
    {
        m_pTcpSocket->deleteLater();
        delete m_pSemphore;
        delete m_pServerIp;
    }

    int DeviceRead(uint8_t *pStart, uint16_t nMaxSize){
        uint16_t nReadSize = 0;

        if(m_pTcpSocket->bytesAvailable() > 0)
        {
          nReadSize = m_pTcpSocket->read((char *)pStart, nMaxSize);
        }
        return nReadSize;
    };

    int DeviceWrite(uint8_t *pStart, uint16_t nSize){
        return m_pTcpSocket->write((char *)pStart, nSize);
    };

    void SetSocketInfo(QString SIpAddress, int nPort)
    {
        if(!m_pServerIp->setAddress(SIpAddress)){
            qDebug()<<"SetAddress error\n";
        }
        m_nPort = nPort;
    }

    void TcpClientSocketInitForThread();
    int TcpClientSocketLoopThread(SSendBuffer *pSendbuffer);
    QTcpSocket *m_pTcpSocket;
    QHostAddress *m_pServerIp;
    int m_nPort;
    QSemaphore *m_pSemphore;

private:
    bool status{false};

signals:
    void send_edit_recv(QString);
    void send_edit_test(QString);

public slots:
    void slotConnected();
    void slotDisconnected();
    void dataReceived();
};

void TcpClientSocketInit(void);
CTcpSocketInfo *GetTcpClientSocketInfo();

#endif // SOCKETCLIENT_H
