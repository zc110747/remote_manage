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
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_UART_THREAD_H
#define _INCLUDE_UART_THREAD_H

#include "UsrProtocol.hpp"

#define UART_MAX_BUFFER_SIZE     		512

template<class T>
class CUartProtocolInfo:public CProtocolInfo<T>
{
public:
	using CProtocolInfo<T>::CProtocolInfo;

	/*串口的通讯读接口*/
	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)
	{
		*ExtraInfo = read(nFd, pDataStart, nDataSize);
		return *ExtraInfo;
	}

	/*串口的通讯写接口*/
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize, T ExtraInfo)
	{
		*ExtraInfo = write(nFd, pDataStart, nDataSize);
		return *ExtraInfo;
	}
};

class UartThreadManage
{
private:
	//device info
	int nComFd{-1};
	pthread_t tid;
	CUartProtocolInfo<int *> *pProtocolInfo;

	//message buffer
	uint8_t nRxCacheBuffer[UART_MAX_BUFFER_SIZE];
	uint8_t nTxCacheBuffer[UART_MAX_BUFFER_SIZE];

	static UartThreadManage* pInstance;

public:
	UartThreadManage() = default;
	~UartThreadManage(){}
	
	bool init();
	int set_opt(int, int, std::string, int);
	void release();

	static UartThreadManage* getInstance();
	CUartProtocolInfo<int *>* getProtocolInfo() {return pProtocolInfo;}
	int getComfd()	{return nComFd;}
};
#endif
