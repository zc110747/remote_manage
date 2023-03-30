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
_Pragma("once")

#include "modules.hpp"

class CenterManage final
{
public:
    CenterManage()=default;

    //单例模式不允许复制和释放
    CenterManage(const CenterManage&)=delete;
    virtual ~CenterManage()=delete;

    static CenterManage* getInstance();
    bool init();
    void run();
    
    //内部事件
    void informHwUpdate();

private:
    static inline CenterManage* pInstance = nullptr;
    FIFOManage *pCenterFiFo{nullptr};

    bool EventProcess(Event *pEvent);
};