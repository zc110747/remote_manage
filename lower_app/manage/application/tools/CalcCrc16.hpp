//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      CalcCrc16.cpp
//
//  Purpose:
//      Calculate crc16 interface.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_CALC_CRC16_HPP
#define _INCLUDE_CALC_CRC16_HPP

#include "../../include/productConfig.hpp"

uint16_t crc16(uint16_t crc, uint8_t const *buffer, uint16_t len);
#endif
