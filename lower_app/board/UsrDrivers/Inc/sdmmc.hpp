_Pragma("once")

#include "main.h"
#include "includes.hpp"

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
    void hardware_init(); 
    BaseType_t test();

private:
    SD_HandleTypeDef hsd_handler_;
    uint32_t rx_data_[128];
};