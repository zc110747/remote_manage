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
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_FIFO_MANAGE_HPP
#define _INCLUDE_FIFO_MANAGE_HPP

#include "MessageBase.hpp"
#include "../../logger/logger.hpp"
#include <mqueue.h>

#define FIFO_MODE               (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  

#define SYSTEM_FIFO             "/tmp/sys.fifo"
#define APPLICATION_FIFO        "/tmp/app.fifo"  


class FIFOMessage:public MessageBase
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
    bool Create(void) override{
        
        //delete fifo
        unlink(fifoM.c_str());

        if(mkfifo(fifoM.c_str(), modeM) < 0)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(),  "fifo %s make error!", fifoM.c_str());
            return false;
        }

        writefdM = open(fifoM.c_str(), O_RDWR);
        readfdM = open(fifoM.c_str(), O_RDWR);

        if(writefdM < 0 || readfdM < 0)
        {
            Release();
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(),  "fifo open error:%d, %d!", writefdM, readfdM);
            return false;
        }
        PRINT_LOG(LOG_INFO, xGetCurrentTime(), "fifo %s open success, fd:%d, %d", fifoM.c_str(), writefdM, readfdM);
        return true;
    }                      

    /*关闭FIFO并释放资源*/
    void Release() override{

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
    int read(char *buf, int bufsize) override{
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
    int write(char *buf, int bufsize) override{
        int writebytes = -1;
        if(writefdM >= 0)
        {
            writebytes = ::write(writefdM, buf, bufsize);
        }
        return writebytes;
    }  
};

class FIFOManage
{
private:
    static FIFOManage* pInstance;
    std::vector<FIFOMessage *> messageM;

public:
    FIFOManage() = default;
    ~FIFOManage() = delete;
    static FIFOManage* getInstance();
    
    bool init();
    void release();

    MessageBase* getFIFOMessage(uint8_t index){
        return static_cast<MessageBase*>(messageM[index]);
    }
};
#endif
