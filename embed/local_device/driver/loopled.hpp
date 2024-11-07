//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_base.hpp
//
//  Purpose:
//      封装的设备底层接口，基类为device_base, 支持open/close
//      io_base主要封装IO支持的接口, open, read, write, close
//      info_base主要封装支持特定外设的接口, 可以读取内部信息
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

#include "device_base.hpp"

#define LOOP_LED_ON                 1
#define LOOP_LED_OFF                0

#define LOOP_LED_YELLOW             0
#define LOOP_LED_RED                1
#define LOOP_LED_GREEN              2

#define GREEN_TICK_TICKCOUNT        60
#define GREEN_BLINK_TICKCOUNT       ((GREEN_TICK_TICKCOUNT) + 10)
#define YELLOW_TICKCOUNT            ((GREEN_BLINK_TICKCOUNT) + 6)
#define RED_TICK_TICKCOUNT          ((YELLOW_TICKCOUNT) + 40)

class loopled_device: public device_base
{
public:
    /// \brief constructor
    using device_base::device_base;

    /// @brief loop_fresh_run
    /// - This method is used to refresh the device.
    void loop_refresh_per_second(void)
    {
        if (is_loopled_run)
        {
            if (tick == 0)
            {
                write_multi_io(LOOP_LED_OFF, LOOP_LED_OFF, LOOP_LED_ON);
                tick++;
            }
            else if (tick >= GREEN_TICK_TICKCOUNT && tick < GREEN_BLINK_TICKCOUNT)
            {
                if (tick % 2 == 0)
                {
                    write_io_status(LOOP_LED_GREEN, LOOP_LED_OFF);
                }
                else
                {
                    write_io_status(LOOP_LED_GREEN, LOOP_LED_ON);
                }
                tick++;
            }
            else if(tick == GREEN_BLINK_TICKCOUNT)
            {
                write_io_status(LOOP_LED_GREEN, LOOP_LED_OFF);
                write_multi_io(LOOP_LED_OFF, LOOP_LED_ON, LOOP_LED_OFF);
                tick++;
            }
            else if (tick == YELLOW_TICKCOUNT)
            {
                write_io_status(LOOP_LED_YELLOW, LOOP_LED_OFF);
                write_multi_io(LOOP_LED_ON, LOOP_LED_OFF, LOOP_LED_OFF);
                tick++;
            }
            else if(tick >= RED_TICK_TICKCOUNT)
            {
                write_multi_io(LOOP_LED_OFF, LOOP_LED_OFF, LOOP_LED_OFF);
                tick = 0;
            }
            else
            {
                tick++;
            }
            PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "%d", tick);
        }
        else
        {
            tick = 0;

            write_multi_io(LOOP_LED_OFF, LOOP_LED_OFF, LOOP_LED_OFF);
        }
    }

    bool write_multi_io(uint8_t red, uint8_t yellow, uint8_t green)
    {
        if (!write_io_status(LOOP_LED_RED, red))
            return false;

        if(!write_io_status(LOOP_LED_YELLOW, yellow))
            return false;

        if(!write_io_status(LOOP_LED_GREEN, green))
            return false;

        return true;
    }

    bool write_io_status(uint8_t led, uint8_t status) 
    {
        bool ret = false;
        ssize_t nSize;
        uint8_t buf[2];

        buf[0] = led;
        buf[1] = status;

        if (device_fd_ >= 0)
        {
            nSize = ::write(device_fd_, buf, 2); 
            if (nSize > 0)
            {
                ret = true;
            }
        }
        return ret;
    }

    void set_state(bool state)
    {
        is_loopled_run = state;
        if (is_loopled_run == false)
        {
            tick = 0;
        }
    }

private:
    /// \brief is_loopled_run
    /// - wheather loop run state
    std::atomic<bool> is_loopled_run{false};

    uint16_t tick{0};
};
