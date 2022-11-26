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
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/26/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_LOGGER_HPP
#define _INCLUDE_LOGGER_HPP

#include "productConfig.hpp"
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "cmdProcess.hpp"
#include "timer.hpp"

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

typedef struct 
{
    int fd;
    std::atomic<bool> islink;
}LOG_SOCKET;

class LoggerManage
{
private:
    char *pNextMemoryBuffer;
    char *pEndMemoryBuffer;

    //logger info
    LOG_LEVEL log_level{LOG_TRACE};
    LOG_SOCKET socket{-1, {false}};

    //thread
    std::atomic<bool> set_thread_work{false};
    bool is_thread_work{false};
    std::thread m_RxThread;
    std::thread m_TxThread;
    std::mutex *pMutex;

    //fd
    LOG_MESSAGE message;
    int readfd{-1};
    int writefd{-1};

    static LoggerManage *pInstance;

private:
    char *getMemoryBuffer(uint16_t size);
    void mutex_lock()       {if(is_thread_work) pMutex->lock();}
    void mutex_unlock()     {if(is_thread_work) pMutex->unlock();}

public:
    LoggerManage() = default;
    ~LoggerManage() = delete;

    bool init();
    void release();

    bool createfifo();
    int print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...);
    static LoggerManage *getInstance();

    void setThreadWork()            {set_thread_work = true;}
    int read_fd()                   {return readfd;}
    void setlevel(LOG_LEVEL level)  {log_level = level;}
    LOG_LEVEL getlevel()            {return log_level;} 
    LOG_SOCKET *getsocket()          {return &socket;}
};

#define PRINT_NOW(...)    { printf(__VA_ARGS__); fflush(stdout);}
#define PRINT_LOG(level, time, fmt, ...) do{ LoggerManage::getInstance()->print_log(level, time, fmt, ##__VA_ARGS__); }while(0);
#endif
