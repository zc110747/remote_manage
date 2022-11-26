_Pragma("once")

#include <mutex>
#include <condition_variable>

namespace EVENT
{
    class Semaphore
    {
    public:
        Semaphore(int count = 0):count(count), wakeups(0) {}
            virtual ~Semaphore() {}
        
        void signal()
        {
            std::lock_guard<std::mutex> lock(mt);
            if(++count <= 0)
            {
                ++wakeups;
                cv.notify_one();
            }

        }
        
        bool wait()
        {
            std::unique_lock<std::mutex> lock(mt);
            if(--count < 0)
            {
                cv.wait(lock, [this]()->bool{
                    return wakeups > 0;
                });
                --wakeups;
            }
            return true;
        }
    private:
        std::mutex mt;
        std::condition_variable cv;
        int count;
        int wakeups;
    };
}

