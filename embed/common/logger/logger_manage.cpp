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
#include "logger_manage.hpp"
#include "time_manage.hpp"
#include "jsonconfig.hpp"
#include "asio_server.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

log_manage* log_manage::instance_pointer_ = nullptr;
log_manage* log_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) log_manage;
        if (instance_pointer_ == nullptr)
        {
            //do something
        }
    }
    return instance_pointer_;
}

char memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE+1];
bool log_manage::init()
{
    memory_start_pointer_ = memoryBuffer;
    memory_end_pointer_ = &memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE];

    //init and Create logger fifo, must before thread run.
    logger_fifo_ = std::make_unique<fifo_manage>(LOGGER_RX_FIFO, S_FIFO_WORK_MODE, FIFO_MODE_W);
    if (logger_fifo_ == nullptr)
        return false;
    if (!logger_fifo_->create())
        return false;

    return true;
}

void log_manage::release()
{
    logger_fifo_->release();
}

char *log_manage::get_memory_buffer_pointer(uint16_t size)
{
    char *pCurrentMemBuffer;

    pCurrentMemBuffer = memory_start_pointer_;
    memory_start_pointer_ = pCurrentMemBuffer+size;
	if (memory_start_pointer_ >  memory_end_pointer_)
	{
		pCurrentMemBuffer = memoryBuffer;
		memory_start_pointer_ = pCurrentMemBuffer + size;
	}
	return(pCurrentMemBuffer);
}

std::string log_manage::convert_timer(uint32_t timer)
{
    uint16_t sec, min, hours, day;

    sec = timer%60;
    timer = timer/60;
    min = timer%60;
    timer = timer/60;
    hours = timer%24;
    timer = timer/24;
    day = timer;

    std::string fmt_str = fmt::format("{0:0>4} {1:0>2}:{2:0>2}:{3:0>2}", day, hours, min, sec);
    return fmt_str;
}

static char logger_tx_buffer[LOGGER_MAX_BUFFER_SIZE];
int log_manage::print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...)
{
    int len;
    char *pbuf;
    int tx_len = 0;
    int bufferlen = LOGGER_MAX_BUFFER_SIZE;

    if (level < log_level_)
        return 0;

    mutex_.lock();

    //step1: add logger info header
    pbuf = logger_tx_buffer;
    len = snprintf(pbuf, bufferlen, "[%s][%s][%d]:", convert_timer(time).c_str(), TOOLS_NAME, level);
    if ((len<=0) || (len>=bufferlen))
    {
        PRINT_NOW("%s not support buffer-0!\n", __func__);
        mutex_.unlock();
        return 0;
    }
    pbuf = &pbuf[len];
    bufferlen -= len;
    tx_len += len;

    //step2: add logger info
    va_list	valist;
    va_start(valist, fmt);
	len = vsnprintf(pbuf, bufferlen, fmt, valist);
	va_end(valist);

    if ((len<=0) || (len>=bufferlen))
    {
        PRINT_NOW("%s not support buffer-1!\n", __func__);
        mutex_.unlock();
        return 0;
    }
    pbuf = &pbuf[len];
    bufferlen -= len;
    tx_len += len;
    mutex_.unlock();

    //step3: add logger tail \r\n
    if (bufferlen < 3)
    {
        PRINT_NOW("%s not support buffer-2!\n", __func__);
        return 0;
    } 
    pbuf[0] = '\r';
    pbuf[1] = '\n';
    tx_len += 2;

    //step4:send the logger info.
    if (!logger_fifo_->is_write_valid())
    {
        len = write(STDOUT_FILENO, logger_tx_buffer, tx_len);
        fflush(stdout);
    }
    else
    {      
        len = logger_fifo_->write(logger_tx_buffer, tx_len);
        if (len<=0)
        {
            PRINT_NOW("%s not support buffer-3!\n", __func__);
        }
    }
    
    return  tx_len;
}

