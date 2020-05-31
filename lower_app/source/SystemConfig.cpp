/*
 * File      : SystemConfig.cpp
 * This file is for SystemConfig
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/

#include "../include/SystemConfig.h"
#include "../driver/Led.h"
#include "../driver/Beep.h"
#include <fstream>
#include <iostream>

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static SSystemConfig SSysConifg = {
    //串口配置
    115200,
    8,
    1,
    std::string("n"),

    //网络设置
    std::string("127.0.0.1"),
    8000,

    //硬件状态
    0,
    0
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/**************************************************************************
* Local Function
***************************************************************************/

/**************************************************************************
* Function
***************************************************************************/
/**
 * 系统硬件初始化
 * 
 * @param NULL
 *  
 * @return 硬件配置处理的结果
 */
int system_config_init(std::string sConfigfile)
{
    Json::Value root;
    std::ifstream ifs;
    ifs.open(sConfigfile.c_str());

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        std::cout << errs << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << root << std::endl;

    //串口状态
    SSysConifg.m_baud = root["Uart"]["Baud"].asInt();
    SSysConifg.m_data_bits = root["Uart"]["DataBits"].asInt();
    SSysConifg.m_stop_bits = root["Uart"]["StopBits"].asInt();
    SSysConifg.m_parity = std::string(root["Uart"]["Parity"].asString());

    //sokcet状态
    SSysConifg.m_ipaddr = std::string(root["Socket"]["ipaddr"].asString());
    SSysConifg.m_net_port = root["Socket"]["net_port"].asInt();

    //硬件状态
    SSysConifg.m_led0_status = root["Led0"].asInt();
    SSysConifg.m_beep0_status = root["Beep0"].asInt();

    LedStatusConvert((uint8_t)SSysConifg.m_led0_status);
    BeepStatusConvert((uint8_t)SSysConifg.m_beep0_status);
    return EXIT_SUCCESS;
}

/**
 * 获取系统状态配置信息
 * 
 * @param NULL
 *  
 * @return 返回设备状态信息
 */
SSystemConfig *GetSSytemConfigInfo(void)
{
    return &SSysConifg;
}