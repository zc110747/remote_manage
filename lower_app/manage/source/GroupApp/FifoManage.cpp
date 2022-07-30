//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      FIFOMessage.cpp
//
//  Purpose:
//      Communication by fifo.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "FIFOManage.hpp"

#if __WORK_IN_WSL == 1
std::vector<std::string> FIFOPath = {
    SYSTEM_FIFO,
    APPLICATION_FIFO,
};

FIFOManage::FIFOManage()
{
    for(auto& path:FIFOPath)
    {
        auto ptr = new(std::nothrow) FIFOMessage(path, FIFO_MODE);
        if(ptr == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "FIFOMessage %s new failed!", path.c_str());
        }
        else
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "FIFOMessage %s new success!", path.c_str());
        }
        messageM.push_back(ptr);
    }
}

FIFOManage::~FIFOManage()
{
    FIFOMessage *pMessage;

    do
    {
        pMessage = *(messageM.begin());
        if(pMessage != nullptr)
        {
            pMessage->Release();
            delete pMessage;
        }
        messageM.erase(messageM.begin());
    } while(messageM.size());
}

FIFOManage* FIFOManage::pInstance = nullptr;
FIFOManage* FIFOManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) FIFOManage;
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "FIFOManage new failed!");
        }
    }
    return pInstance;
}

bool FIFOManage::init()
{
    bool ret = true;

#if __WORK_IN_WSL == 1
    for(auto& val:messageM)
    {
        if(val == nullptr)
        {
            ret = false;
            break;
        }
        ret = ret && val->Create();
    }
#endif
    if(ret == false)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "FIFOManage init failed!");
    }
    return ret;
}
#endif
