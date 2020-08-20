#ifndef CUartThread_H_H
#define CUartThread_H_H

#include <QThread>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"
#include "qextserialport/qextserialport.h"

class CUartProtocolInfo:public QWidget, public CProtocolInfo
{
    Q_OBJECT

public:
    CUartProtocolInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer, int nMaxBufSize):
        CProtocolInfo(pRxBuffer, pTxBuffer, nMaxBufSize){
    }
    ~CUartProtocolInfo(){
    }

    int DeviceRead(uint8_t *pStart, uint16_t nMaxSize){
        return m_pSerialPortCom->read((char *)pStart, nMaxSize);
    }

    int DeviceWrite(uint8_t *pStart, uint16_t nSize){
        m_pSerialPortCom->write((char *)pStart, nSize);
        return nSize;
    }

    int UartLoopThread(SSendBuffer *pSendbuffer);

    volatile bool m_bComStatus{false};
    QextSerialPort *m_pSerialPortCom;

signals:
    void send_edit_recv(QString);
    void send_edit_test(QString);
};

void UartThreadInit(void);
CUartProtocolInfo *GetUartProtocolInfo(void);
#endif // CUartThread_H_H
