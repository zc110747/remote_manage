//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      tcp_thread.hpp
//
//  Purpose:
//      TCP协议执行模块，目前设计为同步模式，后续会修改
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

#define SOCKET_BUFFER_SIZE		1200

class tcp_thread_manage
{
public:
    /// \brief constructor
	tcp_thread_manage() = default;

	/// \brief destructor, delete not allow for singleton pattern.
	~tcp_thread_manage() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
	static tcp_thread_manage* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
	bool init();

    /// \brief send_msg
    /// - this method is used to send data with protocol to remote.
	/// \param buffer - the buffer of data area to remote.
	/// \param size - size of data area to remote.
	/// \return data already send to remote.
	int send_msg(char *buffer, uint16_t size);

private:
    /// \brief tcp_server_run
    /// - server thread to process tcp received, tranform to rx fifo.
	void tcp_server_run();

	/// \brief tcp_rx_run
    /// - read from protocol rx fifo and do protocol management.
	void tcp_rx_run();

	/// \brief tcp_rx_run
    /// - read from protocol tx fifo and do write data.
	void tcp_tx_run();

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
	static tcp_thread_manage* instance_pointer_;

	/// \brief tcp_server_thread_
    /// - tcp server thread object. 
	std::thread tcp_server_thread_;

	/// \brief tcp_rx_thread_
    /// - tcp rx thread object. 
	std::thread tcp_rx_thread_;

	/// \brief tcp_tx_thread_
    /// - tcp tx thread object. 
	std::thread tcp_tx_thread_;

	/// \brief tcp_protocol_pointer_
    /// - pointer of the protocol process object for tcp application. 
	protocol_info* tcp_protocol_pointer_;
};

