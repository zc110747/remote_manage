#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "typedef.h"
#include <QMutex>
#include <QThread>

class SSendBuffer
{
public:
    SSendBuffer(uint8_t *pBuffer = nullptr, int nSize = 0, int nCommand = 0, bool bWriteThrough = false,
                std::function<QString(uint8_t *, int)> pFunc = nullptr, PROTOCOL_STATUS nProtocolStatus = PROTOCOL_NULL,
                QString qPathInfo = nullptr){
        m_nSize = nSize;
        m_pBuffer = pBuffer;
        m_IsWriteThrough = bWriteThrough;
        m_nCommand = nCommand;
        m_pFunc = pFunc;
        m_nProtocolStatus = nProtocolStatus;
        m_qPathInfo = qPathInfo;
    }
    ~SSendBuffer(){
    }

    bool m_bUploadStatus{false};
    uint8_t *m_pBuffer;
    int m_nSize;
    int m_nCommand;
    uint8_t m_IsWriteThrough;
    QString m_qPathInfo;
    std::function<QString(uint8_t *, int)> m_pFunc;
    PROTOCOL_STATUS m_nProtocolStatus;
};

class CProtocolQueue
{
public:
    CProtocolQueue(){
        m_nFreeList = MAX_QUEUE;
        m_nWriteIndex = 0;
        m_nReadIndex = 0;
        m_qLockMutex = new QMutex;
    }
    ~CProtocolQueue(){
    };

    bool isEmpty(){
        int m_nSize;
        m_qLockMutex->lock();
        m_nSize = m_nFreeList;
        m_qLockMutex->unlock();

        if(m_nSize == MAX_QUEUE){
            return true;
        }
        return false;
    }

    void clear(){
        m_qLockMutex->lock();
        m_nFreeList = MAX_QUEUE;
        m_nWriteIndex = 0;
        m_nReadIndex = 0;
        m_qLockMutex->unlock();
    }

    int QueuePost(SSendBuffer *pSendBuffer)
    {
        if(m_nFreeList == 0)
        {
            return QUEUE_INFO_FULL;
        }

        m_qLockMutex->lock();
        qinfo_ptr[m_nWriteIndex] = pSendBuffer;
        m_nWriteIndex++;

        //队列循环
        if(m_nWriteIndex == MAX_QUEUE){
            m_nWriteIndex = 0;
        }
        m_nFreeList--;
        m_qLockMutex->unlock();

        return QUEUE_INFO_OK;
    }

    int QueuePend(SSendBuffer *pSendbuffer){
        if(m_nFreeList < MAX_QUEUE)
        {
            m_qLockMutex->lock();
            pSendbuffer->m_pBuffer = qinfo_ptr[m_nReadIndex]->m_pBuffer;
            pSendbuffer->m_nSize  = qinfo_ptr[m_nReadIndex]->m_nSize;
            pSendbuffer->m_IsWriteThrough =  qinfo_ptr[m_nReadIndex]->m_IsWriteThrough;
            pSendbuffer->m_nCommand = qinfo_ptr[m_nReadIndex]->m_nCommand;
            pSendbuffer->m_pFunc = qinfo_ptr[m_nReadIndex]->m_pFunc;
            pSendbuffer->m_nProtocolStatus = qinfo_ptr[m_nReadIndex]->m_nProtocolStatus;
            pSendbuffer->m_qPathInfo = qinfo_ptr[m_nReadIndex]->m_qPathInfo;

            delete qinfo_ptr[m_nReadIndex];
            qinfo_ptr[m_nReadIndex] = nullptr;
            m_nReadIndex++;

            //队列循环
            if(m_nReadIndex == MAX_QUEUE){
                m_nReadIndex = 0;
            }
            m_nFreeList++;
            m_qLockMutex->unlock();
            //qDebug()<<"queue receive";
            return  QUEUE_INFO_OK;
        }
        else{
            QThread::msleep(100);
            return QUEUE_INFO_EMPTY;
        }
    };
private:
    volatile int m_nFreeList;
    volatile int m_nWriteIndex;
    volatile int m_nReadIndex;
    SSendBuffer *qinfo_ptr[MAX_QUEUE];
    QMutex *m_qLockMutex;
};

class protocol_info
{
public:
    protocol_info(uint8_t *pRxBuffer, uint8_t *pTxBuffer, uint8_t nMaxBufSize){
        m_pRxBuffer = pRxBuffer;
        m_pRxDataBuffer = &pRxBuffer[RECV_DATA_HEAD];
        m_pTxBuffer = pTxBuffer;
        m_MaxBufSize = nMaxBufSize;
        m_pQueue = new CProtocolQueue();
    };
    ~protocol_info(){
        delete  m_pQueue;
        m_pQueue = nullptr;
    };

    int CreateSendBuffer(uint8_t nId, uint16_t nSize, uint8_t *pStart, bool bWriteThrough);
    uint16_t calculate_crc(uint8_t *pStart, int nSize);
    uint16_t GetId(void){
        return m_nId;
    }
    void SetId(uint16_t nCurId){
        m_nId = nCurId;
    }
    int CheckReceiveData(bool IsSignalCheckHead);

    int ExecutCommand(SSendBuffer &sBuffer, int nSize);

    virtual int DeviceRead(uint8_t *pStart, uint16_t nMaxSize) = 0;
    virtual int DeviceWrite(uint8_t *pStart, uint16_t nSize) = 0;

    //socket处理的应用
    int PostQueue(SSendBuffer *pSendBuffer)
    {
        if(m_pQueue != nullptr)
        {
            return m_pQueue->QueuePost(pSendBuffer);
        }
        return QUEUE_INFO_INVALID;
    }

    uint8_t *m_pRxBuffer;
    uint8_t *m_pRxDataBuffer;
    uint8_t *m_pTxBuffer;
    int m_RxBufSize{0};  //接收到缓存区总长度
    int m_RxDataSize{0}; //接收到数据区长度
    CProtocolQueue *m_pQueue;

private:
    uint16_t m_nPacketId{0};
    uint16_t m_nId{0};
    int m_RxTimout{0};
    int m_MaxBufSize;
};

#endif // PROTOCOL_H
