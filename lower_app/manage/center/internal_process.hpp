//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      node_process.hpp
//
//  Purpose:
//      支持node转换接口的服务器
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/20/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "includes.hpp"
#include "logger.hpp"
#include "modules.hpp"

class internal_process final
{
private:
    static internal_process*  pInstance;
    std::thread node_thread;
    cmd_process internal_processCmd;

    void ProcessCallback();

public:
    internal_process() = default;
    ~internal_process() = delete;

    static internal_process* getInstance();
    bool init();
    void run();
    bool send(char *pbuffer, int size);

    void SendStatusBuffer(NAMESPACE_DEVICE::device_read_info &info);
};