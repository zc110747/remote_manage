//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     tpad.cpp
//
//  Purpose:
//     tpad key driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "tpad.hpp"
#include "driver.hpp"

/*
APB2 Timer Clock 90Mhz
Timer Ticks for one Count is 45MHZ
*/
#define TPAD_ARR_MAX_VAL                    0XFFFFFFFF
#define TPAD_TIMER_PRESCALER                1               //clock div 2

/*
capture number must in the range, otherwise is hardware problem
*/
#define TPAD_TIMES_CAPTURE_LOW              50
#define TPAD_TIMES_CAPTURE_HIGH             2500
#define TPAD_INIT_CHECK_TIMES               10
#define TPAD_CAPTURE_LOOP_TIMES             4
#define TPAD_IS_VALID_PUSH_KEY(value, no_push_value) \
    (((value)>((no_push_value)*4/3)) && ((value)<((no_push_value)*10)))
#define TPAD_IS_VALID_CAPTURE(value) \
   (((value)>=TPAD_TIMES_CAPTURE_LOW) && ((value)<=(TPAD_TIMES_CAPTURE_HIGH)))

BaseType_t tpad_driver::init(void)
{
    uint16_t buf[10];
    BaseType_t result;
    
    result = hardware_init();

    if(result == pdPASS)
    {
        /*read the capture value for no push, sort, used middle*/
        for(int i=0; i<TPAD_INIT_CHECK_TIMES;i++)
        {				 
            buf[i] = get_value();
            delay_ms(5);	    
        }
        std::sort(buf, buf+TPAD_INIT_CHECK_TIMES);
        no_push_value_ = std::accumulate(buf+2, buf+8, 0)/6;
        printf("tpad no_push_value_:%d\r\n", no_push_value_);    
    }
    else
    {
        printf("tpad_driver hardware_init failed\r\n");
    }
    return result;
}

uint8_t tpad_driver::scan_key()
{
    /*get the max value when read capture.*/
    current_value_ = get_max_value(); 

    //check wheather is valid key push value.
    if(TPAD_IS_VALID_PUSH_KEY(current_value_, no_push_value_))					 
        return 1;  

    return 0;
}	

void tpad_driver::reset()
{
    GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin = GPIO_PIN_5;            
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  
    GPIO_Initure.Pull = GPIO_PULLDOWN;        
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);	
	
    //delay to wait capture run to zero
    delay_ms(3);	

    __HAL_TIM_CLEAR_FLAG(&timer_handler_, TIM_FLAG_CC1|TIM_FLAG_UPDATE);   
    __HAL_TIM_SET_COUNTER(&timer_handler_, 0); 
    
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;      
    GPIO_Initure.Pull = GPIO_NOPULL;         
    GPIO_Initure.Alternate = GPIO_AF1_TIM2;  
    HAL_GPIO_Init(GPIOA, &GPIO_Initure); 
}

uint16_t tpad_driver::get_value(void)
{
    /*reset the pin for next capture.*/
    reset();
    
    while(__HAL_TIM_GET_FLAG(&timer_handler_, TIM_FLAG_CC1) == RESET) 
    {
        //not more than TPAD_TIMES_CAPTURE_HIGH
        if(__HAL_TIM_GET_COUNTER(&timer_handler_) > TPAD_TIMES_CAPTURE_HIGH) 
        {
            return __HAL_TIM_GET_COUNTER(&timer_handler_);
        }
    };
    return HAL_TIM_ReadCapturedValue(&timer_handler_,TIM_CHANNEL_1);
}

uint16_t tpad_driver::get_max_value()
{
    uint16_t temp; 
    uint16_t res = 0; 
    uint8_t okcnt = 0;
    uint8_t times = TPAD_CAPTURE_LOOP_TIMES;

    while(times--)
    {
        //get the capture value, store the max in res.
        temp = get_value();
        if(temp > res)
        {
            res = temp;
        }

        //key capture need in the scope, so include means valid.
        if(TPAD_IS_VALID_CAPTURE(temp))
        {
            okcnt++;
        }
    }

    if(okcnt >= TPAD_CAPTURE_LOOP_TIMES*2/3)
        return res;
    else 
        return 0;
}

bool tpad_driver::test()
{
    return true;
}

//APB1 Timer clocks 90Mhz
BaseType_t tpad_driver::hardware_init()
{
    TIM_IC_InitTypeDef TIM2_CH1Config;  

    __HAL_RCC_TIM2_CLK_ENABLE();

    timer_handler_.Instance = TIM2;                       
    timer_handler_.Init.Prescaler = TPAD_TIMER_PRESCALER;       
    timer_handler_.Init.CounterMode = TIM_COUNTERMODE_UP;    
    timer_handler_.Init.Period = TPAD_ARR_MAX_VAL;                   
    timer_handler_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&timer_handler_);

    TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;   
    TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;
    TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;         
    TIM2_CH1Config.ICFilter=0;                        
    HAL_TIM_IC_ConfigChannel(&timer_handler_, &TIM2_CH1Config, TIM_CHANNEL_1);
    HAL_TIM_IC_Start(&timer_handler_, TIM_CHANNEL_1);    
    
    return pdPASS;
}
  