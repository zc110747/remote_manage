/*
 * File      : SocketUdpThread.h
 * This file is socket-udp interface
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#ifndef _INCLUDE_SOCKET_UDP_THREAD_H
#define _INCLUDE_SOCKET_UDP_THREAD_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "productConfig.hpp"
#include "ApplicationThread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "UsrProtocol.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define UDP_BUFFER_SIZE		1200

/**************************************************************************
* Global Type Definition
***************************************************************************/
struct UdpInfo
{
    struct sockaddr_in clientaddr;
    socklen_t client_sock_len;   
};

template<class T>
class CUdpProtocolInfo:public CProtocolInfo<T>
{
public:
	using CProtocolInfo<T>::CProtocolInfo;

	/*UDP Socket数据读取接口*/
	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T extra_info)
	{
		int nLen;
		struct UdpInfo *pUdpInfo = (struct UdpInfo *)extra_info;
		nLen = recvfrom(nFd, pDataStart, nDataSize, 0, (struct sockaddr *)&(pUdpInfo->clientaddr), 
					&(pUdpInfo->client_sock_len));

		return nLen;
	}

	/*UDP Socket数据写入接口*/
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T extra_info)
	{
		struct UdpInfo *pUdpInfo = (struct UdpInfo *)extra_info;
		return sendto(nFd, pDataStart, nDataSize, 0, (struct sockaddr *)&(pUdpInfo->clientaddr), 
					pUdpInfo->client_sock_len);
	}
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/

#if SOCKET_UDP_MODULE_ON == 1
/*UDP Socket线程初始化实现*/
void SocketUdpThreadInit(void);
#else
#define SocketUdpThreadInit() 
#endif
#endif
