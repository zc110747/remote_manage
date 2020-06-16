/*
 * File      : InterprocessCom.h
 * InterProcessCom线程通讯接口
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
#ifndef _INCLUDE_INTERPROCESS_COM_H
#define _INCLUDE_INTERPROCESS_COM_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "UsrTypeDef.h"
#include <mqueue.h>

/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define MAIN_MQ       1
#define APP_MQ        2

/**************************************************************************
* Global Type Definition
***************************************************************************/
class CCommunicationInfo
{
public:
    CCommunicationInfo(){};
        ~CCommunicationInfo(){};

    int CreateMqInfomation(void);                                       //创建消息队列
    int CloseMqInformation(void);                                       //释放消息队列
    int WaitMqInformation(uint8_t info, char *buf, int bufsize);        //向消息队列投递消息
    int SendMqInformation(uint8_t info, char *buf, int bufsize, int prio); //发送数据给消息队列

private:
    mqd_t m_MainMqd{-1};
    mqd_t m_AppMqd{-1};
    int isMqOk;
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
CCommunicationInfo *GetCommunicationInfo(void);
#endif
