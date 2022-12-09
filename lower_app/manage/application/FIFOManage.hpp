//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      FIFOMessage.hpp
//
//  Purpose:
//      Communication by fifo interface.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_FIFO_MANAGE_HPP
#define _INCLUDE_FIFO_MANAGE_HPP

#include "logger.hpp"
#include <mqueue.h>
#include "driver.hpp"

#define S_FIFO_WORK_MODE               (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  

class FIFOMessage
{
private:
    std::string  fifoM;
    int modeM;
    int readfdM;
    int writefdM;

public:
    FIFOMessage(const std::string& fstr, int mode){
        fifoM = std::move(fstr);
        modeM = mode;
        readfdM = -1;
        writefdM = -1;
    }
    ~FIFOMessage(){
        Release();
        fifoM.clear();
    }

    /*创建并打开FIFO*/
    bool Create(void){
        
        //delete fifo
        unlink(fifoM.c_str());

        if(mkfifo(fifoM.c_str(), modeM) < 0)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(),  "fifo %s make error!", fifoM.c_str());
            return false;
        }

        writefdM = open(fifoM.c_str(), O_RDWR);
        readfdM = open(fifoM.c_str(), O_RDWR);

        if(writefdM < 0 || readfdM < 0)
        {
            Release();
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(),  "fifo open error:%d, %d!", writefdM, readfdM);
            return false;
        }
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "fifo %s open success, fd:%d, %d", fifoM.c_str(), writefdM, readfdM);
        return true;
    }                      

    /*关闭FIFO并释放资源*/
    void Release(){

        if(writefdM >= 0)
        {
            close(writefdM);
            writefdM = -1;
        }

        if(readfdM >= 0)
        {
            close(readfdM);
            readfdM = -1;
        }
    }       
    
    /*等待FIFO数据接收*/
    int read(char *buf, int bufsize){
        int readbytes = -1;
        if(readfdM >= 0)
        {
            readbytes = ::read(readfdM, buf, bufsize);
        }
        else
        {

        }

        return readbytes;
    }              

    /*向FIFO中投递数据*/
    int write(char *buf, int bufsize){
        int writebytes = -1;
        if(writefdM >= 0)
        {
            writebytes = ::write(writefdM, buf, bufsize);
        }
        return writebytes;
    }  
};
#endif
