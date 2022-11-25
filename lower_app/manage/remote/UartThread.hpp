//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      UartThread.hpp
//
//  Purpose:
//      UartThread module interface.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_UART_THREAD_H
#define _INCLUDE_UART_THREAD_H

#include "Protocol.hpp"

#define UART_MAX_BUFFER_SIZE     		512

class CUartProtocolInfo:public CProtocolInfo<void>
{
public:
	using CProtocolInfo<void>::CProtocolInfo;

	/*串口的通讯读接口*/
	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, void *output = nullptr)
	{
		int ret;
		ret= read(nFd, pDataStart, nDataSize);
		return ret;
	}

	/*串口的通讯写接口*/
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, void *input = nullptr)
	{
		int ret;
		ret = write(nFd, pDataStart, nDataSize);
		return ret;
	}
};

class UartThreadManage
{
private:
	//device info
	int nComFd{-1};
	std::thread m_thread;
	CUartProtocolInfo *pProtocolInfo;

	//message buffer
	uint8_t nRxCacheBuffer[UART_MAX_BUFFER_SIZE];
	uint8_t nTxCacheBuffer[UART_MAX_BUFFER_SIZE];

	static UartThreadManage* pInstance;

public:
	UartThreadManage() = default;
	~UartThreadManage() = delete;
	
	bool init();
	int set_opt(int, int, std::string, int);
	void release();

	static UartThreadManage* getInstance();
	CUartProtocolInfo* getProtocolInfo() {return pProtocolInfo;}
	int getComfd()	{return nComFd;}
};
#endif
