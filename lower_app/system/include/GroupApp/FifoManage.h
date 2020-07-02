/*
 * File      : FifoManage.h
 * 命名管道管理接口
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
#ifndef _INCLUDE_FIFO_MANAGE_H
#define _INCLUDE_FIFO_MANAGE_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "../UsrTypeDef.h"
#include "BaseMessage.h"
#include <mqueue.h>

#if __WORK_IN_WSL == 1
/**************************************************************************
* Global Macro Definition
***************************************************************************/
#define MAIN_FIFO        MAIN_BASE_MESSAGE
#define APP_FIFO         APP_BASE_MESSAGE

/**************************************************************************
* Global Type Definition
***************************************************************************/
class CFifoManageInfo:public CBaseMessageInfo
{
public:
    CFifoManageInfo(){};
        ~CFifoManageInfo(){};

    /*创建并打开FIFO*/
    int CreateInfomation(void);                        

    /*关闭FIFO并释放资源*/
    int CloseInformation(uint8_t info);         
    
    /*等待FIFO数据接收*/
    int WaitInformation(uint8_t info, char *buf, int bufsize);              

    /*向FIFO中投递数据*/
    int SendInformation(uint8_t info, char *buf, int bufsize, int prio);  

private:

    /*主FIFO读描述符*/
    int m_rfd_main{-1};

    /*主FIFO写描述符*/
    int m_wfd_main{-1};

    /*应用FIFO读描述符*/
    int m_rfd_app{-1};

    /*应用FIFO写描述符*/
    int m_wfd_app{-1};
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
CFifoManageInfo *GetFifoMessageInfo(void);
#endif
#endif
