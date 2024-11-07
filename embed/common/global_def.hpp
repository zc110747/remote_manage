//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      global_def.hpp
//
//  Purpose:
//      用于全局共享的定义
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

//定义common执行的进程
#define PROCESS_MAIN_PROCESS            0
#define PROCESS_LOWER_DEVICE            1
#define PROCESS_LOCAL_DEVICE            2
#define PROCESS_LOGGER_TOOL             3

//定义全局的返回状态
#define RT_INVALID_MQ_SIZE              -4
#define RT_INVALID_BUF_SIZE             -3
#define RT_INVALID_MQ                   -2
#define RT_OK                           0
#define RT_FAIL                         1
#define RT_EMPTY                        2
#define RT_TIMEOUT                      3
#define RT_INVALID                      4

//定义使用的配置文件名称
#define DEFAULT_CONFIG_FILE             "config.json"
