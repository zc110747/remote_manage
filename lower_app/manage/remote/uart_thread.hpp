//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      uart_thread.hpp
//
//  Purpose:
//      Uart通讯执行模块，目前设计为同步模式，后续会修改
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

#define UART_MAX_BUFFER_SIZE     		512

class uart_thread_manage
{
private:
	//device info
	int nComFd{-1};
	protocol_info *pProtocolInfo;

	//message buffer
	uint8_t nRxCacheBuffer[UART_MAX_BUFFER_SIZE];
	uint8_t nTxCacheBuffer[UART_MAX_BUFFER_SIZE];

	static uart_thread_manage* pInstance;

	void run();

public:
	uart_thread_manage() = default;
	~uart_thread_manage() = delete;
	
	bool init();
	int set_opt(int, int, std::string, int);
	void release();

	static uart_thread_manage* get_instance();
	int getComfd()	{return nComFd;}
};

