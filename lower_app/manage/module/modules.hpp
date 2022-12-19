//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      UnityMain.hpp
//
//  Purpose:
//      global process by unity main interface.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "productConfig.hpp"
#include "SystemConfig.hpp"
#include "DeviceManageThread.hpp"

uint16_t crc16(uint16_t crc, uint8_t const *buffer, uint16_t len);