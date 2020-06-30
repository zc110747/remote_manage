/*
 * File      : BaseMessage.h
 * 基础消息队列说明
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
#ifndef _INCLUDE_BASE_MESSAGE_H
#define _INCLUDE_BASE_MESSAGE_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "../UsrTypeDef.h"

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define MAIN_BASE_MESSAGE                   1    
#define APP_BASE_MESSAGE                    2

/**************************************************************************
* Global Type Definition
***************************************************************************/
class CBaseMessageInfo
{
public:
    CBaseMessageInfo(){};
        ~CBaseMessageInfo(){};

    virtual int CreateInfomation(void) = 0;                                 
    virtual int CloseInformation(uint8_t info) = 0;                           
    virtual int WaitInformation(uint8_t info, char *buf, int bufsize) = 0;    
    virtual int SendInformation(uint8_t info, char *buf, int bufsize, int prio) = 0;    
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
#endif
