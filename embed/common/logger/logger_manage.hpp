//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      logger_manage.hpp
//
//  Purpose:
//      logger结构转换接口，用于生成发送的logger信息。
//      logger格式:[run timer][tools][level]:info
//      example:
//          [00:00:01][main_process][2]:this is test info.
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common.hpp"
#include "fifo_manage.hpp"

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

typedef enum
{
    LOG_PRINT = 0,
    LOG_UPDATE = 1,
}LOG_ACTION;

typedef struct
{
    LOG_ACTION action;

    LOG_LEVEL level;
}CMD_LOG_INFO;

class log_manage final
{
public:
    /// \brief constructor
    log_manage() = default;

    /// \brief destructor, delete not allow for singleton pattern.
    ~log_manage() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static log_manage *get_instance();

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

    /// \brief convert_timer
    /// - This method convert second to home.
    /// \param timer - timer by second to convert.
    /// \return string after convert.
    std::string convert_timer(uint32_t timer);

private:
    /// \brief get_memory_buffer_pointer
    /// - get buffer pointer of the memory.
    /// \param size - size of the memory pointer
    char *get_memory_buffer_pointer(uint16_t size);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static log_manage *instance_pointer_;

    /// \brief memory_start_pointer_
    /// - memory point the start to get.
    char *memory_start_pointer_;

    /// \brief memory_end_pointer_
    /// - memory point the end.
    char *memory_end_pointer_;

    /// \brief log_level_
    /// - log level defined.
    LOG_LEVEL log_level_{LOG_INFO};

    /// \brief mutex_
    /// - mutex used to protect output.
    std::mutex mutex_;

    /// \brief logger_fifo_
    /// - fifo used for logger manage.
    std::unique_ptr<fifo_manage> logger_fifo_{nullptr};
};

#define PRINT_NOW(...)    { printf(__VA_ARGS__); fflush(stdout);}
#define PRINT_LOG(level, time, fmt, ...) do{ log_manage::get_instance()->print_log(level, time, fmt, ##__VA_ARGS__); }while (0);
