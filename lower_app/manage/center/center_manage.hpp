//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      center_manage.cpp
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
_Pragma("once")

#include "modules.hpp"

class center_manage final
{
public:
    center_manage()=default;

    //单例模式不允许复制和释放
    center_manage(const center_manage&)=delete;
    virtual ~center_manage()=delete;

    static center_manage* getInstance();
    bool init();
    void run();
    
    int send_message(Event *pMsg, uint16_t size);

    //内部事件
    int sendInternalHwRefresh();
    
    //外部事件
    int sendDeviceConfig(uint8_t device, uint8_t action);
private:
    static inline center_manage* pInstance = nullptr;
    fifo_manage *pCenterFiFo{nullptr};

    bool EventProcess(Event *pEvent);
};