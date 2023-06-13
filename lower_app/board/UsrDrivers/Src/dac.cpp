//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      dac.cpp
//
//  Purpose:
//      dac init and set_voltage.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "dac.hpp"

BaseType_t dac_driver::init()
{
    BaseType_t result;
    
    result = hardware_init();
    
    if(result == pdPASS)
    {
        set_voltage(DAC_DEFAULT_VOL);
        
        test();
    }
    else
    {
        printf("dac_driver hardware_init failed\r\n");
    }
    return result;
}

void dac_driver::set_voltage(uint16_t mv)
{
    float adc_value;
    
    if(mv > DAC_REFERENCE_VOL)
        mv = DAC_REFERENCE_VOL;
    
    adc_value = (float)mv/DAC_REFERENCE_VOL * DAC_MAX_VALUE;
    
    HAL_DAC_Stop(&dac_handle_, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&dac_handle_, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)adc_value);
    HAL_DAC_Start(&dac_handle_, DAC_CHANNEL_1);
}

BaseType_t dac_driver::hardware_init()
{
    DAC_ChannelConfTypeDef sConfig = {0};

    dac_handle_.Instance = DAC;
    if(HAL_DAC_Init(&dac_handle_) != HAL_OK)
        return pdFAIL;

    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if(HAL_DAC_ConfigChannel(&dac_handle_, &sConfig, DAC_CHANNEL_1) != HAL_OK)
        return pdFAIL;
    
    return pdPASS;
}

BaseType_t dac_driver::test()
{
    return pdPASS;
}