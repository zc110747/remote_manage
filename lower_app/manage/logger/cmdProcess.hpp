//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      cmdProcess.hpp
//
//  Purpose:
//      cmdProcess Interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      8/8/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include "../include/includes.hpp"
#include <map>

#define COMMAND_MAX_SIZE    4
typedef enum
{
    CmReadDev = 0,
    CmSetDev,
    CmGetNet,
    CmGetSer
}CmdFormat_t;

class cmdProcess
{
private:
    static cmdProcess* pInstance;    
    std::map<std::string, CmdFormat_t> mapM;
    char *pDataM;
    CmdFormat_t formatM;

public:
    cmdProcess();
        ~cmdProcess();
    static cmdProcess* getInstance();
    bool init();
    bool parseData(char *ptr, int size);
    bool ProcessData();
};

