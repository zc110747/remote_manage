//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      BaseMessage.hpp
//
//  Purpose:
//      thread communication message interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_BASE_MESSAGE_HPP
#define _INCLUDE_BASE_MESSAGE_HPP

#include "../../include/productConfig.hpp"


#define MAIN_BASE_MESSAGE                   1    
#define APP_BASE_MESSAGE                    2

class CBaseMessageInfo
{
public:
    CBaseMessageInfo(){};
        ~CBaseMessageInfo(){};

    /*通讯队列的创建*/
    virtual int CreateInfomation(void) = 0;     

    /*通讯队列的资源释放*/                            
    virtual int CloseInformation(uint8_t info) = 0;       

    /*等待通讯队列的数据接收*/                    
    virtual int WaitInformation(uint8_t info, char *buf, int bufsize) = 0;  
    
    /*向通讯队列投递数据*/    
    virtual int SendInformation(uint8_t info, char *buf, int bufsize, int prio) = 0;    
};
#endif
