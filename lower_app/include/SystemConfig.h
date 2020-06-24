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
#include "UsrTypeDef.h"

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

    //网络配置TCP
    std::string m_tcp_ipaddr;
    int m_tcp_net_port;

    //网络配置UDP
    std::string m_udp_ipaddr;
    int m_udp_net_port;

    //硬件状态
    int m_led0_status;
    int m_beep0_status;

    //Device结构
    std::string m_dev_led;
    std::string m_dev_beep;
    std::string m_dev_serial;
    std::string m_dev_icm_spi;
    
    //文件更新的下载地址
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
int system_config_init(std::string &sConfigfile);
SSystemConfig *GetSSytemConfigInfo(void);
#if __SYSTEM_DEBUG == 1
void ShowSSystemConfigInfo(void);
#endif
#endif
