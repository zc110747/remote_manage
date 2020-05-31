//协议层的实现
#include "protocol.h"
#include <QTime>
#include <QEventLoop>
#include <QThread>
#include <QDebug>

CProtocolInfo::CProtocolInfo(uint8_t *pRxBuffer, uint8_t *pTxBuffer)
{
    m_pRxBuffer = pRxBuffer;
    m_pTxBuffer = pTxBuffer;
}

//生成发送数据报文
int CProtocolInfo::CreateSendBuffer(uint8_t nId, uint16_t nSize, uint8_t *pStart, bool bWriteThrough)
{
    if(m_pTxBuffer != nullptr)
    {
        if(bWriteThrough == false)
        {
            uint8_t nTotalSize, nIndex;
            uint16_t nCrcVal;
            uint16_t random;

            //生成随机数
            qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
            random = qrand()%65536;
            m_nPacketId = random;

            nTotalSize = 0;
            m_pTxBuffer[nTotalSize++] = PROTOCOL_SEND_HEAD;
            m_pTxBuffer[nTotalSize++] = nId;
            m_pTxBuffer[nTotalSize++] = (uint8_t)(random>>8);
            m_pTxBuffer[nTotalSize++] = (uint8_t)(random&0xff);
            m_pTxBuffer[nTotalSize++] = (uint8_t)(nSize>>8);
            m_pTxBuffer[nTotalSize++] = (uint8_t)(nSize&0xff);

            if(nSize != 0 && pStart != NULL)
            {
                for(nIndex=0; nIndex<nSize; nIndex++)
                {
                    m_pTxBuffer[nTotalSize++] = *(pStart+nIndex);
                }
            }

            nCrcVal = CaclcuCrcVal(&m_pTxBuffer[1], nTotalSize-1);
            m_pTxBuffer[nTotalSize++] = (uint8_t)(nCrcVal>>8);
            m_pTxBuffer[nTotalSize++] = (uint8_t)(nCrcVal&0xff);

            return nTotalSize;
         }
         else
         {
            memcpy(m_pTxBuffer, pStart, nSize);
            return nSize;
         }
    }
    else
        return 0;
}

//crc校验实现
uint16_t CProtocolInfo::CaclcuCrcVal(uint8_t *pStart, int nSize)
{
    if(pStart == NULL || nSize == 0)
    {
        return 0;
    }
    return 0xffff;
}

int CProtocolQueue::QueuePost(SSendBuffer *pSendBuffer)
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

int CProtocolQueue::QueuePend(SSendBuffer *pSendbuffer)
{
    if(m_nFreeList < MAX_QUEUE)
    {
        m_qLockMutex->lock();
        pSendbuffer->m_pBuffer = qinfo_ptr[m_nReadIndex]->m_pBuffer;
        pSendbuffer->m_nSize  = qinfo_ptr[m_nReadIndex]->m_nSize;
        pSendbuffer->m_IsWriteThrough =  qinfo_ptr[m_nReadIndex]->m_IsWriteThrough;

        delete qinfo_ptr[m_nReadIndex];
        qinfo_ptr[m_nReadIndex] = nullptr;
        m_nReadIndex++;

        //队列循环
        if(m_nReadIndex == MAX_QUEUE){
            m_nReadIndex = 0;
        }
        m_nFreeList++;
        m_qLockMutex->unlock();
        qDebug()<<"queue receive";
        return  QUEUE_INFO_OK;
    }
    else{
        QThread::msleep(100);
        return QUEUE_INFO_EMPTY;
    }
}

//清空队列
void CProtocolQueue::clear()
{
    m_qLockMutex->lock();
    m_nFreeList = MAX_QUEUE;
    m_nWriteIndex = 0;
    m_nReadIndex = 0;
    m_qLockMutex->unlock();
}
