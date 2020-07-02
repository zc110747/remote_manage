/*
 * File      : FifoMessage.cpp
 * 基于Fifo的进程间通讯方案
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

#include "../../include/GroupApp/FifoManage.h"

#if __WORK_IN_WSL == 1
/**************************************************************************
* Local Macro Definition
***************************************************************************/
#define FIFO_MODE           (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  
#define MAIN_FIFO_NAME      "/tmp/main.fifo"
#define APP_FIFO_NAME       "/tmp/app.fifo"
  
/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static CFifoManageInfo FifoMessageInfo;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/**************************************************************************
* Function
***************************************************************************/
/**
 * 创建FIFO命名管道
 * 
 * @param NULL
 *  
 * @return 命名管道的创建结果
 */
int CFifoManageInfo::CreateInfomation(void)
{
    //删除当前已存在的fifo
    unlink(MAIN_FIFO_NAME);
    unlink(APP_FIFO_NAME);

    //重新创建FIFO
    if(mkfifo(MAIN_FIFO_NAME, FIFO_MODE) < 0){
        USR_DEBUG("Create %s Error:%s\n", MAIN_FIFO_NAME, strerror(errno));
        return RT_INVALID_MQ;
    }
    if(mkfifo(APP_FIFO_NAME, FIFO_MODE) < 0){
        USR_DEBUG("Create %s Error:%s\n", APP_FIFO_NAME, strerror(errno));
        return RT_INVALID_MQ;
    }

    USR_DEBUG("start create fifo\n");
    m_rfd_main = open(MAIN_FIFO_NAME, O_RDWR, 0);
    m_rfd_app = open(APP_FIFO_NAME, O_RDWR, 0);
    m_wfd_main = open(MAIN_FIFO_NAME, O_RDWR, 0);
    m_wfd_app = open(APP_FIFO_NAME, O_RDWR, 0);
    if(m_rfd_main < 0 || m_rfd_app < 0 || m_wfd_main<0 || m_wfd_app<0){
        USR_DEBUG("Open Fifo Failed， val:%d, %d, %d, %d\n", m_rfd_main, m_rfd_app,
                m_wfd_main, m_wfd_app);
        close(m_rfd_main);
        close(m_rfd_app);
        close(m_wfd_main);
        close(m_wfd_app);
        m_rfd_main = -1;
        m_rfd_app = -1;
        m_wfd_main = -1;
        m_wfd_app = -1;
        return RT_INVALID_MQ;
    }

    USR_DEBUG("Fifo %s and %s Create Ok\n", MAIN_FIFO_NAME, APP_FIFO_NAME);
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
int CFifoManageInfo::WaitInformation(uint8_t info, char *buf, int bufsize)
{
    int readbytes;
    switch(info)
    {
        case MAIN_FIFO:
            if(m_rfd_main > 0){
                readbytes = read(m_rfd_main, buf, bufsize);
                if(readbytes > 0){
                    //USR_DEBUG("Main Fifo Read, readsize:%d\n", readbytes);
                }
            }
            else
            {
                return RT_INVALID_MQ;
            }
            break;
        case APP_FIFO:
            if(m_rfd_app > 0){
                readbytes = read(m_rfd_app, buf, bufsize);
                if(readbytes > 0){
                    //USR_DEBUG("App Fifo Read:%d\n", readbytes);
                }
            }
            else
            {
                return RT_INVALID_MQ;
            }
            break;
        default:
            return RT_INVALID;
    }
    
    return readbytes;
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
int CFifoManageInfo::SendInformation(uint8_t info, char *buf, int bufsize, int prio)
{
    int writebytes;
    switch(info)
    {
        case MAIN_FIFO:
            if(m_wfd_main > 0){
                writebytes = write(m_wfd_main, buf, bufsize);
                if(writebytes > 0){
                    //USR_DEBUG("Main Fifo write, readsize:%d\n", writebytes);
                }
            }
            else
            {
                return RT_INVALID_MQ;
            }
        break;
        case APP_FIFO:
            if(m_wfd_app > 0){
                writebytes = write(m_wfd_app, buf, bufsize);
                if(writebytes > 0){
                    //USR_DEBUG("App Fifo write:%d\n", writebytes);
                }
            }
            else
            {
                return RT_INVALID_MQ;
            }
        break;
        default:
            return RT_INVALID;
    }
    return RT_OK;
}

/**
 * 关闭消息队列
 * 
 * @param NULL
 *  
 * @return 返回消息队列的关闭状态
 */
int CFifoManageInfo::CloseInformation(uint8_t info)
{
    if(m_rfd_main > 0)
        close(m_rfd_main);

    if(m_rfd_app > 0)
        close(m_rfd_app);

    if(m_wfd_main > 0)
        close(m_wfd_main);

    if(m_wfd_app > 0)
        close(m_wfd_app);

    m_rfd_main = -1;
    m_rfd_app = -1;
    m_wfd_main = -1;
    m_wfd_app = -1;
    return RT_OK;
}


/**
 * 获取线程间通讯接口
 * 
 * @param NULL
 *  
 * @return 返回线程间通讯的信息
 */
CFifoManageInfo *GetFifoMessageInfo(void)
{   
    return &FifoMessageInfo;
}
#endif
