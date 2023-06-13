//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      sdmmc.hpp
//
//  Purpose:
//      sdcard driver interface for init, read, write.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "includes.hpp"

#define SDMMC_READ_WRITE_TIMEOUT        100
#define SDMMC_BLOCK_SIZE                512
#define SDMMC_CLOCK_DIV                 2

class sdmmc_driver
{
public:
    BaseType_t init();

    static sdmmc_driver* get_instance(){
        static sdmmc_driver instance_;
        return &instance_;
    }  
public:
    HAL_StatusTypeDef read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);
    HAL_StatusTypeDef write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);

private:
    BaseType_t hardware_init(); 
    BaseType_t test();

private:
    SD_HandleTypeDef hsd_handler_;
    uint32_t rx_data_[128];
};