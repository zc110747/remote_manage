
#pragma once

#include "main.h"
#include "includes.hpp"

#define LOGGER_MAX_BUFFER_SIZE      256
#define LOGGER_MESSAGE_BUFFER_SIZE  16384
#define LOGGER_TX_QUEUE_NUM  		64

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

class logger_manage
{
public:
    bool init();

    static logger_manage *get_instance()
    {
        static logger_manage instance_;
        return &instance_;
    }
    
    int print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...);
    
    BaseType_t send_data(uint8_t data);
    
private:
    char *get_memory_buffer_pointer(uint16_t size);

    static void logger_rx_run(void *parameter);

    static void logger_tx_run(void *parameter);

    bool put_string(void);
    
    BaseType_t mutex_take(bool thread_ok, TickType_t tick);
    BaseType_t mutex_give(bool thread_ok);

private:
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
    static std::atomic<bool> thread_work_;
    
    /// \brief logger_message_
    /// - message used to save logger information.
    LOG_MESSAGE logger_message_;
    
    SemaphoreHandle_t mutex_;
    
    static QueueHandle_t tx_queue_;
    
    static QueueHandle_t rx_queue_;
    
    static uint8_t interface_;
};

#define PRINT_NOW(...)                   {printf(__VA_ARGS__);}
#define PRINT_LOG(level, time, fmt, ...) do{ logger_manage::get_instance()->print_log(level, time, fmt, ##__VA_ARGS__); }while(0);
