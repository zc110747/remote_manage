/*
 * File      : SocketThread.h
 * Socket Comm
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4       zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#ifndef _INCLUDE_SOCKET_THREAD_H
#define _INCLUDE_SOCKET_THREAD_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "UsrTypeDef.h"
#include "UsrProtocol.h"
#include "ApplicationThread.h"
#include <sys/socket.h>

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define SOCKET_BUFFER_SIZE		1200

/**************************************************************************
* Global Type Definition
***************************************************************************/
class CSocketProtocolInfo:public CProtocolInfo
{
public:
	CSocketProtocolInfo(uint8_t *pRxCachebuf, uint8_t *pTxCacheBuf, uint8_t *pRxData, uint16_t nMaxSize):
		CProtocolInfo(pRxCachebuf, pTxCacheBuf, pRxData, nMaxSize){}
	~CSocketProtocolInfo(){}

	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		return recv(nFd, pDataStart, nDataSize, 0);
	}
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		return send(nFd, pDataStart, nDataSize, 0);
	}
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
void SocketThreadInit(void);
#endif
