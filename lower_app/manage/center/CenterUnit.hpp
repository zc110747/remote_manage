//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      CenterUnit.cpp
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

#define CENTER_UNIT_FIFO    "/tmp/CenterUnit.fifo"

class CenterUnit final
{
public:
    CenterUnit()=default;

    //单例模式不允许复制和释放
    CenterUnit(const CenterUnit&)=delete;
    virtual ~CenterUnit()=delete;

    static CenterUnit* getInstance();
    bool init();
    void run();
    
    //内部事件
    void informHwUpdate();

private:
    static inline CenterUnit* pInstance = nullptr;
    FIFOManage *pCenterFiFo{nullptr};

    bool EventProcess(Event *pEvent);
};