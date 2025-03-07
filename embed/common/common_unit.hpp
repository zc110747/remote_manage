//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      common.hpp
//
//  Purpose:
//      全局的用于支持项目编译包含的库
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common.hpp"

//common unit for all application
#include "tty.hpp"
#include "timer_manage.hpp"
#include "fifo/fifo_manage.hpp"
#include "jsonconfig/json_config.hpp"
#include "logger/logger_manage.hpp"

uint16_t crc16(uint16_t crc, uint8_t const *buffer, uint16_t len);
