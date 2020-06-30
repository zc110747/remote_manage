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
#include "ApplicationThread.h"
#include <sys/socket.h>
#include "UsrProtocol.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define SOCKET_BUFFER_SIZE		1200

/**************************************************************************
* Global Type Definition
***************************************************************************/
template<class T>
class CTcpProtocolInfo:public CProtocolInfo<T>
{
public:
	using CProtocolInfo<T>::CProtocolInfo;

	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)
	{
		*ExtraInfo = recv(nFd, pDataStart, nDataSize, 0);
		return *ExtraInfo;
	}
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)
	{
		*ExtraInfo = send(nFd, pDataStart, nDataSize, 0);
		return *ExtraInfo;
	}
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
void SocketTcpThreadInit(void);
#endif
