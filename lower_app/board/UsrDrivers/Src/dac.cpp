
#include "dac.hpp"

BaseType_t dac_driver::init()
{
    hardware_init();
    
    test();
    return pdPASS;
}

void dac_driver::set_voltage(uint16_t mv)
{
    float adc_value;
    
    if(mv > 3300)
        mv = 3300;
    
    adc_value = (float)mv/3300 * 4096;
    
    HAL_DAC_Stop(&dac_handle_, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&dac_handle_, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)adc_value);
    HAL_DAC_Start(&dac_handle_, DAC_CHANNEL_1);
}

void dac_driver::hardware_init()
{
    DAC_ChannelConfTypeDef sConfig = {0};

    dac_handle_.Instance = DAC;
    HAL_DAC_Init(&dac_handle_);

    /** DAC channel OUT1 config
    */
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&dac_handle_, &sConfig, DAC_CHANNEL_1);
}

BaseType_t dac_driver::test()
{
    set_voltage(2500);
    return pdPASS;
}