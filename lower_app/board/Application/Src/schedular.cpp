
#include "schedular.hpp"
#include "led.hpp"
#include "logger.hpp"
#include "rng.hpp"

bool schedular::init(void)
{
    BaseType_t xReturned;
    
    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    run,       /* Function that implements the task. */
                    "schedular",          /* Text name for the task. */
                    SCHEDULAR_TASK_STACK,                    /* Stack size in words, not bytes. */
                    ( void * ) NULL,    /* Parameter passed into the task. */
                    SCHEDULAR_TASK_PROITY,/* Priority at which the task is created. */
                    &task_handle_ );      /* Used to pass out the created task's handle. */
    
   //wwdg_init();    
                    
   if(xReturned == pdPASS)
       return true;
   return false;
}
    
void schedular::run(void* parameter)
{
    static uint32_t index = 0;
    while(1)
    {
        //PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "LED Task Run!");
        LED0_ON;
        vTaskDelay(100);
        LED0_OFF;
        vTaskDelay(100);
        
        schedular::get_instance()->wwdg_reload();   

        if(index >= 10)
        {
            index = 0;
            PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "Rng value:%u", 
            rng_driver::get_instance()->get_value());      
        }
       
       index++;
    }    
}

void schedular::wwdg_reload(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

//wwdg clock used LSI = 32khz
//times = 4065* 1/(32khz/32) = 4s
void schedular::wwdg_init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = 4095;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        Error_Handler();
    }
}
