//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      driver.cpp
//
//  Purpose:
//      driver defined for init.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "driver.hpp"
#include "sdram.hpp"
#include "led.hpp"
#include "lcd.hpp"
#include "usart.hpp"
#include "key.hpp"
#include "adc.hpp"
#include "rng.hpp"
#include "tpad.hpp"
#include "rtc.hpp"
#include "i2c.hpp"
#include "dac.hpp"
#include "sdmmc.hpp"
#include "spi.hpp"

std::atomic<bool> is_os_on{false};

BaseType_t driver_init(void)
{
    BaseType_t result;
    
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    //usart init
    //usart first init for logger.
    result = usart_driver::get_instance()->init();
    
    //led init
    //all io clock init in this function, so need the first execute.
    result &= led_driver::get_instance()->init();

    //sdram init
    result &= sdram_driver::get_instance()->init();

    //lcd init
    result &= lcd_driver::get_instance()->init();

    //adc init
    result &= adc_driver::get_instance()->init();

    //key0 init
    result &= KEY0::get_instance()->init();
    result &= KEY1::get_instance()->init();
    result &= KEY2::get_instance()->init();

    //rng
    result &= rng_driver::get_instance()->init();

    //tpad 
    result &= tpad_driver::get_instance()->init();
    
    //rtc
    result &= rtc_driver::get_instance()->init();
    
    //i2c
    result &= i2c_driver::get_instance()->init();
    
    //dac
    result &= dac_driver::get_instance()->init();
    
    //sdmmc
    result &= sdmmc_driver::get_instance()->init();
    
    //spi
    result &= spi_driver::get_instance()->init();
    
    return result;
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

HAL_StatusTypeDef read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    return sdmmc_driver::get_instance()->read_disk(buf, startBlocks, NumberOfBlocks);
}

HAL_StatusTypeDef write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    return sdmmc_driver::get_instance()->write_disk(buf, startBlocks, NumberOfBlocks);
}