//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger.hpp
//
//  Purpose:
//      logger interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/26/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_LOGGER_HPP
#define _INCLUDE_LOGGER_HPP

#include "../include/productConfig.hpp"


#define LOGGER_MAX_BUFFER_SIZE      256
#define LOGGER_MESSAGE_BUFFER_SIZE  16384
#define LOGGER_FIFO_PATH            "/tmp/logger.fifo"

typedef enum
{
	LOG_TRACE = 0,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
}LOG_LEVEL;

typedef struct 
{
    char *ptr;
    int length;
}LOG_MESSAGE;

class LoggerManage
{
private:
    LOG_LEVEL log_level;

    char *pNextMemoryBuffer;
    char *pEndMemoryBuffer;
    
    //thread
    bool set_thread_work;
    bool is_thread_work;
    pthread_mutex_t mutex;
    pthread_t tid;

    //fd
    LOG_MESSAGE message;
    int readfd;
    int writefd;

    static LoggerManage *pInstance;

private:
    char *getMemoryBuffer(uint16_t size);
    void mutex_lock()       {if(is_thread_work) pthread_mutex_lock(&mutex);}
    void mutex_unlock()     {if(is_thread_work) pthread_mutex_unlock(&mutex);}

public:
    LoggerManage();
        ~LoggerManage();

    bool init();
    bool createfifo();
    int print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...);
    static LoggerManage *getInstance();

    void setThreadWork()            {set_thread_work = true;}
    int read_fd()                   {return readfd;}
    void setlevel(LOG_LEVEL level)  {log_level = level;}
    LOG_LEVEL getlevel()            {return log_level;}
};

#define PRINT_LOG(level, time, fmt, ...) do{ LoggerManage::getInstance()->print_log(level, time, fmt, ##__VA_ARGS__); }while(0);
#endif
