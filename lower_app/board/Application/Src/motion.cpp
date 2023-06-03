
#include "logger.hpp"
#include "motion.hpp"
#include "adc.hpp"
#include "lcd.hpp"
#include "tpad.hpp"
#include "rtc.hpp"
#include "rng.hpp"

bool motion_manage::init()
{
    BaseType_t xReturned;
    
    xReturned = xTaskCreate(
                    run,      
                    "motion_manage",        
                    MOTION_TASK_STACK,              
                    ( void * ) NULL,   
                    MOTION_TASK_PROITY,
                    &task_handle_ );      
  
   if(xReturned == pdPASS)
       return true;
   return false;
}

bool motion_manage::is_time_escape(uint32_t ticks , uint32_t time)
{
    uint32_t tick_end = ticks + time;
    uint32_t now_tick =  xTaskGetTickCount();
    
    if(tick_end > ticks)
    {
        if(now_tick >= tick_end 
        || ((now_tick < tick_end) && ((tick_end - now_tick) > 65535)))
            return true;
    }
    else
    {
        if(now_tick >= tick_end && now_tick < ticks)
            return true;
    }
    
    return false;
}

void motion_manage::run(void* parameter)
{
    uint8_t tick = 0;
    GPIO_PinState last_key, now_key;
    uint8_t last_tpad_key, now_tpad_key;
    uint32_t adc_temp;
    double temperate;
    uint8_t last_second = 0;
    
    now_key = KEY0::get_instance()->get_value();
    last_tpad_key = tpad_driver::get_instance()->scan_key();
    
    while(1)
    {
        //key interrupt 
        if(KEY1::get_instance()->is_interrupt)
        {
            if(KEY1::get_instance()->get_value() == KEY_ON)
            {
                if(motion_manage::get_instance()->is_time_escape(KEY1::get_instance()->ticks, KEY_CHECK_TIMES_MS))
                {
                    PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Key1 Push down!");
                    KEY1::get_instance()->is_interrupt = false;
                }
            }
            else
            {
                KEY1::get_instance()->is_interrupt = false; 
            }
        }
        
        //key input
        now_key = motion_manage::get_instance()->anti_shake(&tick, now_key, KEY0::get_instance()->get_value());
        if(now_key == KEY_ON)
        {
            if(last_key != now_key)
            {
                PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Key0 Push down!");
                last_key = now_key;
            }
        }
        else
        {
            last_key = now_key;
        }
        
        {
            static uint8_t temp_loop = 0;

            temp_loop++;
            if(temp_loop >= 10)
            {
                char tbuf[60];
                
                temp_loop = 0;
  
                //adc tempature
                adc_temp = adc_driver::get_instance()->get_adc_avg(ADC_CHANNEL_TEMPSENSOR);
                temperate=(float)adc_temp*(3.3/4096);		//电压值
                temperate=(temperate-0.76)/0.0025 + 25;     //转换为温度值 
                temp_loop = 0;
                
                lcd_driver::get_instance()->lcd_show_extra_num(10+11*8,140,(uint32_t)temperate, 2, 16, 0);		//显示整数部分
                lcd_driver::get_instance()->lcd_show_extra_num(10+14*8,140,((uint32_t)(temperate*100))%100, 2, 16, 0);		//显示小数部分 
                
                //rtc timer
                rtc_driver::get_instance()->update();
                auto ptimer = rtc_driver::get_instance()->get_current_time();
                auto pdate = rtc_driver::get_instance()->get_current_date();
                if(last_second != ptimer->Seconds)
                {
                    last_second = ptimer->Seconds;
                    sprintf(tbuf, "Timer: %02d-%02d-%02d %02d:%02d:%02d",
                        pdate->Year,
                        pdate->Month,
                        pdate->Date,
                        ptimer->Hours,
                        ptimer->Minutes,
                        ptimer->Seconds);
                    lcd_driver::get_instance()->lcd_showstring(10, 160, 200, 16, 16, tbuf);
                    
                    PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "20%02d-%02d-%02d Wek:%02d %02d:%02d:%02d tempature:%.2f, rng:%d ",
                        pdate->Year,
                        pdate->Month,
                        pdate->Date,
                        pdate->WeekDay,
                        ptimer->Hours,
                        ptimer->Minutes,
                        ptimer->Seconds,
                        temperate,
                        rng_driver::get_instance()->get_value());    
                }
            }
        }
        
        if(rtc_driver::get_instance()->get_alarm())
        {
            PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "RTC Alarm");
            rtc_driver::get_instance()->set_alarm(false);
        }
        
        //tpad key
        now_tpad_key = tpad_driver::get_instance()->scan_key();
        if(now_tpad_key == 1)
        {
            if(last_tpad_key != now_tpad_key)
            {
                last_tpad_key = now_tpad_key;
                PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Tpad Key Push down, no_push:%d, push:%d!",
                    tpad_driver::get_instance()->get_no_push_value(),
                    tpad_driver::get_instance()->get_current_value()
                );
                
                rtc_driver::get_instance()->delay_alarm(0, 0, 0, 5);
            }
        }
        else
        {
            last_tpad_key = now_tpad_key;  
        }
       
        
        vTaskDelay(25);
    }
}

KEY_STATE motion_manage::anti_shake(uint8_t *pTick, KEY_STATE nowIoStatus, KEY_STATE readIoStatus)
{
    GPIO_PinState OutIoStatus = nowIoStatus;

    (*pTick) += 1;

    if(nowIoStatus == KEY_ON)
    {
        if(readIoStatus == KEY_OFF)
        {
            if(*pTick > ANTI_SHAKE_TICK)
            {
                OutIoStatus = KEY_OFF;
                *pTick = 0;
            }
        }
        else
        {
            *pTick = KEY_ON;
        }
    }
    else if(nowIoStatus == KEY_OFF)
    {
        if(readIoStatus == KEY_ON)
        {
            if(*pTick > ANTI_SHAKE_TICK)
            {
                OutIoStatus = KEY_ON;
                *pTick = 0;
            }
        }
        else
        {
            *pTick = KEY_OFF;
        }
    }

    return OutIoStatus;    
}