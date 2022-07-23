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
#include "productConfig.hpp"
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

	/*TCP Socket数据读取接口*/
	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)
	{
		*ExtraInfo = recv(nFd, pDataStart, nDataSize, 0);
		return *ExtraInfo;
	}

	/*TCP Socket数据写入接口*/
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

#if SOCKET_TCP_MODULE_ON == 1
/*TCP网络通讯任务和数据初始化*/
void SocketTcpThreadInit(void);
#else
#define SocketTcpThreadInit() {}
#endif

#endif
