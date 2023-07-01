//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      led.c
//
//  Purpose:
//      led driver.
//      hardware: 
//          LED0 ------------ PB0
//          LED1 ------------ PB1
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "led.h"

static void led_hardware_init(void);
static BaseType_t led_test(void);

BaseType_t led_init(void)
{
    led_hardware_init();

    led_set(LED0, LED_STATUS_OFF);

    led_test();
    
    return pdPASS;
}

void led_set(led_device dev, led_status status)
{
    GPIO_PinState state = (status == LED_STATUS_OFF)?GPIO_PIN_SET:GPIO_PIN_RESET;

    switch(dev)
    {
        case LED0:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, state);
            break;
        case LED1:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, state);
            break;
        default:
            break;
    }
}

static BaseType_t led_test(void)
{
#if LED_TEST == 1
    set(LED0, LED_STATUS_ON);
    HAL_Delay(500);
    set(LED0, LED_STATUS_OFF);
    HAL_Delay(500);
    set(LED1, LED_STATUS_ON);
    HAL_Delay(500);
    set(LED1, LED_STATUS_OFF);
    HAL_Delay(500);
#endif  
    return pdPASS;
}

static void led_hardware_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    
    /*Configure GPIO pins : PB0 PB1 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
