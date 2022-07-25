//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      MqManage.hpp
//
//  Purpose:
//      Communication by message queue interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_MQ_MANAGE_H
#define _INCLUDE_MQ_MANAGE_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "BaseMessage.hpp"
#include <mqueue.h>

#if __WORK_IN_WSL == 0
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

    /*创建并打开消息队列*/
    int CreateInfomation(void);                                     

    /*释放消息队列*/
    int CloseInformation(uint8_t info);                            

    /*向消息队列投递消息*/
    int WaitInformation(uint8_t info, char *buf, int bufsize);       

    /*发送数据给消息队列*/
    int SendInformation(uint8_t info, char *buf, int bufsize, int prio); 

private:
    
    /*主消息队列描述符*/
    mqd_t m_MainMqd{-1};

    /*应用消息队列描述符*/
    mqd_t m_AppMqd{-1};
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/

/*获取线程间通讯信息*/
CMqMessageInfo *GetMqMessageInfo(void);
#endif
#endif
