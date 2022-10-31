_Pragma("once")

#include <mutex>
#include <condition_variable>

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
    
    void wait()
    {
        std::unique_lock<std::mutex> lock(mt);
        if(--count < 0)
        {
            cv.wait(lock, [this]()->bool{
                return wakeups > 0;
            });
            --wakeups;
        }
    }
private:
    std::mutex mt;
    std::condition_variable cv;
    int count;
    int wakeups;
};
