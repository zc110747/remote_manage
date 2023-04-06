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

#include "protocol.hpp"

#define SOCKET_BUFFER_SIZE		1200

class TcpThreadManage
{
private:
	pthread_t	tid;
	static TcpThreadManage* pInstance;
	std::thread m_server_thread;
	std::thread m_rx_thread;
	std::thread m_tx_thread;

public:
	TcpThreadManage() = default;
	~TcpThreadManage() = delete;

	bool init();
	void tcp_server_run();
	void tcp_rx_run();
	void tcp_tx_run();
	static TcpThreadManage* getInstance();

public:
	protocol_info* protocol_info_ptr_;
};

