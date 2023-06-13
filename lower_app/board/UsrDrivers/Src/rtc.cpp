//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     rtc.cpp
//
//  Purpose:
//     rtc driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "rtc.hpp"

#define RTC_DATE_YEAR       23
#define RTC_DATE_MONTH      06
#define RTC_DATE_DAY        03
#define RTC_DATE_WEEK       6

#define RTC_TIME_HOUR       19
#define RTC_TIME_MINUTE     49
#define RTC_TIME_SECOND     00

#define RTC_SET_FLAGS       0x5A5A
#define RTC_FORMAT_MODE     RTC_FORMAT_BIN

BaseType_t rtc_driver::init()
{
    BaseType_t result;
    
    //unlock backup register update.
    HAL_PWR_EnableBkUpAccess();
    
    result = hardware_init();
    
    if(result == pdPASS)
    {
        delay_alarm(0, 0, 0, 5);
    }
    else
    {
        printf("rtc_driver hardware_init failed\r\n");
    }
    return result;
}

HAL_StatusTypeDef rtc_driver::set_time(uint8_t hour, uint8_t min, uint8_t sec)
{
    time_.Hours = hour;
    time_.Minutes = min;
    time_.Seconds = sec;
    time_.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time_.StoreOperation = RTC_STOREOPERATION_RESET;
    
    return HAL_RTC_SetTime(&rtc_handler_, &time_, RTC_FORMAT_MODE);
}

HAL_StatusTypeDef rtc_driver::set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    date_.Year = year;
    date_.Month = month;
    date_.Date = date;
    date_.WeekDay = week;
    
    return HAL_RTC_SetDate(&rtc_handler_, &date_, RTC_FORMAT_MODE);
}

void rtc_driver::set_alarm(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{
    rtc_arm_handler_.AlarmTime.Hours= hour;  
    rtc_arm_handler_.AlarmTime.Minutes = min; 
    rtc_arm_handler_.AlarmTime.Seconds = sec; 
    rtc_arm_handler_.AlarmTime.SubSeconds = 0;
    rtc_arm_handler_.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    
    rtc_arm_handler_.AlarmMask = RTC_ALARMMASK_NONE;
    rtc_arm_handler_.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_arm_handler_.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    rtc_arm_handler_.AlarmDateWeekDay = week; 
    rtc_arm_handler_.Alarm = RTC_ALARM_A;     
    
    HAL_RTC_SetAlarm_IT(&rtc_handler_, &rtc_arm_handler_, RTC_FORMAT_MODE);
    
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x01, 0x02);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void rtc_driver::delay_alarm(uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint8_t alarm_week = 0;
    uint8_t alarm_hour = 0;
    uint8_t alarm_min = 0;
    uint8_t alarm_sec = 0;
    
    update();

    alarm_sec += time_.Seconds + sec;
    if(alarm_sec >= 60)
    {
        alarm_sec -= 60;
        alarm_min++;
    }
    
    alarm_min += time_.Minutes + min;
    if(alarm_min >= 60)
    {
        alarm_min -= 60;
        alarm_hour++;
    }
    
    alarm_hour += time_.Hours + hour;
    if(alarm_hour >= 24)
    {
        alarm_hour -= 24;
        alarm_week++;
    }
    
    alarm_week += date_.WeekDay + day;
    if(alarm_week > 7)
    {
        alarm_week -= 7;
    }
    
    set_alarm(alarm_week, alarm_hour, alarm_min, alarm_sec);
}

BaseType_t rtc_driver::hardware_init()
{  
    rtc_handler_.Instance = RTC;
    rtc_handler_.Init.HourFormat = RTC_HOURFORMAT_24;
    rtc_handler_.Init.AsynchPrediv = 127;
    rtc_handler_.Init.SynchPrediv = 255;
    rtc_handler_.Init.OutPut = RTC_OUTPUT_DISABLE;
    rtc_handler_.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtc_handler_.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&rtc_handler_) != HAL_OK)
        return pdFAIL;

    if(HAL_RTCEx_BKUPRead(&rtc_handler_, RTC_BKP_DR0) != RTC_SET_FLAGS)
    {
        set_time(RTC_TIME_HOUR, RTC_TIME_MINUTE, RTC_TIME_SECOND);
        set_date(RTC_DATE_YEAR, RTC_DATE_MONTH, RTC_DATE_DAY, RTC_DATE_WEEK);

        HAL_RTCEx_BKUPWrite(&rtc_handler_, RTC_BKP_DR0, RTC_SET_FLAGS);
    }
    
    return pdPASS;
}

bool rtc_driver::update(void)
{
    HAL_RTC_GetTime(&rtc_handler_, &time_, RTC_FORMAT_MODE);
    HAL_RTC_GetDate(&rtc_handler_, &date_, RTC_FORMAT_MODE);
    
    return true;
}

bool rtc_driver::test()
{
    return true;
}

extern "C"
{
    void RTC_Alarm_IRQHandler(void)
    {
      RTC_HandleTypeDef *prtc_handler = rtc_driver::get_instance()->get_rtc_handler();
        
      if(__HAL_RTC_ALARM_GET_IT(prtc_handler, RTC_IT_ALRA))
      {
        if((uint32_t)(prtc_handler->Instance->CR & RTC_IT_ALRA) != (uint32_t)RESET)
        {
          rtc_driver::get_instance()->set_alarm(true);
            
          /* Clear the Alarm interrupt pending bit */
          __HAL_RTC_ALARM_CLEAR_FLAG(prtc_handler,RTC_FLAG_ALRAF);
        }
      }
      
        /* Clear the EXTI's line Flag for RTC Alarm */
      __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();
      
      /* Change RTC state */
      prtc_handler->State = HAL_RTC_STATE_READY; 
    }
}