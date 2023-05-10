//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger.cpp
//
//  Purpose:
//      logger application
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/26/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <stdarg.h>
#include "logger.hpp"
#include "../driver/driver.hpp"
#include "asio_server.hpp"

static asio_server logger_server;

//asio server test ok
void LoggerManage::asio_server_run()
{
    const SocketSysConfig *pSocketConfig = SystemConfig::getInstance()->getlogger();
    
    try
    {
        logger_server.init(pSocketConfig->ipaddr, std::to_string(pSocketConfig->port), [](char* ptr, int length){
            if(cmdProcess::getInstance()->parseData(ptr, length))
            {
                cmdProcess::getInstance()->ProcessData();
            }
        });
        logger_server.run();
    }
    catch (std::exception& e)
    {
        PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Exception:%s", e.what());
    }
}

void LoggerManage::logger_tx_run()
{
    int len;
    LOG_MESSAGE message;
    
    setThreadWork();

    while(1)
    {
        len = ::read(read_fd(), &message, sizeof(message));
        if(len > 0)
        {
            auto session_ptr = logger_server.get_valid_session();
            if(session_ptr != nullptr)
            {
                session_ptr->do_write(message.ptr, message.length);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                len = ::write(STDOUT_FILENO, message.ptr, message.length);
                fflush(stdout);
                if(len < 0)
                {
                    //do something
                }
            }
        }
        else
        {
            PRINT_NOW("%s read failed:%d\n", __func__, len);
        }
    }
}

LoggerManage* LoggerManage::pInstance = nullptr;
LoggerManage* LoggerManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) LoggerManage;
        if(pInstance == nullptr)
        {
            //do something
        }
    }
    return pInstance;
}

bool LoggerManage::createfifo()
{
    unlink(LOGGER_FIFO_PATH);

    if(mkfifo(LOGGER_FIFO_PATH, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0){
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Logger Fifo Create error!");
        return false;
    }

    readfd = open(LOGGER_FIFO_PATH, O_RDWR, 0);
    if(readfd == -1)
    {
        return false;
    }

    writefd = open(LOGGER_FIFO_PATH, O_RDWR, 0);
    if(writefd == -1)
    {
        close(readfd);
        return false;
    }
    return true;
}

char memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE+1];
bool LoggerManage::init()
{
    bool ret = true;
    int nErr;

    pNextMemoryBuffer = memoryBuffer;
    pEndMemoryBuffer = &memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE];

    createfifo();

    m_TxThread = std::thread(std::bind(&LoggerManage::logger_tx_run, this));
    m_TxThread.detach();
    m_AsioServerThread = std::thread(std::bind(&LoggerManage::asio_server_run, this));
    m_AsioServerThread.detach();
    return ret;
}

void LoggerManage::release()
{
    is_thread_work = false;
    
    if(readfd != - 1)
    {
        close(readfd);
        readfd = -1;
    }

    if(writefd == -1)
    {
        close(writefd);   
        writefd = -1;
    }
}

char *LoggerManage::getMemoryBuffer(uint16_t size)
{
    char *pCurrentMemBuffer;

    pCurrentMemBuffer = pNextMemoryBuffer;
    pNextMemoryBuffer = pCurrentMemBuffer+size;
	if(pNextMemoryBuffer >  pEndMemoryBuffer)
	{
		pCurrentMemBuffer = memoryBuffer;
		pNextMemoryBuffer = pCurrentMemBuffer + size;
	}
	return(pCurrentMemBuffer);
}

int LoggerManage::print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...)
{
    int len, bufferlen;
    char *pbuf, *pstart;

    if(level < log_level)
        return 0;

    is_thread_work = set_thread_work;

    mutex_lock();
    pstart = getMemoryBuffer(LOGGER_MAX_BUFFER_SIZE);
    len = LOGGER_MAX_BUFFER_SIZE;
    bufferlen = len - 1;
    pbuf = pstart;
    message.length = 0;
    message.ptr = pstart;

    len = snprintf(pbuf, bufferlen, "LogLevel:%d time:%d info:",level, time);
    if((len<=0) || (len>=bufferlen))
    {
        PRINT_NOW("%s label0 error\n", __func__);
        mutex_unlock();
        return 0;
    }

    message.length += len;
    pbuf = &pbuf[len];
    bufferlen -= len;
    
    va_list	valist;
    va_start(valist, fmt);
	len = vsnprintf(pbuf, bufferlen, fmt, valist);
	va_end(valist);
    mutex_unlock();

    if((len<=0) || (len>=bufferlen))
    {
        PRINT_NOW("%s label1 error\n", __func__);
        return 0;
    }
    
    message.length += len;
    pbuf = &pbuf[len];
    bufferlen -= len;

    if(bufferlen < 3)
    {
        PRINT_NOW("%s label2 error\n", __func__);
        return 0;
    }
    
    pbuf[0] = '\r';
    pbuf[1] = '\n';
    message.length += 2;

    if(!is_thread_work)
    {
        len = write(STDOUT_FILENO, message.ptr, message.length);
        if(len<0)
        {
            //do something error
        }
        fflush(stdout);
    }
    else
    {
        len = ::write(writefd, &message, sizeof(message));
        if(len<=0)
        {
            PRINT_NOW("%s label3 error\n", __func__);
        }
        else
        {
            //PRINT_NOW("%s writefd\n", __func__);
        }
    }
    
    return  message.length;
}

