/*!
    指令相关的数据存储管理实现
*/
#include "commandinfo.h"
#include <QString>

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
   0x01, 0x00, 0x40, 0x00, 0x34,
};

static uint8_t *pSCommandListBuffer[CMD_LIST_SIZE] =
{
    led_on_cmd,
    led_off_cmd,
    beep_on_cmd,
    beep_off_cmd,
    dev_reboot_cmd,
    get_info_cmd,
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
        if(nSize > 16)
        {
            pRegInfoList = (struct SRegInfoList *)pRecvData;
            DecodeBuf = QString("Led Status:%1\n").arg(pRegInfoList->s_base_status.b.led);
            DecodeBuf += QString("Beep Status:%1\n").arg(pRegInfoList->s_base_status.b.beep);
            DecodeBuf += QString("Sensor Ia:%1\n").arg(pRegInfoList->sensor_ia);
            DecodeBuf += QString("Sensor Als:%1\n").arg(pRegInfoList->sensor_als);
            DecodeBuf += QString("Sensor Ps:%1\n").arg(pRegInfoList->sensor_ps);
            DecodeBuf += QString("Sensor Gyro x:%1\n").arg(pRegInfoList->sensor_gyro_x);
            DecodeBuf += QString("Sensor Gyro y:%1\n").arg(pRegInfoList->sensor_gyro_y);
            DecodeBuf += QString("Sensor Gyro z:%1\n").arg(pRegInfoList->sensor_gyro_z);
            DecodeBuf += QString("Sensor Accel x:%1\n").arg(pRegInfoList->sensor_accel_x);
            DecodeBuf += QString("Sensor Accel y:%1\n").arg(pRegInfoList->sensor_accel_y);
            DecodeBuf += QString("Sensor Accel z:%1\n").arg(pRegInfoList->sensor_accel_z);
            DecodeBuf += QString("Sensor temp:%1\n").arg(pRegInfoList->sensor_temp);
            DecodeBuf += QString("rtc :%1\n").arg(((uint64_t)pRegInfoList->rtc_high)<<32 | pRegInfoList->rtc_lower);
        }

        return DecodeBuf;
    },
    nullptr,
};

/*!
  指令的信息初始化，包含指令数据，长度，指令名称和回调函数的赋值
*/
void CommandInfoInit(void)
{
    for(int index=0; index<CMD_LIST_SIZE; index++)
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
    if(index < CMD_LIST_SIZE)
        return (SCommandInfo *)&SCommand[index];
    else
        return nullptr;
}
