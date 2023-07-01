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

#include "interface.h"

#define KEY_MAX_INDEX           3

#define KEY_ON                  GPIO_PIN_RESET
#define KEY_OFF                 GPIO_PIN_SET      
#define ANTI_SHAKE_TICK         4
#define KEY_CHECK_TIMES_MS      100

typedef GPIO_PinState KEY_STATE;

#define KEY0_PORT   GPIOH
#define KEY0_PIN    GPIO_PIN_3
#define KEY1_PORT   GPIOH
#define KEY1_PIN    GPIO_PIN_2
#define KEY2_PORT   GPIOC
#define KEY2_PIN    GPIO_PIN_13

#ifdef __cplusplus
	extern "C" {
#endif
        
BaseType_t key_init(void);
GPIO_PinState key_get_value(uint8_t index);
        
#ifdef __cplusplus
	}
#endif
        