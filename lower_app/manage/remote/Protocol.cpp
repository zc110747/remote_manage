//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      Protocol.cpp
//
//  Purpose:
//      协议接收和处理接口
//		数据格式
//		数据头: 0x5A 0x5B
//		序列号: 0~255
//		功能位: bit7:数据包/同步包 
//
// Author:
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      4/3/2023   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include "Protocol.hpp"
#include "logger.hpp"

const uint8_t PACKED_HEAD[] = {0x5a, 0x5b};

uint16_t CProtocolInfo::CrcCalculate(uint8_t *pdata, uint16_t size)
{
	uint16_t nCrcOut;

	assert(pdata != nullptr);
	nCrcOut = crc16(DEFAULT_CRC_VALUE, pdata, size);
	return nCrcOut;
}

bool CProtocolInfo::CheckRxFrame()
{
	uint8_t data;

	if(DeviceRead(do_fd, &data, 1) > 0)
	{
		uint16_t len;

		//未接收状态，检测第一个head字节
		if(rxStatus == PROTOCOL_FRAME_EMPTY)
		{
			if(data == PACKED_HEAD[0])
			{
				m_RxBuffer[0] = data;
				m_RxBufSize = 1;
				rxStatus = PROTOCOL_FRAME_HEAD_RX;
				m_RxTimeout = xGetCurrentTicks();
			}
		}
		//接收到一个head字节，检测第二个字节
		else if(rxStatus == PROTOCOL_FRAME_HEAD_RX)
		{
			//处理0x5a 0x5a 0x5b的case
			if(data == PACKED_HEAD[0])
			{
				//do nothing
			}
			else if(data == PACKED_HEAD[1])
			{
				m_RxBuffer[m_RxBufSize++] = data;
				rxStatus = PROTOCOL_FRAME_DATA_RX;
			}
			else
				rxStatus = PROTOCOL_FRAME_EMPTY;
		}
		//数据接收处理
		else
		{
			m_RxBuffer[m_RxBufSize++] = data;
			if(m_RxBufSize >= 6)
			{
				len = m_RxBuffer[3];
				
				//接收长度符合协议
				if(m_RxBufSize == len+6)
				{
					uint16_t crc_calc, crc_value;
					crc_value = (m_RxBuffer[m_RxBufSize-1]<<8) | m_RxBuffer[m_RxBufSize-2];
					crc_calc = CrcCalculate(&m_RxBuffer[2], m_RxBufSize-4);
					if(crc_value == crc_calc)
					{
						rxStatus = ROTOCOL_FRAME_FINISHED;
					}
					else
					{
						rxStatus = PROTOCOL_FRAME_EMPTY;
						PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Console Crc Error:%d, %d", crc_value, crc_calc);
					}
				}
			}
		}
		return true;
	}
	return false;
}     

void CProtocolInfo::ProcessRxFrame()
{
	if(rxStatus == ROTOCOL_FRAME_FINISHED)
	{
		rxStatus = PROTOCOL_FRAME_EMPTY;

		//do command process
		
	}
	else if(rxStatus == PROTOCOL_FRAME_EMPTY)
	{
		m_RxBufSize = 0;
	}
	else
	{
		//just next receive, do nothing
	}
}