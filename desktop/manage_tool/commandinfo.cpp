//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      appthread.cpp
//
//  Purpose:
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      11/20/2023   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <QString>
#include "commandinfo.h"

static SCommandInfo SCommand[CMD_LIST_SIZE];

//指令格式
//cmd(1Byte) 0x01 读内部状态 0x02 写内部状态 0x03 上传指令 0x04 上传数据
//reg(2Byte)
//size(2Byte)
//reg_value(size byte) -- 读内部状态时无寄存器值
static uint8_t led_on_cmd[] = {
    0x02, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x01
};
static uint8_t led_off_cmd[] = {
    0x02, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00
};
static uint8_t beep_on_cmd[] = {
    0x02, 0x00, 0x00, 0x00, 0x03, 0x05, 0x00, 0x02
};
static uint8_t beep_off_cmd[] = {
    0x02, 0x00, 0x00, 0x00, 0x03, 0x05, 0x00, 0x00
};
static uint8_t dev_reboot_cmd[] = {
    0x02, 0x00, 0x00, 0x00, 0x01, 0x09
};

static uint8_t get_info_cmd[] = {
   0x01, 0x00, 0x40, 0x00, 0x36,
};

static uint8_t *pSCommandListBuffer[CMD_LIST_SIZE] =
{
    led_on_cmd,
    led_off_cmd,
    beep_on_cmd,
    beep_off_cmd,
    dev_reboot_cmd,
    get_info_cmd,
    nullptr,
    nullptr
};

static uint16_t nSCommandListSize[CMD_LIST_SIZE] =
{
    sizeof(led_on_cmd),
    sizeof(led_off_cmd),
    sizeof(beep_on_cmd),
    sizeof(beep_off_cmd),
    sizeof(dev_reboot_cmd),
    sizeof(get_info_cmd),
    0,
    0
};

static std::function<QString(uint8_t *, int)> FuncList[CMD_LIST_SIZE] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    //get info cmd执行的回调函数
    [](uint8_t *pRecvData, int nSize)->QString{
        QString DecodeBuf = "";
        struct SRegInfoList *pRegInfoList;

        //qDebug()<<nSize;
        if (nSize > 16)
        {
            pRegInfoList = (struct SRegInfoList *)pRecvData;
            DecodeBuf = QString::fromLocal8Bit("LED显示:%1\n").arg(pRegInfoList->s_base_status.b.led==0?"OFF":"ON"); //LED状态
            DecodeBuf += QString::fromLocal8Bit("蜂鸣器状态:%1\n").arg(pRegInfoList->s_base_status.b.beep==0?"OFF":"ON"); //蜂鸣器状态
            DecodeBuf += QString::fromLocal8Bit("环境光强度:%1\n").arg(pRegInfoList->sensor_ia);
            DecodeBuf += QString::fromLocal8Bit("接近距离:%1\n").arg(pRegInfoList->sensor_als);
            DecodeBuf += QString::fromLocal8Bit("红外线强度:%1\n").arg(pRegInfoList->sensor_ps);
            DecodeBuf += QString::fromLocal8Bit("陀螺仪x方向:%1 °/s\n").arg((float)pRegInfoList->sensor_gyro_x/16.4);
            DecodeBuf += QString::fromLocal8Bit("陀螺仪y方向:%1 °/s\n").arg((float)pRegInfoList->sensor_gyro_y/16.4);
            DecodeBuf += QString::fromLocal8Bit("陀螺仪z方向:%1 °/s\n").arg((float)pRegInfoList->sensor_gyro_z/16.4);
            DecodeBuf += QString::fromLocal8Bit("加速度x方向:%1 fg\n").arg((float)pRegInfoList->sensor_accel_x/2048);
            DecodeBuf += QString::fromLocal8Bit("加速度y方向:%1 fg\n").arg((float)pRegInfoList->sensor_accel_y/2048);
            DecodeBuf += QString::fromLocal8Bit("加速度z方向:%1 fg\n").arg((float)pRegInfoList->sensor_accel_z/2048);
            DecodeBuf += QString::fromLocal8Bit("温度:%1°C\n").arg((float)(pRegInfoList->sensor_temp-25)/326.8 + 25);
            DecodeBuf += QString::fromLocal8Bit("RTC定时器时钟:%1:%2:%3\n")
                        .arg(pRegInfoList->rtc_hour, 2, 10, QLatin1Char('0'))
                        .arg(pRegInfoList->rtc_minute, 2, 10, QLatin1Char('0'))
                        .arg(pRegInfoList->rtc_sec, 2, 10, QLatin1Char('0'));
        }

        return DecodeBuf;
    },
    nullptr,
    nullptr,
};

/*!
  指令的信息初始化，包含指令数据，长度，指令名称和回调函数的赋值
*/
void CommandInfoInit(void)
{
    for (int index=0; index<CMD_LIST_SIZE; index++)
    {
        SCommand[index].m_pbuffer = pSCommandListBuffer[index];
        SCommand[index].m_nSize = nSCommandListSize[index];
        SCommand[index].m_nCommand = index;
        SCommand[index].m_pFunc = FuncList[index];
    }
}

/*!
  获取指令的信息结构体，为后续数据发送和读取执行提供数据
*/
SCommandInfo *GetCommandPtr(uint16_t index)
{
    if (index < CMD_LIST_SIZE)
        return (SCommandInfo *)&SCommand[index];
    else
        return nullptr;
}
