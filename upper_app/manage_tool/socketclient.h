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


class CClientSocketThread:public QThread
{
    Q_OBJECT

public:
    explicit CClientSocketThread(QObject *parent = 0);
    void CloseThread();

protected:
    virtual void run();

signals:
    void send_edit_recv(QString);
    void send_edit_test(QString);

private:
    volatile bool m_nIsStop;
};

class CTcpClientSocketInfo:public QObject, public CProtocolInfo
{
    Q_OBJECT

public:
    CTcpClientSocketInfo(QObject *parent=0, uint8_t *pRxBuffer=nullptr, uint8_t *pTxBuffer=nullptr);
    ~CTcpClientSocketInfo();
    int DeviceRead(uint8_t *pStart, uint16_t nMaxSize){
        return tcpSocket->read((char *)pStart, nMaxSize);
    };
    int DeviceWrite(uint8_t *pStart, uint16_t nSize){
        return tcpSocket->write((char *)pStart, nSize);
    };
    int CheckReceiveData(void);

    void SetSocketInfo(QString SIpAddress, int nPort)
    {
        if(!serverIP->setAddress(SIpAddress)){
            qDebug()<<"SetAddress error\n";
        }
        m_nPort = nPort;
    }

    CProtocolQueue *m_pSocketQueue;
    CClientSocketThread *m_pThread;
    QTcpSocket *tcpSocket;
    QHostAddress *serverIP;
    int m_nPort;

private:
    bool status;
    QString ipAddr;

public slots:
    void slotConnected();
    void slotDisconnected();
    void dataReceived();
};

void TcpClientSocketInit(void);
int TcpClientPostQueue(SSendBuffer *buf);
CTcpClientSocketInfo *GetTcpClientSocket();

#endif // SOCKETCLIENT_H
