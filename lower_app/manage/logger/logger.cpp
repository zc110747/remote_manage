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
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/26/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "logger.hpp"
#include <stdarg.h>
#include "../driver/driver.hpp"

char memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE+1];

static void *loggerThread(void *arg)
{
    int len;
    LOG_MESSAGE message;
    LoggerManage *plogger = static_cast<LoggerManage *>(arg);

    plogger->setThreadWork();
    while(1)
    {
        len = ::read(plogger->read_fd(), &message, sizeof(message));
        if(len > 0)
        {
            len = write(STDOUT_FILENO, message.ptr, message.length);
            if(len < 0)
            {
                //do something
            }
            fflush(stdout);
        }
    }

    pthread_detach(pthread_self()); 
    pthread_exit((void *)0);
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

LoggerManage::LoggerManage()
{
    readfd = -1;
    writefd = -1;
    log_level = LOG_TRACE;
    set_thread_work = false;
    is_thread_work = false;
    pNextMemoryBuffer = memoryBuffer;
    pEndMemoryBuffer = &memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE];
}

LoggerManage::~LoggerManage()
{
    is_thread_work = false;
    pthread_mutex_destroy(&mutex); 

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

bool LoggerManage::createfifo()
{
    unlink(LOGGER_FIFO_PATH);

    if(mkfifo(LOGGER_FIFO_PATH, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0){
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Logger Fifo Create error!");
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

bool LoggerManage::init()
{
    bool ret = true;
    int nErr;

    pthread_mutex_init(&mutex, NULL);
    createfifo();

    nErr = pthread_create(&tid, NULL, loggerThread, this);
    if(nErr != 0)
    {
        ret = false;
    }
    return ret;
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
        return 0;
    
    message.length += len;
    pbuf = &pbuf[len];
    bufferlen -= len;

    if(bufferlen < 2)
        return 0;
    
    pbuf[0] = '\n';
    pbuf[1] = 0;
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
        if(len<0)
        {
            //do something error
        }
    }
    return  message.length;
}

