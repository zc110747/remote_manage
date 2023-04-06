//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      fifo_manage.hpp
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
_Pragma("once")

#include "includes.hpp"

#define S_FIFO_WORK_MODE               (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)  

class fifo_manage final
{
public:
    fifo_manage(const std::string& fstr, int mode);
    ~fifo_manage();

    //创建并打开FIFO
    bool create(void);                   

    //关闭FIFO并释放资源
    void release();   
    
    //等待FIFO数据接收
    int read(char *buf, int bufsize);

    //向FIFO中写入数据
    int write(char *buf, int bufsize);

private:
    std::string  fifo_;
    int mode_;
    int readfd_;
    int writefd_;
};