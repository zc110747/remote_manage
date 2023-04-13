//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      uart_thread.hpp
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
_Pragma("once")

#include "protocol.hpp"

#define UART_MAX_BUFFER_SIZE     		512

class uart_thread_manage
{
public:
    /// \brief constructor
	uart_thread_manage() = default;

	/// \brief destructor, delete not allow for singleton pattern.
	~uart_thread_manage() = delete;
	
    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.	
	static uart_thread_manage* get_instance();

	/// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
	bool init();

	/// \brief release
    /// - This method is used to release the object.
	void release();

	/// \brief set_opt
    /// - This method is used to option the uart informate.
	/// \param nBaud - baud rate of the uart option
	/// \param nDataBits - data bits of the uart option
	/// \param cParity - parity of the uart option
	/// \param nStopBits - stop bits of the uart option
	/// \return wheather the option is sucess or failed
	int set_opt(int nBaud, int nDataBits, std::string cParity, int nStopBits);

    /// \brief send_msg
    /// - this method is used to send data with protocol to remote.
	/// \param buffer - the buffer of data area to remote.
	/// \param size - size of data area to remote.
	/// \return data already send to remote.
	int send_msg(char *buffer, uint16_t size);

private:
    /// \brief uart_server_run
    /// - server thread to process tcp received, tranform to rx fifo.
	void uart_server_run();

	/// \brief uart_rx_run
    /// - read from protocol rx fifo and do protocol management.
	void uart_rx_run();

	/// \brief tcp_rx_run
    /// - read from protocol tx fifo and do write data.
	void uart_tx_run();

	/// \brief write_data
    /// - this method is used to write data to uart interface.
	/// \param pbuffer - the buffer of data write to uart interface.
	/// \param size - size of data write to uart interface.
	/// \return data already send to uart interface.
	int write_data(char *pbuffer, uint16_t size);

private:
    /// \brief pInstance
    /// - object used to implement the singleton pattern.
	static uart_thread_manage* pInstance;

    /// \brief com_fd_
    /// - the fd of uart device.
	int com_fd_{-1};

	/// \brief uart_protocol_pointer_
    /// - protocol manage for uart thread.
	protocol_info *uart_protocol_pointer_;

	/// \brief mutex_
    /// - mutex used to protect com send.
	std::mutex mutex_;

	/// \brief uart_server_thread_
    /// - uart server thread object. 
	std::thread uart_server_thread_;

	/// \brief uart_rx_thread_
    /// - uart rx thread object. 
	std::thread uart_rx_thread_;

	/// \brief uart_tx_thread_
    /// - uart tx thread object. 
	std::thread uart_tx_thread_;
};

