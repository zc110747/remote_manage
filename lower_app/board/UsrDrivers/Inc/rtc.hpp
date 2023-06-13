//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     rtc.hpp
//
//  Purpose:
//     rtc interface driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "includes.hpp"

class rtc_driver
{
public:
    BaseType_t init();

    static rtc_driver* get_instance(){
        static rtc_driver instance_;
        return &instance_;
    } 
 
    bool update(void);
    
    HAL_StatusTypeDef set_time(uint8_t hour, uint8_t min, uint8_t sec);
    HAL_StatusTypeDef set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week);
    void set_alarm(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec);
    void delay_alarm(uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
    
public:
    RTC_TimeTypeDef* get_current_time()     { return &time_; }
    RTC_DateTypeDef* get_current_date()     { return &date_; }   
    RTC_HandleTypeDef* get_rtc_handler()    { return &rtc_handler_; }
    bool get_alarm()                        { return is_alarm; }
        
    void set_alarm(bool alarm) {
        is_alarm = alarm;
    }
    
private:
    BaseType_t hardware_init(); 
    bool test();

private:
    RTC_TimeTypeDef time_{0};
    RTC_DateTypeDef date_{0};
    
    RTC_HandleTypeDef rtc_handler_;
    RTC_AlarmTypeDef rtc_arm_handler_;
    
    bool is_alarm{false};
};