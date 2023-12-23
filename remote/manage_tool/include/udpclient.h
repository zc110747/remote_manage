#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"

class CUdpSocketInfo:public QWidget, public protocol_info
{
    Q_OBJECT

public:
    CUdpSocketInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer, int nMaxBufSize):
        protocol_info(pRxBuffer, pTxBuffer, nMaxBufSize){
        m_pSemphore = new QSemaphore(0);
    }
    ~CUdpSocketInfo()
    {
        m_pUdpSocket->deleteLater();
        delete  m_pSemphore;
        delete  m_pServerIp;
    }

    int DeviceRead(uint8_t *pStart, uint16_t nMaxSize){
        uint16_t nReadSize = 0;

        if (m_pUdpSocket->hasPendingDatagrams())
        {
          nReadSize = m_pUdpSocket->readDatagram((char *)pStart, nMaxSize, m_pServerIp, &m_nPort);
        }
        return nReadSize;
    }

    int DeviceWrite(uint8_t *pStart, uint16_t nSize){
        //qDebug()<<*m_pServerIp<<"Port"<<m_nPort;

        return m_pUdpSocket->writeDatagram((char *)pStart, nSize, *m_pServerIp, m_nPort);
    }

    void SetSocketInfo(QString SServerIpAddress, QString SLocalIpAddress, quint16 nPort)
    {
        m_nPort = nPort;
        if (!m_pServerIp->setAddress(SServerIpAddress)){
            qDebug()<<"SetAddress error\n";
        }

        if (!m_pLocalIp->setAddress(SLocalIpAddress)){
            qDebug()<<"SetAddress error\n";
        }
        //qDebug()<<SServerIpAddress<<"Port"<<nPort;
    }

    void UdpSocketClose(void){
          m_pUdpSocket->abort();
    }

    int UdpClientSocketLoopThread(SSendBuffer *pSendbuffer);
    void UdpClientSocketInitForThread();
    QUdpSocket *m_pUdpSocket;
    QHostAddress *m_pServerIp;
    QHostAddress *m_pLocalIp;
    quint16 m_nPort;
    QSemaphore *m_pSemphore;

private:
    bool status{false};

signals:
    void send_edit_recv(QString);
    void send_edit_test(QString);

public slots:
    void dataReceived();
};

void UdpSocketInfoInit(void);
CUdpSocketInfo *GetUdpClientSocketInfo(void);
#endif // UDPCLIENT_H
