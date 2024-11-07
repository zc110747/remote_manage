//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      time_manage.hpp
//
//  Purpose:
//      时间管理模块, 支持注册周期性触发的事件
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common.hpp"

#define TIME_ACTION_ALWAYS      0xFFFFFFF
#define TIME_INTERVAL           100
#define TIME_TICK(ms)           ((ms)/TIME_INTERVAL)

class timer_struct
{
public:
    /// \brief constructor
    timer_struct() = default;
    timer_struct(const timer_struct &time) = delete;

    /// \brief destructor
    ~timer_struct(){
        stop();
    }

    /// \brief start
    /// - This method is used to start timer_struct thread
    /// \param interval - time for the thread run period
    /// \param task - task when the thread run.
    void start(int interval, std::function<void()> task)
    {
        millisecond_ = interval;
        timeout_handler_ = task;

        //start thread
        std::thread(std::bind(&timer_struct::run, this)).detach();
    }

    /// \brief stop
    /// - This method is used to stop one timer_struct thread
    void stop()
    {
        if (stop_loop_)
            return;
        
        stop_loop_ = true;
        cond.notify_one();
    }

private:
    /// \brief stop
    /// - This method is used to run the truth thread.
    void run()
    {
        stop_loop_ = false;

        while (!stop_loop_)
        {
            std::unique_lock<std::mutex> lock{mutex_};
            auto stauts = cond.wait_for(lock, std::chrono::milliseconds(millisecond_));
            if (stauts == std::cv_status::timeout)
            {
                if (timeout_handler_)
                {
                    timeout_handler_();
                }
            }
            else
            {
                std::cout<<"timer_struct finished\n";
            }
        }
    }

private:
    /// \brief cond
    /// - cond used to wait timeout.
    std::condition_variable cond;

    /// \brief mutex_
    /// - mutex_ used for condition.
    std::mutex mutex_;

    /// \brief millisecond_
    /// - ms wait for timeout.
    uint32_t millisecond_{1};

    /// \brief timeout_handler_
    /// - function run when timeout.
    std::function<void()> timeout_handler_;

    /// \brief stop_loop_
    /// - loop flag until stop.
    std::atomic<bool> stop_loop_{false};
};

class time_action
{
public:
    /// \brief constructor
    /// \param id - event id for the process recall
    /// \param time - time for the period action
    /// \param cout - times for the action run
    /// \param func - function run when time action.
    time_action(int id, uint32_t time, uint32_t count, std::function<void(int)> func):
        event_id_(id), 
        tick_compare_(time), 
        action_times_(count),
        time_handler_(func) 
    {
        tick_count_ = 0;
    }

    /// \brief destructor
    ~time_action(){}

    /// \brief action
    /// - This method is used to run every time loop.
    void action()
    {
        if (action_times_ >0 && is_match())
        {
            if (action_times_ != TIME_ACTION_ALWAYS)
            {
                action_times_--;
            }

            time_handler_(event_id_);
        }
    }

    /// \brief get_event_id
    /// - This method is used to get the event id.
    /// \return return the event id of the action.
    uint32_t get_event_id()        {return event_id_;}

    /// \brief get_action_times
    /// - This method is used to get the rest run times.
    /// \return return the rest times the action run.
    uint32_t get_action_times()    {return action_times_;}
    
private:
    /// \brief is_match
    /// - This method is used to check wheather match time compare.
    /// \return return wheather is match the defined
    bool is_match()
    {
        tick_count_++;

        if (tick_count_ >= tick_compare_)
        {
            tick_count_ = 0;
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    /// \brief time_handler_
    /// - function run when time action.
    std::function<void(int)> time_handler_;

    /// \brief tick_count_
    /// - tick plus for internal.
    uint32_t tick_count_;

    /// \brief tick_compare_
    /// - tick compare for user defined.
    uint32_t tick_compare_;

    /// \brief event_id_
    /// - event id for the run action.
    uint32_t event_id_;

    /// \brief action_times_
    /// - times for the action run.
    uint32_t action_times_;
};

class time_manage
{
public:
    /// \brief constructor
    time_manage(): ticks_(0) {
    }
    time_manage(const time_manage&)=delete;

    /// \brief destructor, delete not allow for singleton pattern.
    virtual ~time_manage()=delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static time_manage *get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \param timeInterval - period for the timer_struct run.
    /// \return Wheather initialization is success or failed.
    bool init(uint32_t timeInterval = TIME_INTERVAL);

    /// \brief remove_action
    /// - This method is used to remove the action.
    /// \param id - the id of the action need remove.
    /// \return Wheather remove is success or failed.
    bool remove_action(uint32_t id);

    /// \brief register_action
    /// - This method is used to register the action.
    /// \param id - the id of the action need register.
    /// \param time - the time peroid_ for the action run.
    /// \param count - the tick of  the action run.
    /// \param func - funtion run when the action satisfy.
    /// \return Wheather register is success or failed.
    bool register_action(uint32_t id, uint32_t time, uint32_t count, std::function<void(int id)> func);

    /// \brief get_current_ticks
    /// - This method is used to get run timer_struct ticks.
    /// \return the ticks of the thread run.
    uint32_t get_current_ticks() {return ticks_;}

    /// \brief get_current_ticks
    /// - This method is used to get work peroid
    /// \return the time of the 1 ticks.
    uint32_t get_current_period() {return peroid_;}

private:
    /// \brief run
    /// - This method is used to run the timer_struct management.
    void run();

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static time_manage *instance_pointer_;

    /// \brief timer_
    /// - object used to do timer .
    timer_struct timer_;

    /// \brief time_action_vec_
    /// - vector use to save all action .
    std::vector<time_action> time_action_vec_;

    /// \brief mutex_
    /// - mutex use to protect time action vector.
    std::mutex mutex_;

    /// \brief ticks_
    /// - ticks show current run peroid_.
    std::atomic<uint32_t> ticks_;

    /// \brief peroid_
    /// - peroid defined the interval.
    uint32_t peroid_;
};

uint32_t xGetCurrentTicks(void);
uint32_t xGetCurrentPeroid(void);
uint32_t xGetCurrentTimes(void); //time second.
