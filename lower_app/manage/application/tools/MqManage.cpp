//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      MqManage.cpp
//
//  Purpose:
//      Communication by message queue.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "MqManage.hpp"
#include <memory>

#if __WORK_IN_WSL == 0

static CMqMessageInfo MqMessageInfo;

int CMqMessageInfo::CreateInfomation(void)
{
    struct mq_attr attr;

    //删除当前已经存在的队列, 否则设置的配置无效
    mq_unlink("/MainMq");
    mq_unlink("/AppMq");

    attr.mq_maxmsg=4;
    attr.mq_msgsize=128;
    m_MainMqd = mq_open("/MainMq", O_RDWR | O_CREAT, 0666, &attr);
    if(m_MainMqd < 0)
    {
        USR_DEBUG("MAIN MQ Create Failed, error:%s\n", strerror(errno));
        return RT_INVALID_MQ;
    }

    attr.mq_maxmsg=20;
    m_AppMqd = mq_open("/AppMq", O_RDWR | O_CREAT, 0666, &attr);
    if(m_AppMqd < 0)
    {
        mq_close(m_MainMqd); //失败需要将上一个关闭
        m_MainMqd = -1;
        USR_DEBUG("APP MQ Send Failed, error:%s\n", strerror(errno));
        return RT_INVALID_MQ;
    }

    USR_DEBUG("Mq Create Ok\n");
    return RT_OK;
}

int CMqMessageInfo::WaitInformation(uint8_t info, char *buf, int bufsize)
{
    int nReadSize = -1;
    struct mq_attr attr;
    struct mq_attr attr_old;
    uint32_t prio;
    assert(buf != nullptr);

    switch(info)
    {
        case MAIN_MQ:
            if(m_MainMqd > 0)
            {
                mq_getattr(m_MainMqd, &attr);
                if(attr.mq_maxmsg > 0)
                {
                    std::unique_ptr<char[]> up_main(new char[attr.mq_msgsize]);
                    nReadSize = mq_receive(m_MainMqd, up_main.get(), attr.mq_msgsize, &prio);
                    memcpy(buf, up_main.get(), (bufsize>attr.mq_maxmsg?attr.mq_maxmsg:bufsize));
                    //USR_DEBUG("Wait for Mq Receive, Mqd:%d, %d, %s, %d\n", m_MainMqd, nReadSize, strerror(errno), (int)attr.mq_msgsize);
                }
                else
                {
                    return RT_INVALID_MQ_SIZE;
                }
            }
            else
            {
                USR_DEBUG("Invalid Main Mq\n");
                return RT_INVALID_MQ;
            }    
            break;
        case APP_MQ:
            if(m_AppMqd > 0)
            {
                mq_getattr(m_AppMqd, &attr);
                if(attr.mq_maxmsg > 0)
                {
                    std::unique_ptr<char[]> up_app(new char[attr.mq_msgsize]);
                    nReadSize = mq_receive(m_AppMqd, up_app.get(), attr.mq_msgsize, &prio);
                    memcpy(buf, up_app.get(), (bufsize>attr.mq_msgsize?attr.mq_msgsize:bufsize));
                    //USR_DEBUG("Wait for Mq Receive, Mqd:%d, %d, %s, %d\n", m_MainMqd, nReadSize, strerror(errno), (int)attr.mq_msgsize);
                }
                else
                {
                    return RT_INVALID_MQ_SIZE;
                }
                
                //USR_DEBUG("Wait for Mq Receive, Mqd:%d, %d, %s, %d\n", m_AppMqd, nReadSize, strerror(errno), (int)attr.mq_msgsize);
            }
            else
            {
                USR_DEBUG("Invalid Mq\n");
                return RT_INVALID_MQ;
            }
            break;
        default:
            USR_DEBUG("Invalid Receive MQ Info\n");
            break;
    }
    return nReadSize;
}

int CMqMessageInfo::SendInformation(uint8_t info, char *buf, int bufsize, int prio)
{
    int nWriteSize = -1;

    assert(buf != nullptr);
    
    switch (info)
    {
        case MAIN_MQ:
            if(m_MainMqd > 0)
            {
                nWriteSize = mq_send(m_MainMqd, buf, bufsize, prio);
            }
            else
            {
                return RT_INVALID_MQ;
            }
            break;
        case APP_MQ:
            if(m_AppMqd > 0)
            {
                nWriteSize = mq_send(m_AppMqd, buf, bufsize, prio);
            }
            else
            {
                return RT_INVALID_MQ;
            }
            break;
        default:
            USR_DEBUG("Invalid Send MQ Info\n");
            break;
    }
    return nWriteSize;
}

int CMqMessageInfo::CloseInformation(uint8_t info)
{
    switch(info)
    {
        case MAIN_MQ:
            if(m_MainMqd > 0)
            {
                close(m_MainMqd);
                m_MainMqd = -1;
            }
            break;
        case APP_MQ:
            if(m_AppMqd > 0)
            {
                close(m_AppMqd);
                m_AppMqd = -1;
            }
            break;
        default:
            USR_DEBUG("Invalid Mq Close Info\n");
            break;
    }

    return RT_OK;
}

CMqMessageInfo *GetMqMessageInfo(void)
{
    return &MqMessageInfo;
}
#endif
