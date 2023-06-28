
#include "main.h"

extern UART_HandleTypeDef *get_uart1();
extern BaseType_t usart1_translate(char *ptr, uint16_t size);
extern BaseType_t logger_send_data(uint8_t data);

void USART1_IRQHandler(void)
{
    uint8_t rx_data;
    UART_HandleTypeDef *handle =  get_uart1();   
    
    if(__HAL_UART_GET_FLAG(handle, UART_FLAG_RXNE) != RESET)
    {
        if(HAL_UART_Receive(handle, &rx_data, 1, 100) == HAL_OK)
        {
            logger_send_data(rx_data);
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
    usart1_translate((char *)&ch, 1); 
    return ch;
}
