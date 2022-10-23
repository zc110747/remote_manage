//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SocketTcpThread.hpp
//
//  Purpose:
//      Socket Tcp Thread process interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/31/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_SOCKET_THREAD_H
#define _INCLUDE_SOCKET_THREAD_H

#include <sys/socket.h>
#include "UsrProtocol.hpp"

#define SOCKET_BUFFER_SIZE		1200

class CTcpProtocolInfo:public CProtocolInfo<void>
{
public:
	using CProtocolInfo<void>::CProtocolInfo;

	/*TCP Socket数据读取接口*/
	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, void* output = nullptr)
	{
		int ret;
		ret = recv(nFd, pDataStart, nDataSize, 0);
		return ret;
	}

	/*TCP Socket数据写入接口*/
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, void* input = nullptr)
	{
		int ret;
		ret = send(nFd, pDataStart, nDataSize, 0);
		return ret;
	}
};

class TcpThreadManage
{
private:
	pthread_t	tid;
	static TcpThreadManage* pInstance;
	std::thread *pthread{nullptr};
public:
	TcpThreadManage() = default;
	~TcpThreadManage() = delete;

	bool init();
	static TcpThreadManage* getInstance();
	std::thread *get_thread() { return pthread; }
};
#endif
