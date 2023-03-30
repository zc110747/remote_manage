//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger.hpp
//
//  Purpose:
//      基于asio_server实现的tcp服务器, 提供命令的接收处理以及异步的logger打印接口
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "FIFOManage.hpp"
#include "productConfig.hpp"
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

class LoggerManage final
{
private:
    char *pNextMemoryBuffer;
    char *pEndMemoryBuffer;

    //logger info
    LOG_LEVEL log_level{LOG_INFO};

    //thread
    std::atomic<bool> set_thread_work{false};
    bool is_thread_work{false};
    std::thread m_TxThread;
    std::thread m_AsioServerThread;
    std::mutex mutex_;

    //fd
    LOG_MESSAGE message;
    FIFOManage *pLoggerFIFO{nullptr};

    static LoggerManage *pInstance;
    
    void logger_rx_run();
    void logger_tx_run();
    void asio_server_run();

private:
    char *getMemoryBuffer(uint16_t size);
    void mutex_lock()       {if(is_thread_work) mutex_.lock();}
    void mutex_unlock()     {if(is_thread_work) mutex_.unlock();}

public:
    LoggerManage() = default;
    ~LoggerManage() = delete;

    bool init();
    void release();

    bool createfifo();
    int print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...);
    static LoggerManage *getInstance();

    void setThreadWork()            {set_thread_work = true;}
    void setlevel(LOG_LEVEL level)  {log_level = level;}
    LOG_LEVEL getlevel()            {return log_level;} 
};

#define PRINT_NOW(...)    { printf(__VA_ARGS__); fflush(stdout);}
#define PRINT_LOG(level, time, fmt, ...) do{ LoggerManage::getInstance()->print_log(level, time, fmt, ##__VA_ARGS__); }while(0);
