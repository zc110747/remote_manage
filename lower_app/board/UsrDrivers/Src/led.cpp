
#include "led.hpp"
#include "driver.hpp"


void led_driver::init(void)
{
    //device initialize
    hardware_init();

    set(LED0, LED_STATUS_OFF);
    //set(LED1, LED_STATUS_OFF);

    #if LED_TEST == 1
    test();
    #endif
}

void led_driver::hardware_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    /*Configure GPIO pins : PB0 PB1 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

bool led_driver::test(void)
{
    set(LED0, LED_STATUS_ON);
    HAL_Delay(1000);
    set(LED0, LED_STATUS_OFF);
    HAL_Delay(1000);

//    set(LED1, LED_STATUS_ON);
//    HAL_Delay(1000);
//    set(LED1, LED_STATUS_OFF);
//    HAL_Delay(1000);
    return true;
}

void led_driver::set(led_device dev, led_status status)
{
    GPIO_PinState state = (status == LED_STATUS_OFF)?GPIO_PIN_SET:GPIO_PIN_RESET;

    if(dev == LED0)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, state);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, state);		
    }
}

void led_set(led_device dev, led_status status)
{
    led_driver::get_instance()->set(dev, status);
}