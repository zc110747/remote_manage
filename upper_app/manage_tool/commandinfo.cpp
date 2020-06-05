
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
};

static uint16_t nSCommandListSize[CMD_LIST_SIZE] =
{
    sizeof(led_on_cmd),
    sizeof(led_off_cmd),
    sizeof(beep_on_cmd),
    sizeof(beep_off_cmd),
    sizeof(dev_reboot_cmd),
    sizeof(get_info_cmd),
};

static std::function<QString(uint8_t *, int)> FuncList[CMD_LIST_SIZE] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    //get info cmd执行的回调函数
    [](uint8_t *pData, int nSize)->QString{
            QString DecodeBuf = QString("func test, %1, %2\n").arg(nSize, *pData);
            qDebug()<<DecodeBuf;
            return DecodeBuf;
    },
};

//指令序列初始化
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

//获取指令指针
SCommandInfo *GetCommandPtr(uint16_t index)
{
    if(index < CMD_LIST_SIZE)
        return (SCommandInfo *)&SCommand[index];
    else
        return nullptr;
}
