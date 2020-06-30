/*
 * File      : MqManage.h
 * Posix消息队列处理接口
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
#ifndef _INCLUDE_MQ_MANAGE_H
#define _INCLUDE_MQ_MANAGE_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "../UsrTypeDef.h"
#include "BaseMessage.h"
#include <mqueue.h>

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define MAIN_MQ       MAIN_BASE_MESSAGE
#define APP_MQ        APP_BASE_MESSAGE

/**************************************************************************
* Global Type Definition
***************************************************************************/
class CMqMessageInfo:public CBaseMessageInfo
{
public:
    CMqMessageInfo(){};
        ~CMqMessageInfo(){};

    #if __WORK_IN_WSL == 0
    int CreateInfomation(void);                                       //创建消息队列
    int CloseInformation(uint8_t info);                                       //释放消息队列
    int WaitInformation(uint8_t info, char *buf, int bufsize);        //向消息队列投递消息
    int SendInformation(uint8_t info, char *buf, int bufsize, int prio); //发送数据给消息队列
    #else
    int CreateInfomation(void){};                                       //创建消息队列
    int CloseInformation(uint8_t info){};                                       //释放消息队列
    int WaitInformation(uint8_t info, char *buf, int bufsize){};        //向消息队列投递消息
    int SendInformation(uint8_t info, char *buf, int bufsize, int prio){}; //发送数据给消息队列
    #endif
private:
    mqd_t m_MainMqd{-1};
    mqd_t m_AppMqd{-1};
    int isMainMqOk;
    int isAppMqOk;
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
CMqMessageInfo *GetMqMessageInfo(void);
#endif
