//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      UartThread.hpp
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

#include "Protocol.hpp"

#define UART_MAX_BUFFER_SIZE     		512

class CUartProtocolInfo:public CProtocolInfo
{
public:
	using CProtocolInfo::CProtocolInfo;

	/*串口的通讯读接口*/
	int DeviceRead(int fd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		int ret;
		ret= read(fd, pDataStart, nDataSize);
		return ret;
	}

	/*串口的通讯写接口*/
	int DeviceWrite(int fd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		int ret;
		ret = write(fd, pDataStart, nDataSize);
		return ret;
	}
};

class UartThreadManage
{
private:
	//device info
	int nComFd{-1};
	CUartProtocolInfo *pProtocolInfo;

	//message buffer
	uint8_t nRxCacheBuffer[UART_MAX_BUFFER_SIZE];
	uint8_t nTxCacheBuffer[UART_MAX_BUFFER_SIZE];

	static UartThreadManage* pInstance;

	void run();

public:
	UartThreadManage() = default;
	~UartThreadManage() = delete;
	
	bool init();
	int set_opt(int, int, std::string, int);
	void release();

	static UartThreadManage* getInstance();
	int getComfd()	{return nComFd;}
};

