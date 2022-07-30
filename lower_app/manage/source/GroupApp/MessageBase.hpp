//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      BaseMessage.hpp
//
//  Purpose:
//      Base Message Interface defined.
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

#include "../../logger/logger.hpp"
#include "../../driver/driver.hpp"

#define SYSTEM_MESS_INDEX       0
#define APPLICATION_MESS_INDEX  1

class MessageBase
{
public:
    /*通讯队列的创建*/
    virtual bool Create() = 0;     

    /*通讯队列的资源释放*/                            
    virtual void Release() = 0;       

    /*等待通讯队列的数据接收*/                    
    virtual int read(char *buf, int bufsize) = 0;  
    
    /*向通讯队列投递数据*/    
    virtual int write(char *buf, int bufsize) = 0;    
};

MessageBase *getMessageInfo(uint8_t index);
#endif
