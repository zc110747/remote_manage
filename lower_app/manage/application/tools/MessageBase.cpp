//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      BaseMessage.cpp
//
//  Purpose:
//      Message Base application defined.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "MessageBase.hpp"
#include "FIFOManage.hpp"
#include "MqManage.hpp"

MessageBase *getMessageInfo(uint8_t index)
{
    MessageBase *pMessage = nullptr;

#if MESSAGE_USE_INTERFACE == MESSAGE_USE_FIFO
    pMessage = FIFOManage::getInstance()->getFIFOMessage(index);
#elif MESSAGE_USE_INTERFACE == MESSAGE_USE_MESSAGEQ
    //do something
#endif

    return pMessage;
}