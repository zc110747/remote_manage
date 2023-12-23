//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      appthread.cpp
//
//  Purpose:
//     
//
// Author:
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      11/20/2023   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <QFile>
#include <QScopedArrayPointer>
#include "appthread.h"
#include "uartclient.h"
#include "tcpclient.h"
#include "commandinfo.h"


static CUdpSocketInfo *pCUdpSocketThreadInfo;
static CTcpSocketInfo *pCTcpSocketThreadInfo;
static CUartProtocolInfo *pCUartProtocolTreadInfo;

static CAppThreadInfo *pAppThreadInfo;
static SSendBuffer SendBufferInfo;

void FileUpdateProcess(void);
int InterfaceProcess(void);

/*!
    应用执行的主线程函数
*/
void CAppThreadInfo::run()
{
    int nStatus;

    pCUdpSocketThreadInfo = GetUdpClientSocketInfo();
    pCUdpSocketThreadInfo->UdpClientSocketInitForThread();
    pCUartProtocolTreadInfo = GetUartProtocolInfo();
    pCTcpSocketThreadInfo = GetTcpClientSocketInfo();
    pCTcpSocketThreadInfo->TcpClientSocketInitForThread();

    for (;;)
    {
        if (m_nIsStop)
            return;
        nStatus = m_pQueue->QueuePend(&SendBufferInfo);
        if (nStatus == QUEUE_INFO_OK)
        {
            if (SendBufferInfo.m_nCommand != SYSTEM_UPDATE_CMD)
            {
                SendBufferInfo.m_bUploadStatus = false;
                InterfaceProcess();
            }
            else
            {
                FileUpdateProcess();
            }
            qDebug()<<"Thread Queue Test Ok";
        }
    }
}

/*!
   用于单次通讯的线程执行
*/
int InterfaceProcess(void)
{
    if (SendBufferInfo.m_nProtocolStatus == PROTOCOL_UART)
    {
       return pCUartProtocolTreadInfo->UartLoopThread(&SendBufferInfo);
    }
    else if (SendBufferInfo.m_nProtocolStatus == PROTOCOL_TCP)
    {
       return pCTcpSocketThreadInfo->TcpClientSocketLoopThread(&SendBufferInfo);
    }
    else if (SendBufferInfo.m_nProtocolStatus == PROTOCOL_UDP)
    {
       return pCUdpSocketThreadInfo->UdpClientSocketLoopThread(&SendBufferInfo);
    }
    else
    {
        qDebug()<<"Invalid Protocol Type";
    }

    return RT_FAIL;
}

/*!
    生成发送的最初指令
*/
int CreateFileUpdateCmd(uint8_t *pDst, const char *pName, int nNameSize, int FileTotalSize)
{
    int nSize;
    int nFileBlock;

    nFileBlock = FileTotalSize/FILE_BLOCK_SIZE + (FileTotalSize%FILE_BLOCK_SIZE==0?0:1);

    nSize = 0;
    pDst[nSize++] = 0x03;
    pDst[nSize++] = (uint8_t)(FileTotalSize>>24);
    pDst[nSize++] = (uint8_t)(FileTotalSize>>16);
    pDst[nSize++] = (uint8_t)(FileTotalSize>>8);
    pDst[nSize++] = (uint8_t)(FileTotalSize>>0);
    pDst[nSize++] = (uint8_t)(nFileBlock>>8);
    pDst[nSize++] = (uint8_t)(nFileBlock>>0);
    memcpy((char *)&pDst[nSize], pName, nNameSize);
    nSize += nNameSize;
    pDst[nSize++] = 0; //用于字符串的结尾

    return nSize;
}

int CreateFileUpdateCmd(uint8_t *pDst, uint16_t nFileSize, uint16_t nFileBlock)
{
    int nSize;

    nSize = 0;
    pDst[nSize++] = 0x04;
    pDst[nSize++] = (uint8_t)(nFileSize>>8);
    pDst[nSize++] = (uint8_t)(nFileSize>>0);
    pDst[nSize++] = (uint8_t)(nFileBlock>>8);
    pDst[nSize++] = (uint8_t)(nFileBlock>>0);
    nSize += nFileSize;

    return nSize;
}

/*!
    用于文件传输的处理
*/
void FileUpdateProcess(void)
{
    static uint8_t ArrayBuffer[2000];
    uint16_t nReadSize;
    int nSize;
    int nFileBlock;

    //处理升级的整个流程实现
    QFile file(SendBufferInfo.m_qPathInfo);
    if (file.open(QIODevice::ReadOnly))
    {
        QStringList PathFileNameList = SendBufferInfo.m_qPathInfo.split("/");
        QString PathFileName = PathFileNameList[PathFileNameList.size()-1];
        QFile outfile(QString("D:/")+PathFileName);

        nSize = CreateFileUpdateCmd(ArrayBuffer, PathFileName.toLatin1().data(), PathFileName.size(), file.size());
        SendBufferInfo.m_pBuffer = ArrayBuffer;
        SendBufferInfo.m_nSize = nSize;
        InterfaceProcess();
        nFileBlock = 0;

        while ((nReadSize = file.read((char *)&ArrayBuffer[5], FILE_BLOCK_SIZE)) > 0)
        {
            nFileBlock++;
            nSize = CreateFileUpdateCmd(ArrayBuffer, nReadSize, nFileBlock);
            #if TEST_DEBUG == 1
            qDebug()<<"AppThread.cpp:Send Size"<<nSize;
            #endif
            SendBufferInfo.m_nSize = nSize;
            SendBufferInfo.m_bUploadStatus = true;
            InterfaceProcess();
        }
        file.close();
    }
    else
    {
        qDebug()<<"AppThread.cpp:File Open Filed";
    }

    SendBufferInfo.m_bUploadStatus = false;
    qDebug()<<"AppThread.cpp:File Update Finished";
}

/*!
    主应用线程初始化
*/
void AppThreadInit(void)
{
    pAppThreadInfo = new CAppThreadInfo();
}

/*!
    获取主应用线程的信息数据结构
*/
CAppThreadInfo *GetAppThreadInfo()
{
    return pAppThreadInfo;
}
