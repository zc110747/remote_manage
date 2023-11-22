//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      i2c_ap.hpp
//
//  Purpose:
//      AP模块对应的I2C接口，通过继承支持直接读取模块信息
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

#include "device_base.hpp"

typedef struct _SApInfo
{
    uint16_t ir;     /*红外线强度*/
    uint16_t als;    /*环境光强度*/
    uint16_t ps;     /*接近距离*/
}ap_info;

class ap_device:public info_base<ap_info>
{
public:
    /// \brief constructor
    using info_base::info_base;
};
