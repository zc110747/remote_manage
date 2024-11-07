//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      asio_client.hpp
//
//  Purpose:
//      asio客户端模块
//
// Author:
//         @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      11/20/2023   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "asio.hpp"
#include "common_unit.hpp"

#define CLIENT_RX_MAX_BUFFER_SIZE        1024
#define CLIENT_TX_MAX_BUFFER_SIZE        1024

class asio_client
{
public:
    /// \brief asio_client
    explicit asio_client()
    :io_context_(2), socket_(io_context_)
    {
        memset(rx_buffer_, 0, CLIENT_RX_MAX_BUFFER_SIZE);
        memset(tx_buffer_, 0, CLIENT_TX_MAX_BUFFER_SIZE);
    }

    /// \brief destructor
    ~asio_client() = delete;

public:
    /// \brief send_msg
    /// - this method is used to send data with protocol to remote.
    /// \param buffer - the buffer of data area to remote.
    /// \param size - size of data area to remote.
    /// \return data already send to remote.
    int send_msg(char *buffer, uint16_t size);

    /// \brief init
    /// - This method is used to init the object.
    /// \param timeInterval - period for the timer_struct run.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static asio_client *get_instance();
private:
    /// \brief asio_client_rx_run
    /// - tx thread to process asio client rx info.
    void asio_client_rx_run();

    /// \brief logger_rx_server_run
    /// - rx thread to process logger interface received.
    void asio_client_tx_run();

    /// \brief write_data
    /// - this method is used to write data to uart interface.
    /// \param pbuffer - the buffer of data write to uart interface.
    /// \param size - size of data write to uart interface.
    /// \return data already send to uart interface.
    int write_data(char *pbuffer, uint16_t size);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static asio_client *instance_pointer_;

    /// \brief is_client_link
    /// - wheather client is link.
    std::atomic<bool> is_client_link_{false};

    /// \brief client_tx_thread_
    /// - client tx thread object.
    std::thread client_tx_thread_;

    /// \brief client_rx_thread_
    /// - client rx thread object.
    std::thread client_rx_thread_;

    /// \brief logger_fifo_
    /// - fifo used for logger manage.
    std::unique_ptr<fifo_manage> client_tx_fifo_{nullptr};

    /// \brief rx_buffer_
    /// - rx buffer for receive.
    char rx_buffer_[CLIENT_RX_MAX_BUFFER_SIZE];

    /// \brief tx_buffer_
    /// - tx buffer for receive.
    char tx_buffer_[CLIENT_TX_MAX_BUFFER_SIZE];

    /// \brief io_context_
    /// - manage the context of the server.
    asio::io_context io_context_;

    /// \brief acceptor_
    /// - manage the tcp information.
    asio::ip::tcp::socket socket_;
};