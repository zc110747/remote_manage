//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      key.c
//
//  Purpose:
//      led driver.
//      hardware: 
//          KEY0 ------------ PH3
//          KEY1 ------------ PH2
//          KEY2 ------------ PC13
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "key.h"

BaseType_t key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //clock enable
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    
    //gpio config
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    GPIO_InitStruct.Pin = KEY0_PIN;
    HAL_GPIO_Init(KEY0_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = KEY1_PIN;
    HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = KEY2_PIN;
    HAL_GPIO_Init(KEY2_PORT, &GPIO_InitStruct);
    
    return pdPASS;
}

GPIO_PinState key_get_value(uint8_t index)
{
    GPIO_PinState state;
    
    switch(index)
    {
        case 0:
            state = HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN);
            break;
        case 1:
            state = HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN);
            break;
        case 2:
            state = HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN);
            break;
    }
    
    return state;
}