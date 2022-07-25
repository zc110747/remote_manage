//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     SystemConfig.hpp
//
//  Purpose:
//     SystemConfig Interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/25/2022   Create New Version
//////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_SYSTEM_CONFIG_HPP
#define _INCLUDE_SYSTEM_CONFIG_HPP

#include "json/json.h"
#include "../include/productConfig.hpp"

// using JString = Json::String;

// typedef struct 
// {
//     int init;
//     JString dev;
// }IoSysConfig;

// typedef struct
// {
//     int baud;
//     int dataBits;
//     int stopBits;
//     JString parity;
//     JString dev;
// }SerialSysConfig;

// typedef struct 
// {
//     int port;
//     JString ipaddr;
// }SocketSysConfig;

// typedef struct 
// {
//     JString path;
// }DeviceSysConfig;

// typedef struct 
// {
//     IoSysConfig led; 

//     IoSysConfig beep;   

//     SerialSysConfig serial;

//     SocketSysConfig tcp;

//     SocketSysConfig udp;

//     DeviceSysConfig rtc;

//     DeviceSysConfig icmSpi;

//     DeviceSysConfig apI2c; 
// }SystemParamter;


// class SystemConfig
// {
// private:
//     static SystemConfig *pInstance;
//     SystemParamter parameter;

// public:
//     SystemConfig();
//         ~SystemConfig();

//     static SystemConfig* getInstance();
//     bool init(const char* path);
//     void default_init();

// public:
//     const IoSysConfig *getled() {return &(parameter.led);}
//     const IoSysConfig *getbeep()  {return &(parameter.beep);}
//     const SerialSysConfig *getserial()    {return &(parameter.serial);}
//     const SocketSysConfig *gettcp()   {return &(parameter.tcp);}
//     const SocketSysConfig *getudp()   {return &(parameter.udp);}
//     const DeviceSysConfig *getrtc()   {return &(parameter.rtc);}
//     const DeviceSysConfig *geticmSpi()   {return &(parameter.icmSpi);}
//     const DeviceSysConfig *getapI2c()   {return &(parameter.apI2c);}
// }



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
