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
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/26/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <stdarg.h>
#include "logger.hpp"
#include "../driver/driver.hpp"

char memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE+1];

static void *loggerSocketThread(void *arg)
{
    int server_fd;
    LoggerManage *plogger = static_cast<LoggerManage *>(arg);
    struct sockaddr_in servaddr, clientaddr;  
    socklen_t client_sock_len;  
    int result, is_bind_fail;
    const SocketSysConfig *pSocketConfig = SystemConfig::getInstance()->getlogger();
    LOG_SOCKET *pSocket = LoggerManage::getInstance()->getsocket();

    memset(&servaddr, 0, sizeof(servaddr));    
    servaddr.sin_family = AF_INET;     
    servaddr.sin_addr.s_addr = inet_addr(pSocketConfig->ipaddr.c_str());  
    servaddr.sin_port = htons(pSocketConfig->port); 

    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "%s start!", __func__);
    server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_fd != -1)
    {

        int one = 1;
#ifndef WIN32
        /*Linux平台默认断开后2min内处于Wait Time状态，不允许重新绑定，需要添加配置，允许在该状态下重新绑定*/
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &one, (socklen_t)sizeof(one));
#endif

        do 
        {
            result = bind(server_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
            if(result == -1)
            {
                if(is_bind_fail == 0)
                {
                    is_bind_fail = 1;
                    PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Socket logger bind %s:%d failed!", pSocketConfig->ipaddr.c_str(), pSocketConfig->port); 
                }
                sleep(1);
            }
            else
            {
                break;
            }
        } while (1); //网络等待socket绑定完成后执行后续

        PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Socket logger bind %s:%d success!", pSocketConfig->ipaddr.c_str(), pSocketConfig->port); 
        listen(server_fd, 1);
        for(;;)
        {	   
            uint32_t client_size;
            int client_fd;

            client_size = sizeof(clientaddr);
            client_fd = accept(server_fd, (struct sockaddr *)&clientaddr, &client_size);
            if(client_fd < 0)
            {
                pSocket->islink = false;
                continue;
            } 
            else
            {
                pSocket->fd = client_fd;
                pSocket->islink = true;    
            }

            PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Socket logger accept success!");
            for(;;)
            {
                char recvbuf[64];
                int recvlen;

                recvlen = ::recv(client_fd, recvbuf, 64, 0);
                if(recvlen <= 0)
                {   
                    pSocket->islink = false;
                    pSocket->fd = -1;
                    close(client_fd);
                    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Socket logger recv error!");
                    break;
                }
                else
                {
                    if(cmdProcess::getInstance()->parseData(recvbuf, recvlen))
                    {
                        //PRINT_NOW("Socket logger command Process!");
                        cmdProcess::getInstance()->ProcessData();
                    }
                }
            }
        }
    }
    else
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Socket logger create failed!");
    }

    close(server_fd);
    return (void *)arg;
}

static void *loggerTxThread(void *arg)
{
    int len;
    LOG_MESSAGE message;
    LoggerManage *plogger = static_cast<LoggerManage *>(arg);
    LOG_SOCKET *pSocket = LoggerManage::getInstance()->getsocket();

    plogger->setThreadWork();
    while(1)
    {
        len = ::read(plogger->read_fd(), &message, sizeof(message));
        if(len > 0)
        {
            if(pSocket->islink)
            {
                len = ::send(pSocket->fd, message.ptr, message.length, 0);
                if(len < 0)
                {
                    //do something
                    PRINT_NOW("%s send failed:%d\n", __func__, len);
                }
                else
                {
                    printf("%s", message.ptr);
                    fflush(stdout);
                    usleep(300);
                }
            }
            else
            {
                len = ::write(STDOUT_FILENO, message.ptr, message.length);
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

    pNextMemoryBuffer = memoryBuffer;
    pEndMemoryBuffer = &memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE];

    createfifo();

    m_RxThread = std::move(std::thread(loggerSocketThread, this));
    m_TxThread = std::move(std::thread(loggerTxThread, this));
    pMutex = new(std::nothrow) std::mutex();

    if(pMutex == nullptr)
    {
        ret = false;
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "%s failed, err:%d!", __func__, nErr);
    }

    m_RxThread.detach();
    m_TxThread.detach();

    return ret;
}

void LoggerManage::release()
{
    is_thread_work = false;
    
    if(pMutex != nullptr)
    {
        delete pMutex;
        pMutex = nullptr;
    }

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
        PRINT_NOW("%s label0 error\n", __func__);
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
    {
        PRINT_NOW("%s label1 error\n", __func__);
        return 0;
    }
    
    message.length += len;
    pbuf = &pbuf[len];
    bufferlen -= len;

    if(bufferlen < 3)
    {
        PRINT_NOW("%s label2 error\n", __func__);
        return 0;
    }
    
    pbuf[0] = '\r';
    pbuf[1] = '\n';
    pbuf[2] = 0;
    message.length += 3;

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
        if(len<=0)
        {
            PRINT_NOW("%s label3 error\n", __func__);
        }
        else
        {
            //PRINT_NOW("%s writefd\n", __func__);
        }
    }
    
    return  message.length;
}

