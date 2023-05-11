
#include "driver.hpp"
#include "sdram.hpp"
#include "led.hpp"
#include "lcd.hpp"


void driver_init()
{
	//led init
	//all io clock init in this function, so need the first execute.
	led_driver::get_instance()->init();
	
	//sdram init
	sdram_driver::get_instance()->init();
	
	//lcd init
	lcd_driver::get_instance()->init();
}

void delay_us(uint16_t times)
{
	uint16_t i, j;
	for(i=0; i<times; i++)
	{
		for(j=0; j<5; j++)
		{
			__NOP();
		}
	}
}