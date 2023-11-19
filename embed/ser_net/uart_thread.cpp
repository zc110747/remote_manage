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
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "uart_thread.hpp"
#include <sys/termios.h>

uart_thread_manage* uart_thread_manage::instance_pointer_ = nullptr;
uart_thread_manage* uart_thread_manage::get_instance()
{
	if(instance_pointer_ == nullptr)
	{
		instance_pointer_ = new(std::nothrow) uart_thread_manage();
		if(instance_pointer_ == nullptr)
		{
			PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "uart_thread_manage new failed!");
		}
	}
	return instance_pointer_;
}

#define MAX_BUFFER_SIZE 512
void uart_thread_manage::uart_server_run()
{
	char buffer[MAX_BUFFER_SIZE];

	PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "%s start", __func__);

	for(;;)
	{
		int size = read(com_fd_, buffer, MAX_BUFFER_SIZE);
		if(size > 0)
		{
			uart_protocol_pointer_->write_rx_fifo(buffer, size);
		}
		else
		{
			//do nothing
		}
	}
}

void uart_thread_manage::uart_rx_run()
{
    char data;
    ENUM_PROTOCOL_STATUS status;

	PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "%s start", __func__);

	for(;;)
	{	   
        if(uart_protocol_pointer_->read_rx_fifo(&data, 1) > 0)
        {
            status = uart_protocol_pointer_->check_rx_frame(data);
            if(status == ROTOCOL_FRAME_FINISHED)
            {
                //if process, clear data received.
                uart_protocol_pointer_->process_rx_frame();
                uart_protocol_pointer_->clear_rx_info();
            }
            else if(status == PROTOCOL_FRAME_EMPTY)
            {
                uart_protocol_pointer_->clear_rx_info();
            }
            else
            {
                //in receive, do nothing
            }
        }
	}
}

void uart_thread_manage::uart_tx_run()
{
    ENUM_PROTOCOL_STATUS status;
    int size;
    char buffer[TX_BUFFER_SIZE];

	PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "%s start", __func__);
    while(1)
    {
        size = uart_protocol_pointer_->read_tx_fifo(buffer, TX_BUFFER_SIZE);
        if(size > 0)
        {
            write_data(buffer, size);
        }
        else
        {
            //do nothing
        }
    }
}

int uart_thread_manage::send_msg(char *buffer, uint16_t size)
{
    return uart_protocol_pointer_->send_data(buffer, size);
}

bool uart_thread_manage::init()
{
	const auto& serial_config = system_config::get_instance()->get_serial_config();

	if((com_fd_ = open(serial_config.dev.c_str(), O_RDWR))<0)
	{	
		PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Open Device %s failed!", serial_config.dev.c_str());
		return false;
	}
	else
	{
		if(set_opt(serial_config.baud, serial_config.dataBits, serial_config.parity, serial_config.stopBits) != 0)
		{
			PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Serial Option failed!");
			return false;
		}
	}
	
	uart_protocol_pointer_ = std::make_unique<protocol_info>();
	if(uart_protocol_pointer_ == nullptr)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "uart_protocol_pointer_ create failed!");
		return false;
	}
	auto ret = uart_protocol_pointer_->init(SERVER_UART_RX_FIFO, SERVER_UART_TX_FIFO, [this](char *ptr, int size){
		write_data(ptr, size);
	});
	if(!ret)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "uart_protocol_pointer_ init failed!");
		return false;
	}

	uart_server_thread_ = std::thread(std::bind(&uart_thread_manage::uart_server_run, this));
	uart_server_thread_.detach();
	uart_rx_thread_ = std::thread(std::bind(&uart_thread_manage::uart_rx_run, this));
	uart_rx_thread_.detach();
	uart_tx_thread_ = std::thread(std::bind(&uart_thread_manage::uart_tx_run, this));
	uart_tx_thread_.detach();

	PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "uart_thread_manage init success!");
	return true;
}

int uart_thread_manage::write_data(char *pbuffer, uint16_t size)
{
	int send_size = -1;

	if(com_fd_ >= 0)
	{
		std::lock_guard lock{mutex_};
		send_size = ::write(com_fd_, pbuffer, size);
	}

	return send_size;
}

int uart_thread_manage::set_opt(int nBaud, int nDataBits, std::string cParity, int nStopBits)
{
	struct termios newtio;
	struct termios oldtio;

	if(tcgetattr(com_fd_, &oldtio)  !=  0) 
	{ 
		PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Get serial attribute failed!");
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag |= (CLOCAL|CREAD);
	newtio.c_cflag &= ~CSIZE;

	switch(nDataBits)
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

	switch(cParity[0])
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

	switch(nBaud)
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
	
	if(nStopBits == 1)
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
	if((tcsetattr(com_fd_, TCSANOW, &newtio))!=0)
	{
		PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Serial Config Error!");
		return -1;
	}

	PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Serial Config Done Success!");
	return 0;
}

void uart_thread_manage::release()
{
	if(com_fd_ >= 0)
	{
		close(com_fd_);
		com_fd_ = -1;
	}
}
