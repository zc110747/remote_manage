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
#include "stdarg.h"

char memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE+1];

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
    log_level = LOG_NONE;
    is_thread_work = false;
    pNextMemoryBuffer = memoryBuffer;
    pEndMemoryBuffer = &memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE];
}

LoggerManage::~LoggerManage()
{
    is_thread_work = false;
    pthread_mutex_destroy(&mutex); 
    close(fd[0]);
    close(fd[1]);   
}

static void *loggerThread(void *arg)
{
    int len;
    char buffer[LOGGER_MAX_BUFFER_SIZE];
    LoggerManage *plogger = static_cast<LoggerManage *>(arg);

    plogger->setThreadWork();
    fflush(stdout);

    while(1)
    {
        len = ::read(plogger->read_fd(), buffer, LOGGER_MAX_BUFFER_SIZE);
        if(len > 0)
        {
            printf("loggerThread start:%d\n", len);
            // buffer[len] = '\0';
            // printf("%s", buffer);
            fflush(stdout);
        }
    }

    pthread_detach(pthread_self()); 
    pthread_exit((void *)0);
}

bool LoggerManage::init()
{
    bool ret = true;
    int nErr;

    pthread_mutex_init(&mutex, NULL);
    
    if(pipe(fd) == -1)
    {
        ret = false;
    }

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
    
    mutex_lock();
    pstart = getMemoryBuffer(LOGGER_MAX_BUFFER_SIZE);
    len = LOGGER_MAX_BUFFER_SIZE;
    bufferlen = len - 1;
    pbuf = pstart;

    len = snprintf(pbuf, bufferlen, "LogLevel:%d time:%d info:",level, time);
    if((len<=0) || (len>=bufferlen))
    {
        mutex_unlock();
        return 0;
    }
    pbuf[len] = 0;

    pbuf = &pbuf[len];
    bufferlen -= len;
    
    va_list	valist;
    va_start(valist, fmt);
	len = vsnprintf(pbuf, bufferlen, fmt, valist);
	va_end(valist);
    mutex_unlock();

    if((len<=0) || (len>=bufferlen))
        return 0;

    pbuf = &pbuf[len];
    bufferlen -= len;

    if(bufferlen < 2)
        return 0;
    
    pbuf[0] = '\n';
    pbuf[1] = 0;

    if(!is_thread_work)
    {
        printf("%s", pstart);
        fflush(stdout);
    }
    else
    {
        ssize_t len = ::write(write_fd(), pstart, strlen(pbuf));
        if(len<0)
        {
            //do something
        }
    }
    return (pbuf-pstart);
}

