_Pragma("once")

#include <mutex>
#include <condition_variable>

namespace EVENT
{
    class Semaphore
    {
    public:
        Semaphore(int count = 0):wakeups(count) {}
            virtual ~Semaphore() {}
        
        void signal()
        {
            std::lock_guard<std::mutex> lock(mt);
            ++wakeups;
            cv.notify_one();
        }
        
        bool wait(uint32_t timeout)
        {
            bool ret;

            std::unique_lock<std::mutex> lock(mt);
            ret = cv.wait_for(lock, std::chrono::milliseconds(timeout), [this]()->bool{
                return wakeups > 0;
            });
            if(ret)
                --wakeups;
            
            return ret;
        }

        bool wait()
        {
            std::unique_lock<std::mutex> lock(mt);
            //在这一步释放了lock, 同时进行解锁
            cv.wait(lock, [this]()->bool{
                return wakeups > 0;
            });
            --wakeups;
            return true;
        }

    private:
        std::mutex mt;
        std::condition_variable cv;
        int wakeups;
    };
}

