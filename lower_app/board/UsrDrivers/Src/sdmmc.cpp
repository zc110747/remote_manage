
#include "sdmmc.hpp"
#include "driver.hpp"

#include <string.h>

#define SDMMC_READ_WRITE_TIMEOUT        100
#define SDMMC_BLOCK_SIZE                512
#define SDMMC_CLOCK_DIV                 2

BaseType_t sdmmc_driver::init()
{
    hardware_init();
    
    #if SDMMC_TEST == 1
    test();
    #endif
    return pdPASS;
}

void sdmmc_driver::hardware_init()
{
  hsd_handler_.Instance = SDIO;
  hsd_handler_.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd_handler_.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd_handler_.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd_handler_.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd_handler_.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd_handler_.Init.ClockDiv = SDMMC_CLOCK_DIV;
  if (HAL_SD_Init(&hsd_handler_) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SD_ConfigWideBusOperation(&hsd_handler_, SDIO_BUS_WIDE_4B) != HAL_OK)
  {
    Error_Handler();
  }
}

BaseType_t sdmmc_driver::test()
{
    uint8_t buffer[512] = {0};
    char *ptr = (char *)"sdmmc test success\r\n";
    uint16_t index = 0;
    strcpy((char *)buffer, ptr);
    
    for(index=0; index<20000; index++)
    {
        if(write_disk(buffer, index, 1) == HAL_OK)
        {       
            memset(buffer, 0, 512);
            if(read_disk(buffer, index, 1) == HAL_OK)
            {
                printf("block%d:%s", index, buffer);
            }
            else
                return pdFAIL;
        }
        else
        {   
            printf("block write failed:%d", index);
            continue;
        }
    }
    
    return pdPASS;
}

HAL_StatusTypeDef sdmmc_driver::read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    HAL_StatusTypeDef status = HAL_OK;
    
    //delay to aviod read failed    
    taskENTER_CRITICAL();
    status = HAL_SD_ReadBlocks(&hsd_handler_, (uint8_t*)buf, startBlocks, NumberOfBlocks, SDMMC_READ_WRITE_TIMEOUT);
    taskEXIT_CRITICAL();
    
    while(HAL_SD_GetCardState(&hsd_handler_) != HAL_SD_CARD_TRANSFER )
    {
        HAL_Delay(1);
    }
    return status;
}

HAL_StatusTypeDef sdmmc_driver::write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    HAL_StatusTypeDef status = HAL_OK;
    
    //delay to aviod write failed
    taskENTER_CRITICAL();
    status = HAL_SD_WriteBlocks(&hsd_handler_, (uint8_t*)buf, startBlocks, NumberOfBlocks, SDMMC_READ_WRITE_TIMEOUT);
    taskEXIT_CRITICAL();
    
    while(HAL_SD_GetCardState(&hsd_handler_) != HAL_SD_CARD_TRANSFER)
    {
        HAL_Delay(1);
    }
    return status;
}