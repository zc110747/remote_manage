#ifndef COMMANDINFO_H
#define COMMANDINFO_H

#include "typedef.h"
#include <functional>

#define CMD_LIST_SIZE           6

#define LED_ON_CMD              0
#define LED_OFF_CMD             1
#define BEEP_ON_CMD             2
#define BEEP_OFF_CMD            3
#define DEV_REBOOT_CMD          4
#define GET_INFO_CMD            5

#define DEV_WRITE_THROUGH_CMD   0xFF
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
