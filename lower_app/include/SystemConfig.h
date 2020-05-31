/*
 * File      : SystemConfig.h
 * This file is for system config 
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-30      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#ifndef _INCLUDE_SYSTEM_CONFIG_H
#define _INCLUDE_SYSTEM_CONFIG_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "json/json.h"
#include <string>

/**************************************************************************
* Global Macro Definition
***************************************************************************/
struct SSystemConfig
{
    //串口配置
    int m_baud;
    int m_data_bits;
    int m_stop_bits;
    std::string m_parity;

    //网络配置
    std::string m_ipaddr;
    int m_net_port;

    //硬件状态
    int m_led0_status;
    int m_beep0_status;
};

/**************************************************************************
* Global Type Definition
***************************************************************************/

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
int system_config_init(std::string sConfigfile);
SSystemConfig *GetSSytemConfigInfo(void);
#endif
