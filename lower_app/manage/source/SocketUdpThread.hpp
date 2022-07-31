//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SocketTcpThread.cpp
//
//  Purpose:
//      Socket Tcp Thread process workflow.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/31/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_SOCKET_UDP_THREAD_H
#define _INCLUDE_SOCKET_UDP_THREAD_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "UsrProtocol.hpp"

#define UDP_BUFFER_SIZE		1200

typedef struct 
{
    struct sockaddr_in clientaddr;
    socklen_t client_sock_len;   
}UDP_CLIENT;

class CUdpProtocolInfo:public CProtocolInfo<UDP_CLIENT>
{
public:
	using CProtocolInfo<UDP_CLIENT>::CProtocolInfo;

	/*UDP Socket数据读取接口*/
	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, UDP_CLIENT* out) override
	{
		int nLen;
		nLen = recvfrom(nFd, pDataStart, nDataSize, 0, (struct sockaddr *)&(out->clientaddr), 
					&(out->client_sock_len));

		return nLen;
	}

	/*UDP Socket数据写入接口*/
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, UDP_CLIENT* input) override
	{
		return sendto(nFd, pDataStart, nDataSize, 0, (struct sockaddr *)&(input->clientaddr), 
					input->client_sock_len);
	}
};

class UdpThreadManage
{
private:
	pthread_t tid;

	UDP_CLIENT client;
	CUdpProtocolInfo *pProtocolInfo;
	static UdpThreadManage *pInstance;

	uint8_t RxCacheBuffer[UDP_BUFFER_SIZE];
	uint8_t  TxCacheBuffer[UDP_BUFFER_SIZE];

public:
	UdpThreadManage();
		~UdpThreadManage();
	
	static UdpThreadManage *getInstance();
	bool init();
	UDP_CLIENT *getClient()	{return &client;}
	CUdpProtocolInfo *getProtocolInfo() {return pProtocolInfo;}
};

#endif
