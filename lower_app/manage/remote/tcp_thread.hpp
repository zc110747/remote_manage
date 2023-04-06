//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      tcp_thread.hpp
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

class tcp_thread_manage
{
public:
	tcp_thread_manage() = default;
	~tcp_thread_manage() = delete;

	bool init();
	void tcp_server_run();
	void tcp_rx_run();
	void tcp_tx_run();
	static tcp_thread_manage* getInstance();

private:
	static tcp_thread_manage* pInstance;
	std::thread m_server_thread;
	std::thread m_rx_thread;
	std::thread m_tx_thread;
	protocol_info* protocol_info_ptr_;
};

