/*
 * File      : SystemConfig.cpp
 * 系统配置信息文件处理
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
#include <fstream>
#include <iostream>
#include "SystemConfig.hpp"


// SystemConfig::SystemConfig()
// {

// }

// SystemConfig::~SystemConfig()
// {

// }

// SystemConfig* SystemConfig::pInstance = nullptr;
// SystemConfig* SystemConfig::getInstance()
// {
//     if(pInstance == nullptr)
//     {
//         pInstance = new(std::nothrow) SystemConfig();
//         if(pInstance == nullptr)
//         {
//             //to do error process
//         }
//     }
//     return pInstance;
// }

// bool SystemConfig::init(const char* path)
// {

// }

// void SystemConfig::default_init()
// {
//     parameter.led.init = DEFAULT_LED_INIT;
//     parameter.led.dev = DEFALUT_LED_DEV;

// }

// /**************************************************************************
// * Local Macro Definition
// ***************************************************************************/

// /**************************************************************************
// * Local Type Definition
// ***************************************************************************/

// /**************************************************************************
// * Local static Variable Declaration
// ***************************************************************************/
static SSystemConfig SSysConifg = {
    //串口配置
    BAUD,
    DATABITS,
    STOPBITS,
    std::string(PARITY),

    //TCP网络设置
    std::string(IP_ADDR),
    TCP_PORT,

    //UDP网络设置
    std::string(IP_ADDR),
    UDP_PORT,

    //硬件状态
    0,
    0,

    //设备名称
    std::string(TTY_DEVICE),
    std::string(LED_DEVICE),
    std::string(BEEP_DEVICE),
    std::string(ICM_SPI_DEVICE),
    std::string(RTC_DEVICE),
    std::string(AP_I2C_DEVICE),

    //下载文件的路径
    std::string(UPDATE_FILE_PATH),
};
// /**************************************************************************
// * Global Variable Declaration
// ***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/**************************************************************************
* Function
***************************************************************************/
/**
 * 系统配置信息初始化
 * 
 * @param sConfigfile 传入的系统配置信息文件
 *  
 * @return 硬件配置处理的结果
 */
int SystemConfigInfo(std::string &sConfigfile)
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

    //sokcet TCP状态
    SSysConifg.m_tcp_ipaddr = std::string(root["SocketTcp"]["ipaddr"].asString());
    SSysConifg.m_tcp_net_port = root["SocketTcp"]["net_port"].asInt();

    //sokcet UDP状态
    SSysConifg.m_udp_ipaddr = std::string(root["SocketUdp"]["ipaddr"].asString());
    SSysConifg.m_udp_net_port = root["SocketUdp"]["net_port"].asInt();

    //硬件状态
    SSysConifg.m_led0_status = root["Led0"].asInt();
    SSysConifg.m_beep0_status = root["Beep0"].asInt();

    //设备名称
    SSysConifg.m_dev_led = root["Device"]["Led"].asString();
    SSysConifg.m_dev_beep = root["Device"]["Beep"].asString();
    SSysConifg.m_dev_serial = root["Device"]["Serial"].asString();
    SSysConifg.m_dev_icm_spi = root["Device"]["IcmSpi"].asString();
    SSysConifg.m_dev_rtc = root["Device"]["Rtc"].asString();
    SSysConifg.m_dev_ap_i2c = root["Device"]["ApI2c"].asString();

    //下载路径
    SSysConifg.m_file_path = std::string(root["FilePath"].asString());
    ifs.close();
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


/**
 * 显示系统的配置信息
 * 
 * @param NULL
 *  
 * @return NULL
 */
#if __SYSTEM_DEBUG == 1
void ShowSSystemConfigInfo(void)
{
    //Serial
    std::cout<<"Baud:"<<SSysConifg.m_baud<<std::endl;
    std::cout<<"DataBits:"<<SSysConifg.m_data_bits<<std::endl;
    std::cout<<"StopBits:"<<SSysConifg.m_stop_bits<<std::endl;
    std::cout<<"Parity:"<<SSysConifg.m_parity<<std::endl;

    //Tcp
    std::cout<<"Tcp Ipaddr:"<<SSysConifg.m_tcp_ipaddr<<std::endl;
    std::cout<<"Tcp Port:"<<SSysConifg.m_tcp_net_port<<std::endl;

    //Udp
    std::cout<<"Udp Ipaddr:"<<SSysConifg.m_udp_ipaddr<<std::endl;
    std::cout<<"Udp port:"<<SSysConifg.m_udp_net_port<<std::endl;

    //Hardwart Status
    std::cout<<"led status:"<<SSysConifg.m_led0_status<<std::endl;
    std::cout<<"beep status:"<<SSysConifg.m_beep0_status<<std::endl;

    //Device Name
    std::cout<<"tty dev:"<<SSysConifg.m_dev_serial<<std::endl;
    std::cout<<"led dev:"<<SSysConifg.m_dev_led<<std::endl;
    std::cout<<"beep dev:"<<SSysConifg.m_dev_beep<<std::endl;

    //Download Directory
    std::cout<<"Download:"<<SSysConifg.m_file_path<<std::endl;
}
#endif
