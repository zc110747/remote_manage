
_Pragma("once")

#include "main.h"

#ifdef __cplusplus
	extern "C" {
#endif

typedef enum
{
    LED0 = 0,
}led_device;

//led status
typedef enum
{
    LED_STATUS_OFF = 0,
    LED_STATUS_ON,
}led_status;

BaseType_t driver_init();		

void led_set(led_device dev, led_status status);
void set_os_on();

//if us, use loop delay, if max than 1000, use delay ms.
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
HAL_StatusTypeDef read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);
HAL_StatusTypeDef write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);
#ifdef __cplusplus
	}
#endif
