//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      Protocol.hpp
//
//  Purpose:
//      用于处理协议定义的接口
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

#include "modules.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define RX_BUFFER_SIZE			1024
#define DEFAULT_CRC_VALUE		0xFFFF

#define BIG_ENDING         		0
#if BIG_ENDING	
#define LENGTH_CONVERT(val)	(val)
#else
#define LENGTH_CONVERT(val) (((val)<<8) |((val)>>8))
#endif

/**************************************************************************
* Global Type Definition
***************************************************************************/
typedef enum
{
	PROTOCOL_FRAME_EMPTY = 0,
	PROTOCOL_FRAME_HEAD_RX,
	PROTOCOL_FRAME_DATA_RX,
	ROTOCOL_FRAME_FINISHED,
}ENUM_PROTOCOL_STATUS;

class CProtocolInfo
{
private:
	uint8_t  m_RxBuffer[RX_BUFFER_SIZE];    //接收数据缓存
	uint16_t m_RxBufSize{0};	   			//接收数据长度
	uint32_t m_RxTimeout; 					//超时时间
	ENUM_PROTOCOL_STATUS rxStatus;
	
	int do_fd{-1};							//处理硬件的fd接口

public:  
	CProtocolInfo(int fd)
	{
		memset(m_RxBuffer, 0, RX_BUFFER_SIZE);
		m_RxTimeout = 0;
		rxStatus = PROTOCOL_FRAME_EMPTY;
		
		do_fd = fd;
	};
	~CProtocolInfo(void){}

	bool CheckRxFrame();     
	void ProcessRxFrame();	
	uint16_t CrcCalculate(uint8_t *pdata, uint16_t size);

	virtual int DeviceRead(int fd, uint8_t *data, uint16_t size)=0;  
	virtual int DeviceWrite(int fd, uint8_t *pDataStart, uint16_t nDataSize)=0;
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
