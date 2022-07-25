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
#ifndef _INCLUDE_SYSTEM_CONFIG_HPP
#define _INCLUDE_SYSTEM_CONFIG_HPP

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "json/json.h"
#include "../include/productConfig.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
struct SSystemConfig
{
    /*串口配置*/
    int m_baud;
    int m_data_bits;
    int m_stop_bits;
    std::string m_parity;

    /*TCP Socket配置*/
    std::string m_tcp_ipaddr;
    int m_tcp_net_port;

    /*UDP Socket配置*/
    std::string m_udp_ipaddr;
    int m_udp_net_port;

    /*硬件的初始化状态*/
    int m_led0_status;
    int m_beep0_status;

    /*Device配置信息*/
    std::string m_dev_led;
    std::string m_dev_beep;
    std::string m_dev_serial;
    std::string m_dev_icm_spi;
    std::string m_dev_rtc;
    std::string m_dev_ap_i2c;
    
    /*文件更新的下载地址*/
    std::string m_file_path;
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

/*系统配置信息初始化*/
int SystemConfigInfo(std::string &sConfigfile);

/*获取当前的系统配置信息*/
SSystemConfig *GetSSytemConfigInfo(void);
#if __SYSTEM_DEBUG == 1
void ShowSSystemConfigInfo(void);
#endif
#endif
