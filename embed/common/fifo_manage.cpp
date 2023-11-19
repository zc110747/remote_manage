//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      fifo_manage.cpp
//
//  Purpose:
//      FIFO管理接口，用于提供外部，支持创建读写的接口
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Add explian and update structure.
/////////////////////////////////////////////////////////////////////////////
#include "fifo_manage.hpp"
#include "logger.hpp"

fifo_manage::fifo_manage(const std::string& fstr, int mode){
    fifo_path_ = std::move(fstr);
    mode_ = mode;
    readfd_ = -1;
    writefd_ = -1;
}

fifo_manage::~fifo_manage()
{
    release();
    fifo_path_.clear();
}

bool fifo_manage::create(void){
    
    //delete fifo
    unlink(fifo_path_.c_str());

    if(mkfifo(fifo_path_.c_str(), mode_) < 0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(),  "fifo %s make error!", fifo_path_.c_str());
        return false;
    }

    //for Read, need open as O_RDWR, otherwise will block
    readfd_ = open(fifo_path_.c_str(), O_RDWR);
    writefd_ = open(fifo_path_.c_str(), O_WRONLY);

    if(writefd_ < 0 || readfd_ < 0)
    {
        release();
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(),  "fifo open error:%d, %d!", writefd_, readfd_);
        return false;
    }
    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "fifo %s open success, fd:%d, %d", fifo_path_.c_str(), writefd_, readfd_);
    return true;
}                      

void fifo_manage::release(){

    if(writefd_ >= 0)
    {
        close(writefd_);
        writefd_ = -1;
    }

    if(readfd_ >= 0)
    {
        close(readfd_);
        readfd_ = -1;
    }
}       

int fifo_manage::read(char *buf, int bufsize){
    int readbytes = -1;
    if(readfd_ >= 0)
    {
        readbytes = ::read(readfd_, buf, bufsize);
    }
    else
    {

    }

    return readbytes;
}              

int fifo_manage::write(char *buf, int bufsize){
    int writebytes = -1;
    if(writefd_ >= 0)
    {
        writebytes = ::write(writefd_, buf, bufsize);
    }
    return writebytes;
}  