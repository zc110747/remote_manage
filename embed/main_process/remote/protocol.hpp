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
//        数据格式
//        数据头(2byte): 0x5A 0x5B                             
//        序列号(2byte): 0~65536
//      功能域(1Byte): bit7 ack, all 0 means data
//        长度  (1Byte): 0~255
//        数据: DATA
//      CRC:2Byte -- 计算从序列号起始到数据末尾
//
// Author:
//         @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common.hpp"
#include "fifo_manage.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define RX_BUFFER_SIZE            1024
#define TX_BUFFER_SIZE            1024
#define DEFAULT_CRC_VALUE        0xFFFF

#define PROTOCOL_HEAD_SIZE        2
#define PROTOCOL_SEQU_SIZE        2
#define PROTOCOL_FUNCTION_SIZE    1
#define PROTOCOL_LENGTH_SIZE    1
#define PROTOCOL_CRC_SIZE        2

//上述长度的累加
#define PROTOCOL_FRAME_LENGHT    8
#define MAX_TX_BUFFER_LENGTH    (255+PROTOCOL_FRAME_LENGHT+1)

//function bit
#define FUNCTION_ACK            1<<7

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
    /// \brief constructor
    protocol_info();

    /// \brief destructor
    ~protocol_info(){}

    /// \brief init
    /// - This method is used to init the object.
    /// \param rx_fifo - fifo path for protocol receive
    /// \param tx_fifo - fifo path for protocol transform
    /// \param lambda - function for protocol transform internal
    /// \return Wheather initialization is success or failed.
    bool init(const std::string &rx_fifo, 
            const std::string &tx_fifo, 
            std::function<void(char* ptr, int size)> lambda);
    
    /// \brief write_rx_fifo
    /// - write buffer to rx fifo.
    /// \param ptr - point for data write to fifo
    /// \param size - size of the data write to fifo
    /// \return the size of the data already write.
    int write_rx_fifo(char *ptr, uint16_t size);

    /// \brief read_rx_fifo
    /// - read buffer from rx fifo.
    /// \param ptr - point to the start of data read from rx fifo
    /// \param size - max size of the data can be read.
    /// \return the size of the data already read.
    int read_rx_fifo(char *ptr, uint16_t size);

    /// \brief write_tx_fifo
    /// - write buffer to tx fifo.
    /// \param buf - point for data write to fifo
    /// \param size - size of the data write to fifo
    /// \return the size of the data already write.
    int write_tx_fifo(char *buf, int size);

    /// \brief read_tx_fifo
    /// - read buffer from tx fifo.
    /// \param ptr - point to the start of data read from tx fifo.
    /// \param size - max size of the data can be read.
    /// \return the size of the data already read.
    int read_tx_fifo(char *buf, int size);

    /// \brief check_rx_frame
    /// - check the receive data by byte, and verify protocol.
    /// \param data - one byte receive data to check protocol.
    /// \return protocol process status for receive.
    ENUM_PROTOCOL_STATUS check_rx_frame(uint8_t data);    

    /// \brief process_rx_frame
    /// - process the whole data by protocol.
    void process_rx_frame();    

    /// \brief clear_rx_info
    /// - clear the protocol receive infomation for next receive.
    void clear_rx_info();

    /// \brief send_data
    /// - send the data to remote, without func.
    /// \param pIntput - pointer to the start of send data.
    /// \param len - size of the data need send.
    /// \return size of data already send to remote.
    int send_data(char *pIntput, uint16_t len);
    
    /// \brief send_func_data
    /// - send the data with functional to remote.
    /// \param sequence - sequence value will back to remote.
    /// \param func - functional of the data
    void send_func_data(uint16_t sequence, uint8_t func);

private:
    /// \brief create_output_frame
    /// - create output buffer by protocol.
    /// \param pOut - point to outer buffer.
    /// \param pIn - point to input buffer.
    /// \param len - size of the input buffer.
    /// \return the size of the outer buffer.
    uint16_t create_output_frame(char *pOut, char *pIn, uint16_t len);

    /// \brief calculate_crc
    /// - calculate the crc16 for the data input.
    /// \param pdata - point to data for calculate crc16.
    /// \param size - size of the data for calculate crc16.
    /// \return the crc16 value of the data.
    uint16_t calculate_crc(uint8_t *pdata, uint16_t size);

private:
    /// \brief rx_buffer_
    /// - buffer used to save the receive protocol data.
    uint8_t  rx_buffer_[RX_BUFFER_SIZE];   

    /// \brief rx_buffer_size_
    /// - size of the buffer to save the receive protocol data.
    uint16_t rx_buffer_size_{0};    

    /// \brief rx_timeout_
    /// - timeout used to check the receive continuous.
    uint32_t rx_timeout_;

    /// \brief rx_status_
    /// - current rx protocol check status.
    ENUM_PROTOCOL_STATUS rx_status_;
    
    /// \brief sequence_num
    /// - sequence for the send num, will add one every send.
    uint16_t sequence_num{0};

    /// \brief rx_fifo_ptr_
    /// - pointer for protocol receive fifo manage.
    std::unique_ptr<fifo_manage> rx_fifo_ptr_;

    /// \brief tx_fifo_ptr_
    /// - pointer for protocol transform fifo manage.
    std::unique_ptr<fifo_manage> tx_fifo_ptr_;

    /// \brief handler_
    /// - function used to process send data in protocol.
    std::function<void(char* ptr, int size)> handler_;
};