_Pragma("once")

#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include <iostream>

namespace DeviceManage
{
    class Timer
    {
    public:
        Timer() = default;
        Timer(const Timer &time) = delete;
        ~Timer(){
            stop();
        }

        void start(int interval, std::function<void()> task)
        {
            milli_s = interval;
            m_task = task;

            //start thread
            std::thread(std::bind(&Timer::run, this)).detach();
        }

        void stop()
        {
            if(m_stop)
                return;
            
            m_stop = true;
            cond.notify_one();
        }

    private:
        void run()
        {
            m_stop = false;

            while(!m_stop)
            {
                std::unique_lock<std::mutex> lock{mt};
                auto stauts = cond.wait_for(lock, std::chrono::milliseconds(milli_s));
                if(stauts == std::cv_status::timeout)
                {
                    m_task();
                }
                else
                {
                    std::cout<<"timer finished\n";
                }
            }
        }

    private:
        std::condition_variable cond;
        std::mutex mt;
        uint32_t milli_s{1};
        std::function<void()> m_task;
        std::atomic<bool> m_stop{false};
    };
}

uint32_t xGetCurrentTicks(void);
