//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      CenterUnit.hpp
//
//  Purpose:
//      用于管理整个数据通讯的核心线程
//
// Author:
//     	Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#define CENTER_UNIT_FIFO    "/tmp/CenterUnit.fifo"

class CenterUnit final
{
public:
    CenterUnit()=default;

    //单例模式不允许复制和释放
    CenterUnit(const CenterUnit&)=delete;
    virtual ~CenterUnit()=delete;

    static CenterUnit* getInstance();
    void init();
    void run();

private:
    static inline CenterUnit* pInstance = nullptr;
    FIFOMessage *pCenterFiFo{nullptr};
};