/*
 * File      : MqManage.cpp
 * 使用Posix MessageQueue的进程间通讯方案
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

#include "../../include/GroupApp/MqManage.h"

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static CMqMessageInfo MqMessageInfo;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/**************************************************************************
* Local Function
***************************************************************************/

/**************************************************************************
* Function
***************************************************************************/
#if __WORK_IN_WSL == 0
/**
 * 创建POSIX消息队列
 * 
 * @param NULL
 *  
 * @return 消息队列创建的结果
 */
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
        isMainMqOk = false;
        return RT_INVALID_MQ;
    }

    attr.mq_maxmsg=128;
    m_AppMqd = mq_open("/AppMq", O_RDWR | O_CREAT, 0666, &attr);
    if(m_AppMqd < 0)
    {
        mq_close(m_MainMqd); //失败需要将上一个关闭
        m_MainMqd = -1;
        USR_DEBUG("APP MQ Send Failed, error:%s\n", strerror(errno));
        isAppMqOk = false;
        return RT_INVALID_MQ;
    }

    USR_DEBUG("Mq Create Ok\n");
    isMainMqOk = true;
    isAppMqOk = true;
    return RT_OK;
}

/**
 * 等待消息队列的信息返回
 * 
 * @param info 选择发送的消息队列
 * @param buf 发送的消息队列的首地址
 * @param bufsize 发送的消息队列长度
 * @param prio 发送的消息队列优先级
 * 
 * @return 返回消息队列的关闭状态
 */
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
            if(isMainMqOk)
            {
                char *ptr = nullptr;
                mq_getattr(m_MainMqd, &attr);
                if(attr.mq_maxmsg > 0)
                {
                    ptr = new char[attr.mq_maxmsg];
                    nReadSize = mq_receive(m_MainMqd, ptr, attr.mq_maxmsg, &prio);
                    memcpy(buf, ptr, (bufsize>attr.mq_maxmsg?attr.mq_maxmsg:bufsize));
                    delete ptr;
                    ptr = nullptr;
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
            if(isAppMqOk)
            {
                char *ptr = nullptr;
                mq_getattr(m_AppMqd, &attr);
                if(attr.mq_maxmsg > 0)
                {
                    ptr = new char[attr.mq_maxmsg];
                    nReadSize = mq_receive(m_AppMqd, buf, bufsize, &prio);
                    memcpy(buf, ptr, (bufsize>attr.mq_maxmsg?attr.mq_maxmsg:bufsize));
                    delete ptr;
                    ptr = nullptr;
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

/**
 * 发送消息队列
 * 
 * @param info 选择发送的消息队列
 * @param buf 发送的消息队列的首地址
 * @param bufsize 发送的消息队列长度
 * @param prio 发送的消息队列优先级
 * 
 * @return 返回消息队列的关闭状态
 */
int CMqMessageInfo::SendInformation(uint8_t info, char *buf, int bufsize, int prio)
{
    int nWriteSize = -1;

    assert(buf != nullptr);
    
    switch (info)
    {
        case MAIN_MQ:
            if(isMainMqOk)
            {
                nWriteSize = mq_send(m_MainMqd, buf, bufsize, prio);
            }
            else
            {
                return RT_INVALID_MQ;
            }
            break;
        case APP_MQ:
            if(isAppMqOk)
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

/**
 * 关闭消息队列
 * 
 * @param NULL
 *  
 * @return 返回消息队列的关闭状态
 */
int CMqMessageInfo::CloseInformation(uint8_t info)
{
    switch(info)
    {
        case MAIN_MQ:
            if(isMainMqOk)
            {
                close(m_MainMqd);
                m_MainMqd = -1;
            }
            break;
        case APP_MQ:
            if(isAppMqOk)
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
#endif

/**
 * 获取线程间通讯接口
 * 
 * @param NULL
 *  
 * @return 返回线程间通讯的信息
 */
CMqMessageInfo *GetMqMessageInfo(void)
{
    return &MqMessageInfo;
}
