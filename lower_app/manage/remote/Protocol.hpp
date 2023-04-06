//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      protocol.hpp
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
#include "FIFOManage.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define RX_BUFFER_SIZE			1024
#define TX_BUFFER_SIZE			1024
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

class protocol_info
{
public:  
	/// Construct and create rx/tx fifo for protocol process
	/**
	 * @param rx_fifo a fifo path for protocol rx process
	 *
	 * @param tx_fifo a fifo path for protocol tx process
	 */
	protocol_info(const std::string &rx_fifo, const std::string &tx_fifo);
	~protocol_info(void);

	bool init();

	//protocol rx process
	int write_rx_fifo(char*, uint16_t);
	int read_rx_fifo(char*, uint16_t);
	ENUM_PROTOCOL_STATUS check_rx_frame(uint8_t);    
	void process_rx_frame();	
	void clear_rx_info();

	//protocol tx process
	int read_tx_fifo(char *, uint16_t);
	int write_tx_fifo(char *, uint16_t);

	//calculate crc16
	uint16_t calculate_crc(uint8_t*, uint16_t);

private:
	//protocol rx buffer process
	uint8_t  rx_buffer_[RX_BUFFER_SIZE];   
	uint16_t rx_buffer_size_{0};	   			
	uint32_t rx_timeout_; 					
	ENUM_PROTOCOL_STATUS rx_status_;
	
	FIFOManage *rx_fifo_ptr_;
	std::string rx_fifo_path;
	
	FIFOManage *tx_fifo_ptr_;
	std::string tx_fifo_path_;
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
