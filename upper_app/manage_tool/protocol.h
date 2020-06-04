#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "typedef.h"
#include <QMutex>

//协议相关的指令
#define PROTOCOL_SEND_HEAD          0x5A
#define PROTOCOL_RECV_HEAD          0x5B
#define PROTOCOL_RECV_HEAD_SIZE     3
#define PROTOCOL_CRC_SIZE           2
#define PROTOCOL_TIMEOUT            4000

//缓存的大小
#define BUFF_CACHE_SIZE     1200

#define RT_OK               0
#define RT_EMPTY            -1
#define RT_TIMEOUT          -2
#define RT_CRC_ERROR        -3

//队列相关的信息
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
    CProtocolInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer, uint8_t nMaxBufSize){
        m_pRxBuffer = pRxBuffer;
        m_pTxBuffer = pTxBuffer;
        m_MaxBufSize = nMaxBufSize;
    };
    ~CProtocolInfo(){};

    int CreateSendBuffer(uint8_t nId, uint16_t nSize, uint8_t *pStart, bool bWriteThrough);
    uint16_t CrcCalculate(uint8_t *pStart, int nSize);
    uint16_t GetId(void){
        return m_nId;
    }
    void SetId(uint16_t nCurId){
        m_nId = nCurId;
    }
    int CheckReceiveData(void);
    int ExecutCommand(SSendBuffer &sBuffer, int nSize);

    virtual int DeviceRead(uint8_t *pStart, uint16_t nMaxSize) = 0;
    virtual int DeviceWrite(uint8_t *pStart, uint16_t nSize) = 0;

    uint8_t *m_pRxBuffer;
    uint8_t *m_pTxBuffer;
    int m_RxBufSize{0};  //接收到缓存区总长度
    int m_RxDataSize{0}; //接收到数据区长度

private:
    uint16_t m_nPacketId{0};
    uint16_t m_nId{0};
    int m_RxTimout{0};
    int m_MaxBufSize;
};

#endif // PROTOCOL_H
