/*
 * File      : InterprocessCom.cpp
 * InterProcessCom线程通讯实现
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

/**************************************************************************
* Local Macro Definition
***************************************************************************/
#include "../include/Communication.h"

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static CCommunicationInfo CommunicationInfo;

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
/**
 * 创建POSIX消息队列
 * 
 * @param NULL
 *  
 * @return 消息队列创建的结果
 */
int CCommunicationInfo::CreateMqInfomation(void)
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
int CCommunicationInfo::WaitMqInformation(uint8_t info, char *buf, int bufsize)
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
                mq_getattr(m_MainMqd, &attr);
                nReadSize = mq_receive(m_MainMqd, buf, bufsize, &prio);
                //USR_DEBUG("Wait for Mq Receive, Mqd:%d, %d, %s, %d\n", m_MainMqd, nReadSize, strerror(errno), (int)attr.mq_msgsize);
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
                mq_getattr(m_AppMqd, &attr);
                nReadSize = mq_receive(m_AppMqd, buf, bufsize, &prio);
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
int CCommunicationInfo::SendMqInformation(uint8_t info, char *buf, int bufsize, int prio)
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
int CCommunicationInfo::CloseMqInformation(uint8_t info)
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

/**
 * 获取线程间通讯接口
 * 
 * @param NULL
 *  
 * @return 返回线程间通讯的信息
 */
CCommunicationInfo *GetCommunicationInfo(void)
{
    return &CommunicationInfo;
}
