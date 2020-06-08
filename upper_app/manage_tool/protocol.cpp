/*!
    协议相关的创建，校验和解析接收的应用
*/
#include "protocol.h"
#include <QTime>
#include <QEventLoop>
#include <QRandomGenerator>

/*!
    生成上位机发送数据协议的函数实现
    具体结构:
    协议头 1Byte -- 0x5A
    数据长度 2Byte
    设备ID 1Byte
    数据编号 2Byte
    实际内部数据 数据长度-3
    奇偶校验位 2Byte
*/
int CProtocolInfo::CreateSendBuffer(uint8_t nId, uint16_t nSize, uint8_t *pStart, bool bWriteThrough)
{
    if(m_pTxBuffer != nullptr)
    {
        if(bWriteThrough == false)
        {
            uint8_t nTotalSize, nIndex;
            uint16_t nCrcVal;
            uint16_t random;
            uint16_t nSendSize;

            //生成随机数
            qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
            random = QRandomGenerator::global()->bounded(65536);
            m_nPacketId = random;

            nSendSize = nSize+3;

            nTotalSize = 0;
            m_pTxBuffer[nTotalSize++] = PROTOCOL_SEND_HEAD;
            m_pTxBuffer[nTotalSize++] = (uint8_t)(nSendSize>>8);
            m_pTxBuffer[nTotalSize++] = (uint8_t)(nSendSize&0xff);
            m_pTxBuffer[nTotalSize++] = nId;
            m_pTxBuffer[nTotalSize++] = (uint8_t)(random>>8);
            m_pTxBuffer[nTotalSize++] = (uint8_t)(random&0xff);

            if(nSize != 0 && pStart != NULL)
            {
                for(nIndex=0; nIndex<nSize; nIndex++)
                {
                    m_pTxBuffer[nTotalSize++] = *(pStart+nIndex);
                }
            }

            nCrcVal = CrcCalculate(&m_pTxBuffer[1], nTotalSize-1);
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

/*!
    CRC16校验的代码实现
*/
uint16_t CProtocolInfo::CrcCalculate(uint8_t *pStart, int nSize)
{
    if(pStart == NULL || nSize == 0)
    {
        return 0;
    }
    return 0xffff;
}

/*!
    接收数据并返回处理结果
*/
int CProtocolInfo::CheckReceiveData(bool IsSignalCheckHead)
{
    int nRead;
    int CrcRecv, CrcCacl;

    //回复的初始状态初始状态
    m_RxBufSize = 0;
    m_RxTimout = 0;
    m_RxDataSize = 0;

    do
    {
        if(m_RxBufSize == 0 && IsSignalCheckHead == false)
        {
            nRead = DeviceRead(&m_pRxBuffer[m_RxBufSize], 1);
            if(nRead > 0 && m_pRxBuffer[0] == PROTOCOL_RECV_HEAD)
            {
                m_RxBufSize++;
                m_RxTimout = 0;
            }
            else
            {
                m_RxBufSize = 0;
                m_RxTimout++;
            }
        }

        if(m_RxBufSize > 0 || IsSignalCheckHead == true)
        {
            nRead = DeviceRead(&m_pRxBuffer[m_RxBufSize], m_MaxBufSize-m_RxBufSize);
            if(nRead > 0)
            {
                if(IsSignalCheckHead == true && m_pRxBuffer[0] != PROTOCOL_RECV_HEAD)
                {
                    m_RxBufSize = 0;
                    return RT_FAIL;
                }

                m_RxTimout = 0;
                m_RxBufSize += nRead;
                if(nRead >= PROTOCOL_RECV_HEAD_SIZE)
                {
                    int nLen;

                    m_RxDataSize =  m_pRxBuffer[1]<<8 | m_pRxBuffer[2];
                    nLen = m_RxDataSize + PROTOCOL_RECV_HEAD_SIZE + PROTOCOL_CRC_SIZE;
                    if(m_RxBufSize >= nLen)
                    {
                        /*计算head后到CRC尾之前的所有数据的CRC值*/
                        CrcRecv = (m_pRxBuffer[nLen-2]<<8) + m_pRxBuffer[nLen-1];
                        CrcCacl = CrcCalculate(&m_pRxBuffer[1], nLen-PROTOCOL_CRC_SIZE-1);
                        if(CrcRecv == CrcCacl)
                        {
                            qDebug()<<QString("Receive Ok");
                            break;
                        }
                        else
                        {
                            qDebug()<<QString("CRC err, Recv:%1, Cacl:%2").arg(CrcRecv, CrcCacl);
                            return RT_CRC_ERROR;
                        }
                    }
                }
            }
            else
            {
                m_RxTimout++;
            }
        }

        if(m_RxTimout != 0)
        {
            QThread::msleep(1);
            if(m_RxTimout > PROTOCOL_TIMEOUT)
            {
               qDebug()<<QString("Receive timeout");
               return RT_TIMEOUT;
            }
        }
    }while(1);

    return RT_OK;
}
