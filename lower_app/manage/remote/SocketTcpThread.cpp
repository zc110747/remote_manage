//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SocketTcpThread.cpp
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
#include "SocketTcpThread.hpp"
#include "asio_server.hpp"

static AsioServer socket_tcp_server;

void TcpThreadManage::tcp_server_run()
{
    const SocketSysConfig *pSocketConfig = SystemConfig::getInstance()->gettcp();
    
    try
    {
        socket_tcp_server.init(pSocketConfig->ipaddr, std::to_string(pSocketConfig->port), [this](char* ptr, int length){
            protocol_info_ptr_->write_rx_fifo(ptr, length);
        });
        socket_tcp_server.run();
    }
    catch (std::exception& e)
    {
        PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "TcpThreadManage, Exception:%s", e.what());
    }
}


void TcpThreadManage::tcp_rx_run()
{
    char data;
    ENUM_PROTOCOL_STATUS status;

    while(1)
    {
        if(protocol_info_ptr_->read_rx_fifo(&data, 1) > 0)
        {
            status = protocol_info_ptr_->check_rx_frame(data);
            if(status == ROTOCOL_FRAME_FINISHED)
            {
                //if process, clear data received.
                protocol_info_ptr_->process_rx_frame();
                protocol_info_ptr_->clear_rx_info();
            }
            else if(status == PROTOCOL_FRAME_EMPTY)
            {
                protocol_info_ptr_->clear_rx_info();
            }
            else
            {
                //in receive, do nothing
            }
        }
    }
}

void TcpThreadManage::tcp_tx_run()
{
    ENUM_PROTOCOL_STATUS status;
    int size;
    char buffer[TX_BUFFER_SIZE];

    while(1)
    {
        size = protocol_info_ptr_->read_tx_fifo(buffer, TX_BUFFER_SIZE);
        if(size > 0)
        {
            socket_tcp_server.do_write(buffer, size);
        }
    }
}

bool TcpThreadManage::init()
{
    m_server_thread = std::thread(std::bind(&TcpThreadManage::tcp_server_run, this));
    m_server_thread.detach();
    m_rx_thread = std::thread(std::bind(&TcpThreadManage::tcp_rx_run, this));
    m_rx_thread.detach();
    // m_tx_thread = std::thread(std::bind(&TcpThreadManage::tcp_tx_run, this));
    // m_tx_thread.detach();

    protocol_info_ptr_ = new(std::nothrow) protocol_info(SOCKET_TCP_RX_FIFO, SOCKET_TCP_TX_FIFO);
    if(protocol_info_ptr_ == nullptr)
        return false;
    if(!protocol_info_ptr_->init())
        return false;

    return true;
}


TcpThreadManage* TcpThreadManage::pInstance = nullptr;
TcpThreadManage* TcpThreadManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) TcpThreadManage;
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Tcp thread manage new failed!");
        }
    }
    return pInstance;
}