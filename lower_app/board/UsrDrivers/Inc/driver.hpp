
_Pragma("once")

#include "main.h"

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

void driver_init();		
void delay_us(uint16_t times);
void led_set(led_device dev, led_status status);
		
#ifdef __cplusplus
	}
#endif
