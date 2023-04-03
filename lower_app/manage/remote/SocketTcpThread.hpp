//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SocketTcpThread.hpp
//
//  Purpose:
//      TCP协议执行模块，目前设计为同步模式，后续会修改
//
// Author:
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include <sys/socket.h>
#include "Protocol.hpp"

#define SOCKET_BUFFER_SIZE		1200

class CTcpProtocolInfo:public CProtocolInfo
{
public:
	using CProtocolInfo::CProtocolInfo;

	/*TCP Socket数据读取接口*/
	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		int ret;
		ret = recv(nFd, pDataStart, nDataSize, 0);
		return ret;
	}

	/*TCP Socket数据写入接口*/
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
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
	std::thread m_thread;
public:
	TcpThreadManage() = default;
	~TcpThreadManage() = delete;

	bool init();
	static TcpThreadManage* getInstance();
};

