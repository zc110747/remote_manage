
#include "adc.hpp"
#include "includes.hpp"

bool adc_driver::init(void)
{
    hardware_init();
    
    return true;
}

void adc_driver::hardware_init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  adc1_hander_.Instance = ADC1;
  adc1_hander_.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  adc1_hander_.Init.Resolution = ADC_RESOLUTION_12B;
  adc1_hander_.Init.ScanConvMode = DISABLE;
  adc1_hander_.Init.ContinuousConvMode = DISABLE;
  adc1_hander_.Init.DiscontinuousConvMode = DISABLE;
  adc1_hander_.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adc1_hander_.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  adc1_hander_.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  adc1_hander_.Init.NbrOfConversion = 1;
  adc1_hander_.Init.DMAContinuousRequests = DISABLE;
  adc1_hander_.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&adc1_hander_) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
  if (HAL_ADC_ConfigChannel(&adc1_hander_, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

uint16_t adc_driver::get_adc_value(uint32_t channel)
{
    uint16_t value = 0;
    
    //start the adc run
    HAL_ADC_Start(&adc1_hander_);                              
    HAL_ADC_PollForConversion(&adc1_hander_, 10);     
    
    value = HAL_ADC_GetValue(&adc1_hander_);
    return value;
}

uint16_t adc_driver::get_adc_avg(uint32_t channel)
{
    uint32_t temp = 0;
    uint8_t index;
    
    for(index=0; index<ADC_AVG_TIMES; index++)
    {
        temp += get_adc_value(channel);
        vTaskDelay(1);
    }
    
    return temp/ADC_AVG_TIMES;
}