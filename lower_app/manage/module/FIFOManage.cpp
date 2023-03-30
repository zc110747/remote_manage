//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      FIFOManage.cpp
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
#include "FIFOManage.hpp"
#include "logger.hpp"
//#include "driver.hpp"

FIFOManage::FIFOManage(const std::string& fstr, int mode){
    fifoM = std::move(fstr);
    modeM = mode;
    readfdM = -1;
    writefdM = -1;
}

FIFOManage::~FIFOManage()
{
    Release();
    fifoM.clear();
}

bool FIFOManage::Create(void){
    
    //delete fifo
    unlink(fifoM.c_str());

    if(mkfifo(fifoM.c_str(), modeM) < 0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(),  "fifo %s make error!", fifoM.c_str());
        return false;
    }

    //for Read, need open as O_RDWR, otherwise will block
    readfdM = open(fifoM.c_str(), O_RDWR);
    writefdM = open(fifoM.c_str(), O_WRONLY);

    if(writefdM < 0 || readfdM < 0)
    {
        Release();
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(),  "fifo open error:%d, %d!", writefdM, readfdM);
        return false;
    }
    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "fifo %s open success, fd:%d, %d", fifoM.c_str(), writefdM, readfdM);
    return true;
}                      

void FIFOManage::Release(){

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

int FIFOManage::read(char *buf, int bufsize){
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

int FIFOManage::write(char *buf, int bufsize){
    int writebytes = -1;
    if(writefdM >= 0)
    {
        writebytes = ::write(writefdM, buf, bufsize);
    }
    return writebytes;
}  