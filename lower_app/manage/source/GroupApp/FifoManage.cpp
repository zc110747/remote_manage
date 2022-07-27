//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      FifoManage.cpp
//
//  Purpose:
//      Communication by fifo.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "FifoManage.hpp"

#if __WORK_IN_WSL == 1
#define FIFO_MODE           (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  
#define MAIN_FIFO_NAME      "/tmp/main.fifo"
#define APP_FIFO_NAME       "/tmp/app.fifo"
  
static CFifoManageInfo FifoMessageInfo;

int CFifoManageInfo::CreateInfomation(void)
{
    //删除当前已存在的fifo
    unlink(MAIN_FIFO_NAME);
    unlink(APP_FIFO_NAME);

    //重新创建FIFO
    if(mkfifo(MAIN_FIFO_NAME, FIFO_MODE) < 0){
        return RT_INVALID_MQ;
    }
    if(mkfifo(APP_FIFO_NAME, FIFO_MODE) < 0){
        return RT_INVALID_MQ;
    }

    m_rfd_main = open(MAIN_FIFO_NAME, O_RDWR, 0);
    m_rfd_app = open(APP_FIFO_NAME, O_RDWR, 0);
    m_wfd_main = open(MAIN_FIFO_NAME, O_RDWR, 0);
    m_wfd_app = open(APP_FIFO_NAME, O_RDWR, 0);
    if(m_rfd_main < 0 || m_rfd_app < 0 || m_wfd_main<0 || m_wfd_app<0){
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

    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "System Fifo %s and %s Create Ok", MAIN_FIFO_NAME, APP_FIFO_NAME);
    return RT_OK;
}

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

CFifoManageInfo *GetFifoMessageInfo(void)
{   
    return &FifoMessageInfo;
}
#endif
