
_Pragma("once")

#include "led.h"
#include "key.h"

#ifdef __cplusplus
	extern "C" {
#endif

BaseType_t driver_init();		
void set_os_on();

//if us, use loop delay, if max than 1000, use delay ms.
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
HAL_StatusTypeDef read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);
HAL_StatusTypeDef write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);
        
#ifdef __cplusplus
	}
#endif
