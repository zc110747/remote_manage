//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      protocol.hpp
//
//  Purpose:
//      用于处理协议定义的接口
//      协议接收和处理接口
//		数据格式
//		数据头(2byte): 0x5A 0x5B                             
//		序列号(2byte): 0~65536
//      功能域(1Byte): bit7 ack, all 0 means data
//		长度  (1Byte): 0~255
//		数据:	DATA
//      CRC:	2Byte -- 计算从序列号起始到数据末尾
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
#include "fifo_manage.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define RX_BUFFER_SIZE			1024
#define TX_BUFFER_SIZE			1024
#define DEFAULT_CRC_VALUE		0xFFFF

#define PROTOCOL_HEAD_SIZE		2
#define PROTOCOL_SEQU_SIZE		2
#define PROTOCOL_FUNCTION_SIZE	1
#define PROTOCOL_LENGTH_SIZE	1
#define PROTOCOL_CRC_SIZE		2

//上述长度的累加
#define PROTOCOL_FRAME_LENGHT	8
#define MAX_TX_BUFFER_LENGTH	(255+PROTOCOL_FRAME_LENGHT+1)

//function bit
#define FUNCTION_ACK			1<<7


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
	protocol_info(const std::string &rx_fifo, const std::string &tx_fifo, std::function<void(char* ptr, int size)> lambda);
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
	uint16_t create_output_frame(char *pOut, char *pIn, uint16_t len);
	int send_data(char *pIntput, uint16_t len);
	void send_func_data(uint16_t sequence, uint8_t func);

	//calculate crc16
	uint16_t calculate_crc(uint8_t*, uint16_t);

private:
	//protocol rx buffer process
	uint8_t  rx_buffer_[RX_BUFFER_SIZE];   
	uint16_t rx_buffer_size_{0};	   			
	uint32_t rx_timeout_; 					
	ENUM_PROTOCOL_STATUS rx_status_;
	
	fifo_manage *rx_fifo_ptr_{nullptr};
	std::string rx_fifo_path;
	
	fifo_manage *tx_fifo_ptr_{nullptr};
	std::string tx_fifo_path_;
	std::function<void(char* ptr, int size)> handler_;
	
	uint16_t sequence_num{0};
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
