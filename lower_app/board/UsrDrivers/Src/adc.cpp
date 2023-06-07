
#include "adc.hpp"
#include "includes.hpp"

bool adc_driver::init(void)
{
    hardware_init();
    
    return true;
}

void adc_driver::hardware_init(void)
{
    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
     */
    adc1_hander_.Instance=ADC1;
    adc1_hander_.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
    adc1_hander_.Init.Resolution=ADC_RESOLUTION_12B;             //12位模式
    adc1_hander_.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
    adc1_hander_.Init.ScanConvMode=DISABLE;                      //非扫描模式
    adc1_hander_.Init.EOCSelection=DISABLE;                      //关闭EOC中断
    adc1_hander_.Init.ContinuousConvMode=DISABLE;                //关闭连续转换
    adc1_hander_.Init.NbrOfConversion=1;                         //1个转换在规则序列中 也就是只转换规则序列1 
    adc1_hander_.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续采样模式
    adc1_hander_.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
    adc1_hander_.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
    adc1_hander_.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
    adc1_hander_.Init.DMAContinuousRequests=DISABLE;             //关闭DMA请求
    HAL_ADC_Init(&adc1_hander_);                                 //初始化 
    if (HAL_ADC_Init(&adc1_hander_) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
     */

}

uint16_t adc_driver::get_adc_value(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    sConfig.Offset = 0;
    HAL_ADC_ConfigChannel(&adc1_hander_, &sConfig);
    
    //start the adc run
    HAL_ADC_Start(&adc1_hander_);                              
    HAL_ADC_PollForConversion(&adc1_hander_, 10);     
    
    return HAL_ADC_GetValue(&adc1_hander_);
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