
#include "includes.hpp"
#include "driver.hpp"
#include "sdram.hpp"
#include "led.hpp"
#include "lcd.hpp"
#include "usart.hpp"
#include "key.hpp"
#include "adc.hpp"
#include "rng.hpp"
#include "tpad.hpp"

std::atomic<bool> is_os_on = false;

void driver_init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    //led init
    //all io clock init in this function, so need the first execute.
    led_driver::get_instance()->init();

    //sdram init
    sdram_driver::get_instance()->init();

    //lcd init
    lcd_driver::get_instance()->init();

    //usart init
    usart_driver::get_instance()->init();

    //adc init
    adc_driver::get_instance()->init();

    //key0 init
    KEY0::get_instance()->init();
    KEY1::get_instance()->init();
    KEY1::get_instance()->interrupt_init();
    KEY2::get_instance()->init();

    //rng
    rng_driver::get_instance()->init();

    //tpad 
    tpad_driver::get_instance()->init();
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

void set_os_on()
{
    is_os_on = true;
}

void delay_ms(uint16_t ms)
{
    if(is_os_on)
    {
        vTaskDelay(ms);
    }
    else
    {
        HAL_Delay(ms);
    }
}
