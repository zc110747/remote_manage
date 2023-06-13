//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      usart.hpp
//
//  Purpose:
//      usart driver interface process.
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

#include "driver.hpp"

#define USART_TRANSLATE_DELAY_TIME  (200)

class usart_driver
{
public:
    BaseType_t init(void);
    bool test(void);
    static usart_driver* get_instance(){
        static usart_driver instance_;
        return &instance_;
	}
    UART_HandleTypeDef *get_uart1(){
        return &huart1;
    }
    
    BaseType_t usart1_translate(char *ptr, uint16_t size);
    
private:
    BaseType_t hardware_init(void);
    bool is_init{false};

private:
    UART_HandleTypeDef huart1;
};
