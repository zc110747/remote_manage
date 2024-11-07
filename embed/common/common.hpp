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

//c interface
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <stdarg.h>

//c++ interface
#include <new>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <atomic>
#include <map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <vector>
#include <functional>

#include "event.hpp"
#include "thread_queue.hpp"
#include "productConfig.hpp"

typedef union
{
    float f_val;

    int i_val;

    uint8_t buffer[4];
}UNION_FLOAT_INT;

#define DEVICE_RX_BUFFER_SIZE           512
#define DEVICE_TX_BUFFER_SIZE           512

//logger fifo list
#define LOGGER_RX_FIFO                  "/tmp/app/log_rx.fifo"          //all device => logger(rx fifo)
#define LOGGER_GUI_TX_FIFO              "/tmp/app/log_gui_tx.fifo"      //logger => gui
#define LOGGER_LOC_DEV_TX_FIFO          "/tmp/app/log_loc_dev_tx.fifo"  //logger => local device
#define LOGGER_LOW_DEV_TX_FIFO          "/tmp/app/log_low_dev_tx.fifo"  //logger => lower device
#define LOGGER_MP_TX_FIFO               "/tmp/app/log_mp_tx.fifo"       //logger => mp device

//local device list
#define LOCAL_DEVICE_CMD_FIFO           "/tmp/app/local_dev_cmd.fifo" //main_process => local_device
#define LOCAL_DEVICE_INFO_FIFO          "/tmp/app/local_dev_info.fifo" //local_device => main_process

//device name
#ifndef TOOLS_NAME
#define TOOLS_NAME                      "common"
#endif

#ifndef PRINT_NOW_HEAD_STR
#define PRINT_NOW_HEAD_STR              "[0000 00:00:00][common][5]"
#endif

#if __cplusplus < 201703
#error "c++ compiler need newer than g++7, can use 'g++ -v' to see version."
#endif

#define CREATE_UINT16(a, b)             ((a)<<8 | (b))
#define CREATE_UINT32(a, b, c, d)       ((a)<<24 | (b)<<16 | (c)<<8 | d)
#define CREATE_FLOAT(a, b, c, d)        (float)((a)<<24 | (b)<<16 | (c)<<8 | d)

#define DEVICE_LOOP_EVENT               0
#define DEVICE_HW_SET_EVENT             1
#define DEVICE_SYNC_EVENT               2
#define DEVICE_PER_SECOND_EVENT         3

//process hardware chage
#define DEVICE_LED                      0x00
#define DEVICE_BEEP                     0x01
#define DEVICE_PWM                      0x02
#define DEVICE_RTC                      0x03
#define DEVICE_RTC_ALARM                0x04
#define DEVICE_LOOPLED                  0x05

#define LOCAL_DEVICE_ID                 0

#define COMMAND_HEATBEATS               0x00
#define COMMAND_UPDATE_LOCAL            0x01
#define COMMAND_UPDATE_REMOTE           0x02

#define MQTT_SOURCE_NODE                0
#define MQTT_SOURCE_GUI                 1
#define MQTT_SOURCE_DESKTOP             2
