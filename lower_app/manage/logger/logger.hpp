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

#include "fifo_manage.hpp"
#include "productConfig.hpp"
#include "cmd_process.hpp"
#include "time_manage.hpp"

#define LOGGER_MAX_BUFFER_SIZE      256
#define LOGGER_MESSAGE_BUFFER_SIZE  16384

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
public:
    /// \brief constructor
    LoggerManage() = default;

    /// \brief destructor, delete not allow for singleton pattern.
    ~LoggerManage() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static LoggerManage *get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief release
    /// - This method is used to release the object.
    /// \return Wheather release is success or failed.
    void release();

    /// \brief print_log
    /// - This method is print logger info.
    /// \param level - logger level for the data
    /// \param time - time when send the data
    /// \param fmt - pointer to send data
    /// \return the logger info already send.
    int print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...);

    /// \brief set_level
    /// - This method defined the level can set.
    /// \param level - logger level can send.
    void set_level(LOG_LEVEL level)  {log_level_ = level;}

    /// \brief get_level
    /// - This method get the level can send
    /// \return logger level can send.
    LOG_LEVEL get_level()            {return log_level_;} 

private:
    /// \brief logger_tx_run
    /// - tx thread to process logger info.
    void logger_tx_run();

    /// \brief logger_rx_server_run
    /// - rx thread to process logger interface received.
    void logger_rx_server_run();

    /// \brief get_memory_buffer_pointer
    /// - get buffer pointer of the memory.
    /// \param size - size of the memory pointer
    char *get_memory_buffer_pointer(uint16_t size);

    /// \brief update_thread_in_work
    /// - update thread already work.
    void update_thread_in_work()  {thread_work_ = true;}
    
private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static LoggerManage *instance_pointer_;

    /// \brief memory_start_pointer_
    /// - memory point the start to get.
    char *memory_start_pointer_;

    /// \brief memory_end_pointer_
    /// - memory point the end.
    char *memory_end_pointer_;

    /// \brief log_level_
    /// - log level defined.
    LOG_LEVEL log_level_{LOG_INFO};
    
    /// \brief thread_work_
    /// - wheather thread is work.
    std::atomic<bool> thread_work_{false};

    /// \brief logger_tx_thread_
    /// - logger tx thread object. 
    std::thread logger_tx_thread_;

    /// \brief logger_rx_thread_
    /// - logger rx thread object. 
    std::thread logger_rx_thread_;

    /// \brief mutex_
    /// - mutex used to protect output.
    std::mutex mutex_;

    /// \brief logger_message_
    /// - message used to save logger information.
    LOG_MESSAGE logger_message_;

    /// \brief logger_fifo_
    /// - fifo used for logger manage.
    std::unique_ptr<fifo_manage> logger_fifo_{nullptr};
};

#define PRINT_NOW(...)    { printf(__VA_ARGS__); fflush(stdout);}
#define PRINT_LOG(level, time, fmt, ...) do{ LoggerManage::get_instance()->print_log(level, time, fmt, ##__VA_ARGS__); }while(0);
