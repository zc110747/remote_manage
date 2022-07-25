/*
 * File      : uart_task.h
 * uart task interface
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 * 2020-5-20     zc           Code standardization 
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#ifndef _INCLUDE_UART_TASK_H
#define _INCLUDE_UART_TASK_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "UsrProtocol.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define UART_BUFFER_SIZE     		1200

/**************************************************************************
* Global Type Definition
***************************************************************************/
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

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/

/*Uart通讯相关的线程初始化*/
#if UART_MODULE_ON == 1
void UartThreadInit(void);
#else
#define UartThreadInit()
#endif

#endif
