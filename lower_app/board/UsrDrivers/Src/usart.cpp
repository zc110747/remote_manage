//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      usart.cpp
//
//  Purpose:
//      usart driver interrupt rx and normal tx.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "usart.hpp"
#include "logger.hpp"

BaseType_t usart_driver::init(void)
{
    BaseType_t result;
    
    //device initialize
    result  = hardware_init();
    if(result == pdPASS)
    {
        is_init = true;

        test();
    }
    
    return result;
}

BaseType_t usart_driver::usart1_translate(char *ptr, uint16_t size)
{
    BaseType_t result = pdPASS;

    if(!is_init)
    {
        return pdFAIL;
    }

    if(HAL_UART_Transmit(&huart1, (uint8_t *)ptr, size, USART_TRANSLATE_DELAY_TIME) != HAL_OK)
    {
        result = pdFAIL;
    }
    return result;
}
    
bool usart_driver::test(void)
{
#if UART_TEST == 1
    usart1_translate((char *)"hello world\r\n", strlen("hello world\r\n"));
    
    printf("This is for hello world test:%d\r\n", 1);
#endif
    return true;
}


BaseType_t usart_driver::hardware_init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
        return pdFAIL;
    
    //start usart1 interrupt.
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    HAL_NVIC_EnableIRQ(USART1_IRQn);			
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);	
    
    return pdPASS;
}

extern "C" 
{   
    void USART1_IRQHandler(void)
    {
        uint8_t rx_data;
        UART_HandleTypeDef *handle =  usart_driver::get_instance()->get_uart1();   
        
        if(__HAL_UART_GET_FLAG(handle, UART_FLAG_RXNE) != RESET)
        {
            if(HAL_UART_Receive(handle, &rx_data, 1, 100) == HAL_OK)
            {
                logger_manage::get_instance()->send_data(rx_data);
            }
        }
    }
    
    //define fputc used for printf remap.
    FILE __stdout;   
    void _sys_exit(int x) 
    { 
        x = x; 
    } 
    int fputc(int ch, FILE *f)
    { 	
        usart_driver::get_instance()->usart1_translate((char *)&ch, 1); 
        return ch;
    }

}