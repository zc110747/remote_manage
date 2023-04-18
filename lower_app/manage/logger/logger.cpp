//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger.cpp
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
#include <stdarg.h>
#include "logger.hpp"
#include "driver.hpp"
#include "asio_server.hpp"

static asio_server logger_server;

//asio server test ok
void LoggerManage::logger_rx_server_run()
{
    const auto& socket_config = system_config::get_instance()->get_logger_config();
    cmd_process Loggercmd_process;

    try
    {
        logger_server.init(socket_config.ipaddr, std::to_string(socket_config.port), [&Loggercmd_process](char* ptr, int length){
            if(Loggercmd_process.parse_data(ptr, length))
            {
                Loggercmd_process.process_data();
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
    LOG_MESSAGE logger_message_;
    
    update_thread_in_work();

    while(1)
    {
        len = logger_fifo_->read((char *)&logger_message_, sizeof(logger_message_));
        if(len > 0)
        {
            if(logger_server.is_valid())
            {
                logger_server.do_write(logger_message_.ptr, logger_message_.length);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                len = ::write(STDOUT_FILENO, logger_message_.ptr, logger_message_.length);
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

LoggerManage* LoggerManage::instance_pointer_ = nullptr;
LoggerManage* LoggerManage::get_instance()
{
    if(instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) LoggerManage;
        if(instance_pointer_ == nullptr)
        {
            //do something
        }
    }
    return instance_pointer_;
}

char memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE+1];
bool LoggerManage::init()
{
    memory_start_pointer_ = memoryBuffer;
    memory_end_pointer_ = &memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE];

    //init and Create logger fifo, must before thread run.
    logger_fifo_ = new(std::nothrow) fifo_manage(LOGGER_FIFO_PATH, S_FIFO_WORK_MODE);
    if(logger_fifo_ == nullptr)
        return false;
    if(!logger_fifo_->create())
        return false;

    //init thread for logger
    logger_tx_thread_ = std::thread(std::bind(&LoggerManage::logger_tx_run, this));
    logger_tx_thread_.detach();
    logger_rx_thread_ = std::thread(std::bind(&LoggerManage::logger_rx_server_run, this));
    logger_rx_thread_.detach();

    return true;
}

void LoggerManage::release()
{
    logger_fifo_->release();
}

char *LoggerManage::get_memory_buffer_pointer(uint16_t size)
{
    char *pCurrentMemBuffer;

    pCurrentMemBuffer = memory_start_pointer_;
    memory_start_pointer_ = pCurrentMemBuffer+size;
	if(memory_start_pointer_ >  memory_end_pointer_)
	{
		pCurrentMemBuffer = memoryBuffer;
		memory_start_pointer_ = pCurrentMemBuffer + size;
	}
	return(pCurrentMemBuffer);
}

int LoggerManage::print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...)
{
    int len, bufferlen;
    char *pbuf, *pstart;
    bool is_thread_work = false;

    if(level < log_level_)
        return 0;

    is_thread_work = thread_work_;

    mutex_.lock();

    pstart = get_memory_buffer_pointer(LOGGER_MAX_BUFFER_SIZE);
    len = LOGGER_MAX_BUFFER_SIZE;
    bufferlen = len - 1;
    pbuf = pstart;
    logger_message_.length = 0;
    logger_message_.ptr = pstart;

    len = snprintf(pbuf, bufferlen, "LogLevel:%d time:%d info:",level, time);
    if((len<=0) || (len>=bufferlen))
    {
        PRINT_NOW("%s label0 error\n", __func__);
        mutex_.unlock();
        return 0;
    }

    logger_message_.length += len;
    pbuf = &pbuf[len];
    bufferlen -= len;
    
    va_list	valist;
    va_start(valist, fmt);
	len = vsnprintf(pbuf, bufferlen, fmt, valist);
	va_end(valist);
    mutex_.unlock();

    if((len<=0) || (len>=bufferlen))
    {
        PRINT_NOW("%s label1 error\n", __func__);
        return 0;
    }
    
    logger_message_.length += len;
    pbuf = &pbuf[len];
    bufferlen -= len;

    if(bufferlen < 3)
    {
        PRINT_NOW("%s label2 error\n", __func__);
        return 0;
    }
    
    pbuf[0] = '\r';
    pbuf[1] = '\n';
    logger_message_.length += 2;

    if(!is_thread_work)
    {
        len = write(STDOUT_FILENO, logger_message_.ptr, logger_message_.length);
        if(len<0)
        {
            //do something error
        }
        fflush(stdout);
    }
    else
    {
        
        len = logger_fifo_->write((char *)&logger_message_, sizeof(logger_message_));
        if(len<=0)
        {
            PRINT_NOW("%s lable3 error\n", __func__);
        }
        else
        {
            //PRINT_NOW("%s writefd\n", __func__);
        }
    }
    
    return  logger_message_.length;
}

