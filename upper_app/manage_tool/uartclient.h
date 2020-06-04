#ifndef CUartThread_H_H
#define CUartThread_H_H

#include <QThread>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"
#include "qextserialport/qextserialport.h"

class CUartProtocolThreadInfo:public QThread, public CProtocolInfo
{
    Q_OBJECT

public:
    CUartProtocolThreadInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer, int nMaxBufSize):
        CProtocolInfo(pRxBuffer, pTxBuffer, nMaxBufSize){
    }
    ~CUartProtocolThreadInfo(){
    }

    int DeviceRead(uint8_t *pStart, uint16_t nMaxSize);
    int DeviceWrite(uint8_t *pStart, uint16_t nSize);

    void CloseThread()
    {
        m_nIsStop = 0;
    }

    //串口的硬件信息
    volatile bool m_bComStatus{false};
    QextSerialPort *m_pSerialPortCom;

protected:
    virtual void run();

signals:
    void send_edit_recv(QString);
    void send_edit_test(QString);

private:
    volatile bool m_nIsStop{false};
};

void UartThreadInit(void);
CUartProtocolThreadInfo *GetUartProtocolInfo(void);
#endif // CUartThread_H_H
