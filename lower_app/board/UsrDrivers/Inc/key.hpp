//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      key.hpp
//
//  Purpose:
//      key driver interface use gpio.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
_Pragma("once");

#include "includes.hpp"

#define KEY_MAX_INDEX           3

#define KEY_ON                  GPIO_PIN_RESET
#define KEY_OFF                 GPIO_PIN_SET      
#define ANTI_SHAKE_TICK         4
#define KEY_CHECK_TIMES_MS      100

typedef GPIO_PinState KEY_STATE;

template<uint8_t KeyIndex, typename U = typename std::enable_if<(KeyIndex < 3), bool>::type>
class key
{
public:
    BaseType_t init(void)
    {
        /*Configure GPIO pins : PH2 PH3 */
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        GPIO_InitStruct.Pin = GPIO_Pin;
        GPIO_InitStruct.Mode = GPIO_Mode;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIO_Port, &GPIO_InitStruct);

        return pdPASS;
    }
    
    void interrupt_init(void)
    {
        if(KeyIndex == 0)
        {
            HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(EXTI3_IRQn);
        }
        else if(KeyIndex == 1)
        {
            HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(EXTI2_IRQn);
        }
        else
        {
            HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); 
        }
    }
    
    GPIO_PinState get_value()
    {
        return HAL_GPIO_ReadPin(GPIO_Port, GPIO_Pin);
    }

    static key* get_instance()
    {
        static key instance_;
        return &instance_;
    }

public:
     std::atomic<uint32_t> ticks{false};

private:
    uint32_t GPIO_Pin;
    uint32_t GPIO_Mode;
    GPIO_TypeDef *GPIO_Port{nullptr};


private:
    constexpr key()
    {
        GPIO_Mode = GPIO_MODE_INPUT;
        if(KeyIndex == 0)
        {
            GPIO_Pin = GPIO_PIN_3;
            GPIO_Port = GPIOH; 
        }
        else if(KeyIndex == 1)
        {
            GPIO_Pin = GPIO_PIN_2;
            GPIO_Port = GPIOH;
        }
        else
        {
            GPIO_Pin = GPIO_PIN_13;
            GPIO_Port = GPIOC;
        }
    }
    
};

using KEY0 = key<0>;
using KEY1 = key<1>;
using KEY2 = key<2>;