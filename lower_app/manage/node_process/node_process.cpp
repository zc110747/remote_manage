//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      node_process.cpp
//
//  Purpose:
//      支持node转换接口的服务器处理
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/20/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////

#include "node_process.hpp"
#include "asio_server.hpp"
#include "logger.hpp"
#include "modules.hpp"

NodeProcess*  NodeProcess::pInstance = nullptr;
NodeProcess* NodeProcess::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) NodeProcess();
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "NodeProcess new error!");
        }
    }
    return pInstance;
}

static AsioServer node_server;

void NodeProcess::run()
{
    const SocketSysConfig *pSocketConfig = SystemConfig::getInstance()->getnode();
    cmdProcess NodeProcessCmd;

    try
    {
        node_server.init(pSocketConfig->ipaddr, std::to_string(pSocketConfig->port), [&NodeProcessCmd](char* ptr, int length){
            if(NodeProcessCmd.parseData(ptr, length))
            {
                NodeProcessCmd.ProcessData();
            }
        });
        node_server.run();
    }
    catch (std::exception& e)
    {
        PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Exception:%s", e.what());
    }
}

bool NodeProcess::init()
{
    node_thread = std::thread(std::bind(&NodeProcess::run, this));
    node_thread.detach();

    return true;
}

bool NodeProcess::send(char *pbuffer, int size)
{
    bool ret = false;
    auto session_ptr = node_server.get_valid_session();

    if(session_ptr != nullptr)
    {
        session_ptr->do_write(pbuffer, size);
        ret = true;
    }
    return ret;
}
