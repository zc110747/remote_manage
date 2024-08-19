//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      UartThread.cpp
//
//  Purpose:
//      Uart通讯执行模块，目前设计为同步模式，后续会修改
//
// Author:
//         @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "serial.hpp"
#include <sys/termios.h>
#include "asio_client.hpp"
#include "common_unit.hpp"


serial_manage* serial_manage::instance_pointer_ = nullptr;
serial_manage* serial_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) serial_manage();
        if (instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "serial_manage new failed!");
        }
    }
    return instance_pointer_;
}

void serial_manage::uart_server_run()
{
    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "%s start", __func__);

    for (;;)
    {
        int size = ::read(com_fd_, rx_buffer_, SERIAL_RX_MAX_BUFFER_SIZE);
        if (size > 0)
        {
            //put int asio client to send
            asio_client::get_instance()->send_msg(rx_buffer_, size);
        }
        else
        {
            //do nothing
        }
    }
}

void serial_manage::uart_tx_run()
{
    int size;

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "%s start", __func__);
    while (1)
    {
        size = serial_tx_fifo_->read(tx_buffer_, SERIAL_RX_MAX_BUFFER_SIZE);
        if (size > 0)
        {
            write_data(tx_buffer_, size);
        }
        else
        {
            //do nothing
        }
    }
}

int serial_manage::send_msg(char *buffer, uint16_t size)
{
    return serial_tx_fifo_->write(buffer, size);
}

bool serial_manage::init()
{
    const auto& serial_config = system_config::get_instance()->get_serial_config();

    if ((com_fd_ = open(serial_config.dev.c_str(), O_RDWR))<0)
    {    
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Open Device %s failed!", serial_config.dev.c_str());
        return false;
    }
    else
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Open Device %s success, id:%d!", serial_config.dev.c_str(), com_fd_);
        if (set_opt(serial_config.baud, serial_config.dataBits, serial_config.parity, serial_config.stopBits) != 0)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Serial Option failed!");
            return false;
        }
    }

    serial_tx_fifo_ = std::make_unique<fifo_manage>(SERVER_UART_TX_FIFO, S_FIFO_WORK_MODE);

    uart_server_thread_ = std::thread(std::bind(&serial_manage::uart_server_run, this));
    uart_server_thread_.detach();
    uart_tx_thread_ = std::thread(std::bind(&serial_manage::uart_tx_run, this));
    uart_tx_thread_.detach();

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "serial_manage init success!");
    return true;
}

int serial_manage::write_data(char *pbuffer, uint16_t size)
{
    int send_size = -1;

    if (com_fd_ >= 0)
    {
        send_size = ::write(com_fd_, pbuffer, size);
    }

    return send_size;
}

int serial_manage::set_opt(int nBaud, int nDataBits, std::string cParity, int nStopBits)
{
    struct termios newtio;
    struct termios oldtio;

    if (tcgetattr(com_fd_, &oldtio)  !=  0) 
    { 
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Get serial attribute failed!");
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= (CLOCAL|CREAD);
    newtio.c_cflag &= ~CSIZE;

    switch (nDataBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
        default:
            break;
    }

    switch (cParity[0])
    {
        case 'O':
        case 'o':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E': 
        case 'e':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N': 
        case 'n': 
            newtio.c_cflag &= ~PARENB;
            break;
    }

    switch (nBaud)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
            break;
        case 921600:
            cfsetispeed(&newtio, B921600);
            cfsetospeed(&newtio, B921600);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }
    
    if (nStopBits == 1)
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if (nStopBits == 2)
    {
        newtio.c_cflag |=  CSTOPB;
    }
    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;

    tcflush(com_fd_, TCIFLUSH);
    if ((tcsetattr(com_fd_, TCSANOW, &newtio))!=0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Serial Config Error!");
        return -1;
    }

    PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Serial Config Done Success!");
    return 0;
}

void serial_manage::release()
{
    if (com_fd_ >= 0)
    {
        close(com_fd_);
        com_fd_ = -1;
    }
}
