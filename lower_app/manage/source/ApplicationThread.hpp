/* 
 * File      : app_task.h
 * application task interface
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date         Author       Notes
 * 2020-5-4     zc           the first version
 * 2020-5-20     zc           Code standardization 
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#ifndef _INCLUDE_APP_TASK_H
#define _INCLUDE_APP_TASK_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "../include/productConfig.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
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
    
    /*状态更新定时触发源启动*/
    void TimerSingalStart(void);

    /*进行所有硬件的处理, 包含硬件配置和状态读取*/
    int RefreshAllDevice(void);

    /*根据寄存器配置更新硬件状态*/
    void WriteDeviceConfig(uint8_t cmd, uint8_t *pRegConfig, int size);

    /*读取硬件状态并更新到寄存器中*/
    bool ReadDeviceStatus(void);

    /*将数据写入内部共享的数据寄存器*/
    void SetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart);

    /*从内部共享数据寄存器中读取数据*/
    uint16_t GetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart);
    
    /*带判断是否修改的写入寄存器实现*/
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
    pthread_t tid;

public:
    ApplicationThread();
    ApplicationThread(CApplicationReg *pReg);
        ~ApplicationThread();

    bool init();
    static ApplicationThread *getInstance();
    CApplicationReg *GetApplicationReg() {return pApplicationReg;}
};
#endif
