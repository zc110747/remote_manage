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
        size_t size() {
            return Queue_.size();
        }

        void QueueSend(T& Object)
        {
            {
                std::unique_lock<std::mutex> lock(mut);
                Queue_.push(Object);
            }
            //std::cout<<Queue_.size()<<" | ";
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
            else
            {
                std::cout<<"rx failed"<<" | ";
            }

            return ret;
        }
    };
}