#ifndef CUartThread_H_H
#define CUartThread_H_H

#include <QThread>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"
#include "qextserialport/qextserialport.h"

class CComInfo
{
public:
    CComInfo(){
    }
    ~CComInfo(){
    }

    QextSerialPort *com;
    bool com_status;
};

class CUartThread:public QThread
{
    Q_OBJECT

public:
    explicit CUartThread(QObject *parent = 0);
    void CloseThread();

protected:
    virtual void run();

signals:
    void send_edit_recv(QString);
    void send_edit_test(QString);

private:
    volatile bool m_nIsStop;
};

class CUartProtocolInfo:public CProtocolInfo
{
public:
    CUartProtocolInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer, CComInfo *pComInfo,
                     CProtocolQueue *pUartQueue, CUartThread *pUartThread):
        CProtocolInfo(pRxBuffer, pTxBuffer){
        m_pComInfo = pComInfo;
        m_pUartQueue = pUartQueue;
        m_pThread = pUartThread;
    }
    ~CUartProtocolInfo(){
        delete m_pComInfo;
        delete m_pUartQueue;
        delete m_pThread;
    }

    CComInfo *m_pComInfo;
    CProtocolQueue *m_pUartQueue;
    CUartThread *m_pThread;

    int CheckReceiveData(void);
    int DeviceRead(uint8_t *pStart, uint16_t nMaxSize);
    int DeviceWrite(uint8_t *pStart, uint16_t nSize);
};

void UartThreadInit(void);
int UartPostQueue(SSendBuffer *pSendBuffer);
CUartProtocolInfo *GetUartProtocolInfo(void);
#endif // CUartThread_H_H
