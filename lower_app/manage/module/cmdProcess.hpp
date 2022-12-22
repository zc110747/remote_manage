//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      cmdProcess.hpp
//
//  Purpose:
//      用于支持命令行处理的接口, 包含字符串处理和事件触发
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "includes.hpp"

#define COMMAND_MAX_SIZE    4
typedef enum
{
    CmdReadDev = 0,
    CmdGetOS,
    CmdSetDev,
    cmdSetLevel,
    CmdGetHelp,
    CmdConnect,
}CmdFormat_t;

class cmdProcess
{
private:
    char *pDataM;
    CmdFormat_t formatM;

public:
    cmdProcess() = default;
    
    bool init();
    bool parseData(char *ptr, int size);
    bool ProcessData();
};

