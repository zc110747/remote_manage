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
#include <pthread.h>
#include <stdlib.h>
#include "UsrTypeDef.h"

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

/**************************************************************************
* Global Type Definition
***************************************************************************/
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
    union UBaseStatus s_base_status; //基础状态信息
    uint16_t sensor_ia;            
    uint16_t sensor_als;
    uint16_t sensor_ps;
    uint16_t reserved0;
    uint32_t sensor_gyro_x;
    uint32_t sensor_gyro_y; 
    uint32_t sensor_gyro_z;
    uint32_t sensor_accel_x;
    uint32_t sensor_accel_y; 
    uint32_t sensor_accel_z;
    uint32_t sensor_temp;
    uint32_t rtc_lower;
    uint32_t rtc_high;
};
#pragma pack(pop)

class CApplicationReg
{
public:
    CApplicationReg(void);
        ~CApplicationReg();
    
    int RefreshAllDevice(void);
    void WriteHardware(uint8_t cmd, uint8_t *pRegConfig, int size);
    void UpdateHardware(void);
    void SetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart);
    uint16_t GetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart);
    int DiffSetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart, uint8_t *pDataCompare);
private:
    uint8_t m_RegVal[REG_NUM];
    pthread_mutex_t m_RegMutex; /*数据读取都要执行该锁*/
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
void ApplicationThreadInit(void);
CApplicationReg *GetApplicationReg(void);
void SetApplicationReg(CApplicationReg *pAppReg);
#endif
