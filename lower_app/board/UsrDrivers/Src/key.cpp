
#include "key.hpp"
#include "includes.hpp"

extern "C" {
    
    void EXTI3_IRQHandler(void)
    {
        if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_3) != RESET)
        {
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
            KEY1::get_instance()->is_interrupt = true;
            KEY1::get_instance()->ticks = xTaskGetTickCount();
        }
    }
    
}