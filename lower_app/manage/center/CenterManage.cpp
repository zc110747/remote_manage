//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      CenterManage.cpp
//
//  Purpose:
//      核心管理模块，用于接收所有模块的数据，进行管理并分发
//      输入:网络通讯模块, QT界面事件，node输入事件, 设备状态更新，logger调试接口
//      输出：本地硬件设备
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
#include "CenterManage.hpp"
#include "logger.hpp"
#include "InternalProcess.hpp"
#include <new>

CenterManage* CenterManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) CenterManage;
        if(pInstance == nullptr)
        {
             PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "DeviceManageThread new error!");
        }
    }
    return pInstance;
}

void CenterManage::informHwUpdate()
{
    Event event(WORKFLOW_ID_HARDWARE_UPDATE);
    pCenterFiFo->write(reinterpret_cast<char *>(&event), sizeof(event));
}

bool CenterManage::EventProcess(Event *pEvent)
{
    uint16_t id = pEvent->getId();
    switch(id)
    {
    case WORKFLOW_ID_HARDWARE_UPDATE:
        {
            auto info = NAMESPACE_DEVICE::DeviceManageThread::getInstance()->getDeviceInfo();
            InterProcess::getInstance()->SendStatusBuffer(info);
        }
        break;

    default:
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device Command:%d!", id);
        break;
    }
    return true;
}

void CenterManage::run()
{
    char buffer[1024];
    int size;
    
    for(;;)
    {
        size = pCenterFiFo->read(buffer, READ_BUFFER_SIZE);
        if(size > 0)
        {
            PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "CenterManage Command, %d!", size);
            EventProcess(reinterpret_cast<Event *>(buffer));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

bool CenterManage::init()
{
    //clear thread
    std::thread(std::bind(&CenterManage::run, this)).detach();
    
    pCenterFiFo = new(std::nothrow) FIFOManage(CENTER_UNIT_FIFO, S_FIFO_WORK_MODE);
    if(pCenterFiFo == nullptr)
    {
        return false;
    }
    
    return pCenterFiFo->Create();
}