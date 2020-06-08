#ifndef COMMANDINFO_H
#define COMMANDINFO_H

#include "typedef.h"
#include <functional>

#define CMD_LIST_SIZE           7

#define LED_ON_CMD              0x00
#define LED_OFF_CMD             0x01
#define BEEP_ON_CMD             0x02
#define BEEP_OFF_CMD            0x03
#define DEV_REBOOT_CMD          0x04
#define GET_INFO_CMD            0x05
#define ABORT_CMD               0x06

#define DEV_WRITE_THROUGH_CMD   0xFF

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

struct SCommandInfo
{
   uint8_t *m_pbuffer;
   uint16_t m_nSize;
   uint8_t m_nCommand;
   std::function<QString(uint8_t *, int)>  m_pFunc;
};

void CommandInfoInit(void);
SCommandInfo *GetCommandPtr(uint16_t index);
#endif // COMMANDINFO_H
