//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      spi.cpp
//
//  Purpose:
//      spi driver for flash.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "spi.hpp"
#include "logger.hpp"

#define SPI_RW_TIMEOUT  20

BaseType_t spi_driver::init()
{
    BaseType_t result;
    
    result = hardware_init();
    if(result == pdPASS)
    {
        chip_type_update();
       
        test();
    }
    return result;
}

BaseType_t spi_driver::hardware_init()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOF_CLK_ENABLE();
  
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET);
  
  /*Configure GPIO pin : PF6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  spi_handler_.Instance = SPI5;
  spi_handler_.Init.Mode = SPI_MODE_MASTER;
  spi_handler_.Init.Direction = SPI_DIRECTION_2LINES;
  spi_handler_.Init.DataSize = SPI_DATASIZE_8BIT;
  spi_handler_.Init.CLKPolarity = SPI_POLARITY_HIGH;
  spi_handler_.Init.CLKPhase = SPI_PHASE_2EDGE;
  spi_handler_.Init.NSS = SPI_NSS_SOFT;
  spi_handler_.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  spi_handler_.Init.FirstBit = SPI_FIRSTBIT_MSB;
  spi_handler_.Init.TIMode = SPI_TIMODE_DISABLE;
  spi_handler_.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  spi_handler_.Init.CRCPolynomial = 10;
  
  if (HAL_SPI_Init(&spi_handler_) != HAL_OK)
    return pdFAIL;
  
  return pdPASS;
}

void spi_driver::chip_type_update()
{
    uint16_t id;
    
    id = read_id();

    switch(id)
    {
        case CHIP_ID_Q8:
            type_ = w25_q8;
            break;
        case CHIP_ID_Q16:
            type_ = w25_q16;
            break;
        case CHIP_ID_Q32:
            type_ = w25_q32;
            break;
        case CHIP_ID_Q64:
            type_ = w25_q64;
            break;
        case CHIP_ID_Q128:
            type_ = w25_q128;
            break;
        case CHIP_ID_Q256:
            type_ = w25_q256;
            break;        
    }
    
    printf("spi id:0x%x, type:%d\r\n", id, type_); 
}

uint8_t spi_driver::read_write_byte(uint8_t data)
{
    uint8_t rx_data;
    
    HAL_SPI_TransmitReceive(&spi_handler_,&data, &rx_data, 1, SPI_RW_TIMEOUT); 
    
    return rx_data;
}

//read ChipID				   
//0XEF13,means W25Q80  
//0XEF14,means W25Q16    
//0XEF15,means W25Q32  
//0XEF16,means W25Q64 
//0XEF17,means W25Q128 	  
//0XEF18,means W25Q256
uint16_t spi_driver::read_id(void)
{
    uint16_t id = 0;
    
    SPI_CS_ON;
    read_write_byte(0x90);
    read_write_byte(0x00);
    read_write_byte(0x00);
    read_write_byte(0x00);
    id = read_write_byte(0xff)<<8;
    id |= read_write_byte(0xff);
    SPI_CS_OFF; 
    
    return id;
}

BaseType_t spi_driver::test()
{
    return pdPASS;
}