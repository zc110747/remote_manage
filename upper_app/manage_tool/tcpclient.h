#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"

class CTcpSocketThreadInfo:public QThread, public CProtocolInfo
{
    Q_OBJECT

public:
    CTcpSocketThreadInfo(uint8_t *pRxBuffer=nullptr, uint8_t *pTxBuffer=nullptr, int nMaxBufSize = 0);
    ~CTcpSocketThreadInfo();
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
    void CloseThread()
    {
        m_nIsStop = 1;
    }

    QTcpSocket *m_pTcpSocket;
    QHostAddress *m_pServerIp;
    int m_nPort;

protected:
    virtual void run();

private:
    bool status{false};
    volatile bool m_nIsStop{0};

signals:
    void send_edit_recv(QString);
    void send_edit_test(QString);

public slots:
    void slotConnected();
    void slotDisconnected();
    void dataReceived();
};

void TcpClientSocketInit(void);
CTcpSocketThreadInfo *GetTcpClientSocket();

#endif // SOCKETCLIENT_H
