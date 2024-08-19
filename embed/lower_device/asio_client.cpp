//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      asio_client.cpp
//
//  Purpose:
//      asio客户端模块
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      11/20/2023   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "asio_client.hpp"
#include "serial.hpp"

asio_client* asio_client::instance_pointer_ = nullptr;
asio_client* asio_client::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) asio_client;
        if (instance_pointer_ == nullptr)
        {
            //do something
        }
    }
    return instance_pointer_;
}

bool asio_client::init()
{
    //create the tx fifo
    client_tx_fifo_ = std::make_unique<fifo_manage>(ASIO_CLENET_FIFO, S_FIFO_WORK_MODE);
    if (client_tx_fifo_ == nullptr)
        return false;
    if (!client_tx_fifo_->create())
        return false;

    client_rx_thread_ = std::thread(std::bind(&asio_client::asio_client_rx_run, this));
    client_rx_thread_.detach();
    client_tx_thread_ = std::thread(std::bind(&asio_client::asio_client_tx_run, this));
    client_tx_thread_.detach();

    return true;
}

int asio_client::send_msg(char *buffer, uint16_t size)
{
    int out_size;
    out_size = client_tx_fifo_->write(buffer, size);
    return out_size;
}

void asio_client::asio_client_tx_run()
{
    int size;

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "%s start", __func__);
    while (1)
    {
        size = client_tx_fifo_->read(tx_buffer_, CLIENT_TX_MAX_BUFFER_SIZE);
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

void asio_client::asio_client_rx_run()
{
    asio::ip::tcp::resolver resolver(io_context_);
    const auto& ipaddress = system_config::get_instance()->get_local_ipaddress();
    const auto& port = system_config::get_instance()->get_lower_device_remote_port();

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "%s start", __func__);

    while (1)
    {
        try
        {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "start asio client connet:%s:%d", ipaddress.c_str(), port);
            asio::connect(socket_, resolver.resolve(ipaddress, std::to_string(port)));
            is_client_link_ = true;
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "asio client connet success!");

            do
            {
                size_t reply_length = asio::read(socket_, asio::buffer(rx_buffer_, CLIENT_RX_MAX_BUFFER_SIZE));
                if (reply_length > 0)
                {
                    serial_manage::get_instance()->send_msg(rx_buffer_, reply_length);
                }
                else
                {
                    //do nothing
                }
            }while (1);
        }
        catch(std::exception& e)
        {
            is_client_link_ = false;
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "asio client run error:%s", e.what());

            //for client, detect link every 15s
            std::this_thread::sleep_for(std::chrono::seconds(15));
        }
    }
}

int asio_client::write_data(char *pbuffer, uint16_t size)
{
    int send_size = -1;

    if (is_client_link_)
    {
        send_size = asio::write(socket_, asio::buffer(pbuffer, size));
    }

    return send_size;
}