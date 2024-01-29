//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      Protocol.cpp
//
//  Purpose:
//
// Author:
//         @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      4/3/2023   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include "protocol.hpp"
#include "common_unit.hpp"

const uint8_t PACKED_HEAD[] = {0x5a, 0x5b};

protocol_info::protocol_info()
{
    memset(rx_buffer_, 0, RX_BUFFER_SIZE);
    rx_timeout_ = 0;
    rx_status_ = PROTOCOL_FRAME_EMPTY;
}

bool protocol_info::init(
    const std::string &rx_fifo, 
    const std::string &tx_fifo, 
    std::function<void(char* ptr, int size)> lambda
)
{
    //rx fifo
    rx_fifo_ptr_ = std::make_unique<fifo_manage>(rx_fifo, S_FIFO_WORK_MODE);
    if (!rx_fifo_ptr_->create())
        return false;
    
    //tx fifo
    tx_fifo_ptr_ = std::make_unique<fifo_manage>(tx_fifo, S_FIFO_WORK_MODE);
    if (!tx_fifo_ptr_->create())
        return false;

    handler_ = lambda;
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
    if (rx_status_ == PROTOCOL_FRAME_EMPTY)
    {
        if (data == PACKED_HEAD[0])
        {
            rx_buffer_[0] = data;
            rx_buffer_size_ = 1;
            rx_status_ = PROTOCOL_FRAME_HEAD_RX;
            rx_timeout_ = xGetCurrentTimes();
        }
    }
    //接收到一个head字节，检测第二个字节
    else if (rx_status_ == PROTOCOL_FRAME_HEAD_RX)
    {
        //处理0x5a 0x5a 0x5b的case
        if (data == PACKED_HEAD[0])
        {
            //do nothing
        }
        else if (data == PACKED_HEAD[1])
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
        if (rx_buffer_size_ >= 5)
        {
            len = rx_buffer_[5];
            
            //接收长度符合协议
            if (rx_buffer_size_ == len+PROTOCOL_FRAME_LENGHT)
            {
                uint16_t crc_calc, crc_value;
                crc_value = (((uint16_t)rx_buffer_[rx_buffer_size_-2])<<8) | rx_buffer_[rx_buffer_size_-1];
                crc_calc = calculate_crc(&rx_buffer_[2], rx_buffer_size_-4);
                if (crc_value == crc_calc)
                {
                    rx_status_ = ROTOCOL_FRAME_FINISHED;
                }
                else
                {
                    rx_status_ = PROTOCOL_FRAME_EMPTY;
                    PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Console Crc Error:0x%x, 0x%x", crc_value, crc_calc);
                }
            }
        }
    }
    
    return rx_status_;
}     

void protocol_info::process_rx_frame()
{
    uint8_t rx_func = rx_buffer_[4];
    uint16_t sequence_num = (((uint16_t)rx_buffer_[2])<<8) | rx_buffer_[3];
    
    //empty data
    if (rx_buffer_size_ == PROTOCOL_FRAME_LENGHT)
    {
        if (rx_func == FUNCTION_ACK)
        {
            //for support retry
        }
        else if (rx_func == 0) //empty headbeats packetage
        {
            //recall ack
            send_func_data(sequence_num, FUNCTION_ACK);
        }
    }
    else if (rx_func == 0)
    {
        //recall ack
        uint16_t event_id = 0;
        send_func_data(sequence_num, FUNCTION_ACK);

        if (rx_buffer_[5] >= 2)
        {
            event_id = (((uint16_t)rx_buffer_[6])<<8) | rx_buffer_[7];
            //center_manage::get_instance()->send_message(event_id, &rx_buffer_[8], rx_buffer_[5]-2);
        }
        else
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "protocol rx length is too small!");
        }
    }
}

void protocol_info::clear_rx_info()
{
    rx_status_ = PROTOCOL_FRAME_EMPTY;
    rx_buffer_size_ = 0;
}

int protocol_info::read_tx_fifo(char *buf, int size)
{
    return tx_fifo_ptr_->read(buf, size);
}

int protocol_info::write_tx_fifo(char *buf, int size)
{
    return tx_fifo_ptr_->write(buf, size);
}

int protocol_info::send_data(char *pIntput, uint16_t len)
{
    char tx_buffer[MAX_TX_BUFFER_LENGTH];
    uint16_t tx_size;

    tx_size = create_output_frame(tx_buffer, pIntput, len);
    if (tx_size > 0)
    {
        write_tx_fifo(tx_buffer, tx_size);
    }
    
    return tx_size;
}

void protocol_info::send_func_data(uint16_t sequence, uint8_t func)
{
    char buffer[64];
    uint8_t size = 0;
    uint16_t crc_check;

    buffer[size++] = PACKED_HEAD[0];
    buffer[size++] = PACKED_HEAD[1];
    buffer[size++] = sequence>>8;
    buffer[size++] = sequence&0xff;
    buffer[size++] = func;  //Data数据， function功能为0
    buffer[size++] = 0;

    crc_check = calculate_crc((uint8_t *)&buffer[2], size-2);
    buffer[size++] = crc_check>>8;
    buffer[size++] = crc_check&0xff;
    
    //send data
    handler_(buffer, size);
}

uint16_t protocol_info::create_output_frame(char *pOutput, char *pInput, uint16_t len)
{
    uint16_t size = 0;
    uint16_t crc_check;

    sequence_num++;

    pOutput[size++] = PACKED_HEAD[0];
    pOutput[size++] = PACKED_HEAD[1];
    pOutput[size++] = sequence_num>>8;
    pOutput[size++] = sequence_num&0xff;
    pOutput[size++] = 0;  //Data数据， function功能为0
    pOutput[size++] = len;

    memcpy(&pOutput[size], pInput, len);
    size += len;

    crc_check = calculate_crc((uint8_t *)&pOutput[2], size-2);
    pOutput[size++] = crc_check>>8;
    pOutput[size++] = crc_check&0xff;
    return size;
}

uint16_t protocol_info::calculate_crc(uint8_t *pdata, uint16_t size)
{
    uint16_t nCrcOut;

    assert(pdata != nullptr);
    nCrcOut = crc16(DEFAULT_CRC_VALUE, pdata, size);
    return nCrcOut;
}