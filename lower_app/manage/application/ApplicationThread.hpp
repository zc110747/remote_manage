//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      ApplicationThread.hpp
//
//  Purpose:
//      Deivce Application Process interface.
//
// Author:
//     	ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_APP_TASK_H
#define _INCLUDE_APP_TASK_H

#include "tools/FifoManage.hpp"
#include "tools/MqManage.hpp"

#define REG_NUM                 256
#define REG_CONFIG_NUM          64
#define REG_INFO_NUM            192

/*设备配置信息*/
#define DEVICE_LED0             1
#define DEVICE_BEEP             2
#define DEVICE_REBOOT           3

union UBaseStatus
{
    uint32_t d32;

    struct 
    {
        uint32_t led:1;
        uint32_t beep:1;
        uint32_t reserved:30;
    }b;
};

#pragma pack(push)
#pragma pack(1)
struct SRegInfoList
{
    /*系统基本状态信息，包含LED，Beep状态*/
    union UBaseStatus s_base_status;

    /*传感器红外线强度*/
    uint16_t sensor_ir; 

    /*传感器环境光强度*/        
    uint16_t sensor_als;

    /*传感器接近距离*/  
    uint16_t sensor_ps;

    uint16_t reserved0;

    /*陀螺仪x轴角速度*/
    uint32_t sensor_gyro_x;

    /*陀螺仪y轴角速度*/
    uint32_t sensor_gyro_y; 

    /*陀螺仪z轴角速度*/
    uint32_t sensor_gyro_z;

    /*加速度计x轴加速度*/
    uint32_t sensor_accel_x;

    /*加速度计y轴加速度*/
    uint32_t sensor_accel_y; 

    /*加速度计z轴加速度*/
    uint32_t sensor_accel_z;

    /*温度采样*/
    uint32_t sensor_temp;
};
#pragma pack(pop)

class CApplicationReg
{
public:
    CApplicationReg(void);
        ~CApplicationReg();
        
    //device process
    int RefreshAllDevice(void);
    void WriteDeviceConfig(uint8_t cmd, uint8_t *pRegConfig, int size);
    bool ReadDeviceStatus(void);

    //reg process
    void SetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart);
    uint16_t GetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart);
    int DiffSetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart, uint8_t *pDataCompare);
private:
    uint8_t m_RegVal[REG_NUM];
    pthread_mutex_t m_RegMutex; /*数据读取都要执行该锁*/
};

class ApplicationThread
{
private:
    static ApplicationThread *pInstance;
    CApplicationReg *pApplicationReg{nullptr};
    MessageBase *pAppMessageInfo{nullptr};
    
    std::thread *pthread{nullptr};

public:
    ApplicationThread();
    ApplicationThread(CApplicationReg *pReg);
        ~ApplicationThread();

    bool init();
    void TimerSingalStart(void);
    static ApplicationThread *getInstance();
    CApplicationReg *GetApplicationReg() {return pApplicationReg;}
    MessageBase *getAppMessage()    {return pAppMessageInfo;}
};
#endif
