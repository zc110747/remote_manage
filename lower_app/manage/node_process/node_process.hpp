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

class NodeProcess final
{
private:
    static NodeProcess*  pInstance;
    std::thread node_thread;
    
public:
    NodeProcess() = default;
    ~NodeProcess() = delete;

    static NodeProcess* getInstance();
    bool init();
    void run();
    bool send(char *pbuffer, int size);
};