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
#include "UsrTypeDef.h"
#include "UsrProtocol.h"
#include "ApplicationThread.h"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define UART_BUFFER_SIZE     		1200

/**************************************************************************
* Global Type Definition
***************************************************************************/
class CUartProtocolInfo:public CProtocolInfo
{
public:
	CUartProtocolInfo(uint8_t *pRxCachebuf, uint8_t *pTxCacheBuf, uint8_t *pRxData, uint16_t nMaxSize):
		CProtocolInfo(pRxCachebuf, pTxCacheBuf, pRxData, nMaxSize){}
	~CUartProtocolInfo(){}

	int DeviceRead(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		return read(nFd, pDataStart, nDataSize);
	}
	int DeviceWrite(int nFd, uint8_t *pDataStart, uint16_t nDataSize)
	{
		return write(nFd, pDataStart, nDataSize);
	}
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
void UartThreadInit(void);
#endif
