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

#include "protocol.hpp"
#include "logger.hpp"

const uint8_t PACKED_HEAD[] = {0x5a, 0x5b};

protocol_info::protocol_info(const std::string &rx_fifo, const std::string &tx_fifo)
{
	rx_fifo_path = rx_fifo;
	tx_fifo_path_ = tx_fifo;

	memset(rx_buffer_, 0, RX_BUFFER_SIZE);
	rx_timeout_ = 0;
	rx_status_ = PROTOCOL_FRAME_EMPTY;
}
protocol_info::~protocol_info(void)
{
}

bool protocol_info::init()
{
	//rx fifo
	rx_fifo_ptr_ = new(std::nothrow) fifo_manage(rx_fifo_path, S_FIFO_WORK_MODE);
	if(rx_fifo_ptr_ == nullptr)
		return false;
	if(!rx_fifo_ptr_->create())
		return false;
	
	//tx fifo
	tx_fifo_ptr_ = new(std::nothrow) fifo_manage(tx_fifo_path_, S_FIFO_WORK_MODE);
	if(tx_fifo_ptr_ == nullptr)
		return false;
	if(!tx_fifo_ptr_->create())
		return false;

	return true;
}

int protocol_info::write_rx_fifo(char *ptr, uint16_t size)
{
	return rx_fifo_ptr_->write(ptr, size);
}

int protocol_info::read_rx_fifo(char *ptr, uint16_t size)
{
	return rx_fifo_ptr_->read(ptr, size);
}

ENUM_PROTOCOL_STATUS protocol_info::check_rx_frame(uint8_t data)
{
	uint16_t len;

	//未接收状态，检测第一个head字节
	if(rx_status_ == PROTOCOL_FRAME_EMPTY)
	{
		if(data == PACKED_HEAD[0])
		{
			rx_buffer_[0] = data;
			rx_buffer_size_ = 1;
			rx_status_ = PROTOCOL_FRAME_HEAD_RX;
			rx_timeout_ = xGetCurrentTicks();
		}
	}
	//接收到一个head字节，检测第二个字节
	else if(rx_status_ == PROTOCOL_FRAME_HEAD_RX)
	{
		//处理0x5a 0x5a 0x5b的case
		if(data == PACKED_HEAD[0])
		{
			//do nothing
		}
		else if(data == PACKED_HEAD[1])
		{
			rx_buffer_[rx_buffer_size_++] = data;
			rx_status_ = PROTOCOL_FRAME_DATA_RX;
		}
		else
			rx_status_ = PROTOCOL_FRAME_EMPTY;
	}
	//数据接收处理
	else
	{
		rx_buffer_[rx_buffer_size_++] = data;
		if(rx_buffer_size_ >= 6)
		{
			len = rx_buffer_[3];
			
			//接收长度符合协议
			if(rx_buffer_size_ == len+6)
			{
				uint16_t crc_calc, crc_value;
				crc_value = (rx_buffer_[rx_buffer_size_-1]<<8) | rx_buffer_[rx_buffer_size_-2];
				crc_calc = calculate_crc(&rx_buffer_[2], rx_buffer_size_-4);
				if(crc_value == crc_calc)
				{
					rx_status_ = ROTOCOL_FRAME_FINISHED;
				}
				else
				{
					rx_status_ = PROTOCOL_FRAME_EMPTY;
					PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Console Crc Error:%d, %d", crc_value, crc_calc);
				}
			}
		}
	}
	
	return rx_status_;
}     

void protocol_info::process_rx_frame()
{
	

}

void protocol_info::clear_rx_info()
{
	rx_status_ = PROTOCOL_FRAME_EMPTY;
	rx_buffer_size_ = 0;
}

int protocol_info::read_tx_fifo(char *buf, uint16_t size)
{
	return tx_fifo_ptr_->read(buf, size);
}

int protocol_info::write_tx_fifo(char *buf, uint16_t size)
{
	return tx_fifo_ptr_->write(buf, size);
}


uint16_t protocol_info::calculate_crc(uint8_t *pdata, uint16_t size)
{
	uint16_t nCrcOut;

	assert(pdata != nullptr);
	nCrcOut = crc16(DEFAULT_CRC_VALUE, pdata, size);
	return nCrcOut;
}