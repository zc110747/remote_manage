
_Pragma("once")

#include "main.h"
#include <string.h>

#ifdef __cplusplus
	extern "C" {
#endif

typedef enum
{
    LED0 = 0,
    LED1,
}led_device;

//led status
typedef enum
{
    LED_STATUS_OFF = 0,
    LED_STATUS_ON,
}led_status;

#define LED_TEST            0
#define SDRAM_TEST          0
#define LCD_TEST            1
#define UART_TEST           0


void driver_init();		

void led_set(led_device dev, led_status status);
void set_os_on();

//if us, use loop delay, if max than 1000, use delay ms.
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

#ifdef __cplusplus
	}
#endif
