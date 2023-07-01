
#include "logger.hpp"
#include "monitor.hpp"
#include "adc.hpp"
#include "lcd.hpp"
#include "tpad.hpp"
#include "rtc.hpp"
#include "rng.hpp"
#include "i2c_monitor.hpp"
#include "dac.hpp"

KEY_STATE monitor_manage::key_last_[KEY_NUM];
KEY_STATE monitor_manage::key_now_[KEY_NUM];
uint8_t monitor_manage::tick[KEY_NUM];

bool monitor_manage::init()
{
    BaseType_t xReturned;
    int index;
    
    for(index=0; index<KEY_NUM; index++)
    {
        key_last_[index] = KEY_OFF;
        key_now_[index] = KEY_OFF;
        tick[index] = 0;
    }
    
    xReturned = xTaskCreate(
                    run,      
                    "monitor_manage",        
                    MONITOR_TASK_STACK,              
                    ( void * ) NULL,   
                    MONITOR_TASK_PROITY,
                    &task_handle_ );      
  
   if(xReturned == pdPASS)
       return true;
   return false;
}

bool monitor_manage::is_time_escape(uint32_t ticks , uint32_t time)
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

std::function<void()> key_func_list[] = {
    [](){
        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Key0 Push down!");
        i2c_monitor::get_instance()->write_io(OUTPUT_BEEP, IO_ON);
    },
    [](){
        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Key1 Push down!");
        i2c_monitor::get_instance()->write_io(OUTPUT_BEEP, IO_OFF);
    },
    [](){
        static uint16_t voltage = 0;
        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Key2 Push down!");
        voltage += 200;
        if(voltage > 3300)
            voltage = 0;
        dac_driver::get_instance()->set_voltage(voltage);
    },
    [](){
        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Tpad Key Push down, no_push:%d, push:%d!",
            tpad_driver::get_instance()->get_no_push_value(),
            tpad_driver::get_instance()->get_current_value()
        );
        rtc_driver::get_instance()->delay_alarm(0, 0, 0, 5);
    },    
    [](){
        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "EXIO Push down!");
    },    
};

void monitor_manage::key_motion()
{
    key_now_[0] = monitor_manage::get_instance()->anti_shake(&tick[0], key_now_[0], key_get_value(0));
    key_now_[1] = monitor_manage::get_instance()->anti_shake(&tick[1], key_now_[1], key_get_value(1));
    key_now_[2] = monitor_manage::get_instance()->anti_shake(&tick[2], key_now_[2], key_get_value(2));
    key_now_[3] = monitor_manage::get_instance()->anti_shake(&tick[3], key_now_[3], tpad_driver::get_instance()->scan_key()==1?KEY_ON:KEY_OFF);
    key_now_[4] = monitor_manage::get_instance()->anti_shake(&tick[4], key_now_[4], i2c_monitor::get_instance()->get_read_io()->u.exio==0?KEY_ON:KEY_OFF);
    
    for(int index=0; index<KEY_NUM; index++)
    {
        if(key_now_[index] == KEY_ON)
        {
            if(key_last_[index] != key_now_[index])
            {
                key_last_[index] = key_now_[index];
                key_func_list[index]();
            }
        }
        else
        {
            key_last_[index] = key_now_[index];
        }
    }
}

void monitor_manage::timer_loop_motion()
{
    static uint8_t last_second = 0;
    static uint8_t temp_loop = 0;
    char tbuf[60];
    
     temp_loop++;
     if(temp_loop >= 10)
     {
        temp_loop = 0;
        
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

            PRINT_LOG(LOG_DEBUG, xTaskGetTickCount(), "20%02d-%02d-%02d Wek:%02d %02d:%02d:%02d rng:%d ",
            pdate->Year,
            pdate->Month,
            pdate->Date,
            pdate->WeekDay,
            ptimer->Hours,
            ptimer->Minutes,
            ptimer->Seconds,
            rng_driver::get_instance()->get_value());
        }
    }
     
    if(rtc_driver::get_instance()->get_alarm())
    {
        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "RTC Alarm");

        rtc_driver::get_instance()->set_alarm(false);
    }
}

void monitor_manage::adc_monitor()
{
    static uint8_t temp_loop = 0;
    uint32_t adc_temp = 0, adc_vol;
    double temperate;
    double voltage;

    temp_loop++;
    if(temp_loop >= 20)
    {
        temp_loop = 0;

        adc_temp = adc_driver::get_instance()->get_adc_avg(ADC_CHANNEL_TEMPSENSOR);
        temperate = (float)adc_temp*(3.3/4096);		//电压值
        temperate = (temperate-0.76)/0.0025 + 25;     //转换为温度值 

        lcd_driver::get_instance()->lcd_show_extra_num(10+11*8,140,(uint32_t)temperate, 2, 16, 0);		//显示整数部分
        lcd_driver::get_instance()->lcd_show_extra_num(10+14*8,140,((uint32_t)(temperate*100))%100, 2, 16, 0);		//显示小数部分 
        
        //PB1 - ADC Channel 9
        adc_vol = adc_driver::get_instance()->get_adc_avg(ADC_CHANNEL_9);
        voltage = (float)adc_vol*(3.3/4096);
        lcd_driver::get_instance()->lcd_show_extra_num(10+23*8,140,(uint32_t)voltage, 2, 16, 0);		//显示整数部分
        lcd_driver::get_instance()->lcd_show_extra_num(10+26*8,140,((uint32_t)(voltage*100))%100, 2, 16, 0);		//显示小数部分 
    }
}
    
void monitor_manage::run(void* parameter)
{
    while(1)
    {
        //key motion loop
        key_motion();
        
        timer_loop_motion();
        
        adc_monitor();
        
        vTaskDelay(20);
    }
}

KEY_STATE monitor_manage::anti_shake(uint8_t *pTick, KEY_STATE nowIoStatus, KEY_STATE readIoStatus)
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