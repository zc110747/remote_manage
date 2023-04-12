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
    public:
        /// \brief queue_send
        /// - This method is used to post queue.
        /// \param Object - the object send to queue.
        void queue_send(T& Object)
        {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                queue_.push(Object);
            }

            semaphore_.signal();
        }

        /// \brief queue_receive
        /// - This method is used to receive queue.
        /// \param timeout - timeout wait for the queue.
        /// \param Object - the object receive from queue.
        /// \return wheather success receive the queue.
        bool queue_receive(uint32_t timeout, T& Object)
        {
            int ret = false;

            if(semaphore_.wait(timeout))
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if(queue_.size() != 0)
                {
                    Object = queue_.front();
                    queue_.pop();
                    ret = true;
                }
            }
            return ret;
        }

    private:
        /// \brief queue_
        /// - the queue for the communication.
        std::queue<T> queue_;

        /// \brief mutex_
        /// - mutex used to protect the queue.
        std::mutex  mutex_;

        /// \brief semaphore_
        /// - semaphore used to protect the queue.
        semaphore semaphore_;
    }
}