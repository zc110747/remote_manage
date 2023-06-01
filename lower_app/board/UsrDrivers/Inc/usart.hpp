
#pragma once

#include "main.h"
#include "driver.hpp"

#define USART_TRANSLATE_DELAY_TIME  (200)

class usart_driver
{
public:
    void init(void);
    bool test(void);
    static usart_driver* get_instance(){
        static usart_driver instance_;
        return &instance_;
	}
    UART_HandleTypeDef *get_uart1(){
        return &huart1;
    }
    
    HAL_StatusTypeDef usart1_translate(char *ptr, uint16_t size);
    
private:
    void hardware_init(void);

private:
    UART_HandleTypeDef huart1;
};
