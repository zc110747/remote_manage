//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      modules.hpp
//
//  Purpose:
//      组件的总接口, 提供对外的接口
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Add explian and update structure.
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "productConfig.hpp"
#include "system_config.hpp"
#include "device_manage.hpp"

uint16_t crc16(uint16_t crc, uint8_t const *buffer, uint16_t len);