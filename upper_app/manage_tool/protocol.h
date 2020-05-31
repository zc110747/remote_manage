#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "typedef.h"
#include <QMutex>

#define PROTOCOL_SEND_HEAD  0x5A
#define BUFF_CACHE_SIZE     1200

#define PROTOCOL_RECV_HEAD  0x5B


#define MAX_QUEUE            20
#define QUEUE_INFO_OK        0
#define QUEUE_INFO_FULL     -1
#define QUEUE_INFO_INVALID  -2
#define QUEUE_INFO_EMPTY    -3

enum PROTOCOL_STATUS
{
    PROTOCOL_NULL = 0,
    PROTOCOL_UART,
    PROTOCOL_SOCKET
};

class SSendBuffer
{
public:
    SSendBuffer(uint8_t *pBuffer = nullptr, int nSize = 0, int nCommand = 0, bool bWriteThrough = false){
        m_nSize = nSize;
        m_pBuffer = pBuffer;
        m_IsWriteThrough = bWriteThrough;
        m_nCommand = nCommand;
    }
    ~SSendBuffer(){
    }

    int m_nSize;
    int m_nCommand;
    uint8_t m_IsWriteThrough;
    uint8_t *m_pBuffer;
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

    void clear();
    int QueuePost(SSendBuffer *pSendBuffer);
    int QueuePend(SSendBuffer *pSendbuffer);
private:
    volatile int m_nFreeList;
    volatile int m_nWriteIndex;
    volatile int m_nReadIndex;
    SSendBuffer *qinfo_ptr[MAX_QUEUE];
    QMutex *m_qLockMutex;
};

class CProtocolInfo
{
public:
    CProtocolInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer);
    ~CProtocolInfo(){};

    int CreateSendBuffer(uint8_t nId, uint16_t nSize, uint8_t *pStart, bool bWriteThrough);
    uint16_t CaclcuCrcVal(uint8_t *pStart, int nSize);
    uint16_t GetId(void){
        return m_nId;
    }
    void SetId(uint16_t nCurId){
        m_nId = nCurId;
    }

    virtual int DeviceRead(uint8_t *pStart, uint16_t nMaxSize) = 0;
    virtual int DeviceWrite(uint8_t *pStart, uint16_t nSize) = 0;
    virtual int CheckReceiveData(void) = 0;
private:
    uint8_t *m_pRxBuffer;
    uint8_t *m_pTxBuffer;  
    uint16_t m_nPacketId;
    uint16_t m_nId;
};

#endif // PROTOCOL_H
