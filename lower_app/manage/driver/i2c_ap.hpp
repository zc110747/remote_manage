//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      i2c_ap.cpp
//
//  Purpose:
//      i2c read ap interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _I2C_AP_HPP
#define _I2C_AP_HPP

#include "deviceBase.hpp"

typedef struct _SApInfo
{
    uint16_t ir;     /*红外线强度*/
    uint16_t als;    /*环境光强度*/
    uint16_t ps;     /*接近距离*/
}AP_INFO;

class APDevice:public InfoBase<AP_INFO>
{
private:
    static APDevice *pInstance;

public:
    //constructor
    using InfoBase::InfoBase;

    static APDevice *getInstance();
    void release();
};
#endif
