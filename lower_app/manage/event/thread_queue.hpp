//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      thread_queue.hpp
//
//  Purpose:
//      基于Semaphore实现的消息队列, 用于线程间传递消息
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include <queue>
#include "semaphore.hpp"
#include <iostream>

namespace EVENT
{
    template<typename T>
    class Thread_Queue
    {
    private:
        std::queue<T> Queue_;
        std::mutex  mut;
        Semaphore Semaphore_;
        
    public:
        void QueueSend(T& Object)
        {
            {
                std::unique_lock<std::mutex> lock(mut);
                Queue_.push(Object);
            }

            Semaphore_.signal();
        }

        bool QueueReceive(uint32_t timeout, T& Object)
        {
            int ret = false;

            if(Semaphore_.wait(timeout))
            {
                std::unique_lock<std::mutex> lock(mut);
                if(Queue_.size() != 0)
                {
                    Object = Queue_.front();
                    Queue_.pop();
                    ret = true;
                }
            }
            return ret;
        }
    };
}