
#include "schedular.hpp"
#include "led.hpp"
#include "logger.hpp"
#include "ff.h"

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

BYTE work[FF_MAX_SS];

void schedular::ff_work(void)
{
    FATFS fs;
    FIL fil;
    UINT bw;  
    
    FRESULT res;
    
    res = f_mount(&fs, "1:", 1);
    if(res == FR_OK)
    {
       goto __mount;
    }
    else
    {
//        res = f_mkfs("1:", 0, work, FF_MAX_SS);
//        if(res == FR_OK)
        if(1)
        {
            res = f_mount(&fs, "1:", 1);

            if(res == FR_OK)
            {
 __mount:
                res = f_open(&fil, "1:hello.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
                if(res != FR_OK)
                {
                   PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "f_mount open failed!") 
                }
                else
                {
                    f_write(&fil, "hello world test for mmc success", 32, &bw);
                    if(bw != 0)
                    {
                        f_lseek(&fil, 0);
                        f_read(&fil, work, 32, &bw);
                        if(bw != 0)
                        {
                            work[bw] = 0;
                            PRINT_LOG(LOG_INFO, xTaskGetTickCount(),"%s", work); 
                        }
                    }
                    f_sync(&fil);
                    f_close(&fil);
                }
                f_mount(0, "1:", 0);
            }
            else
            {
               PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "f_mount failed:%d", res); 
            }
        }
        else
        {
            PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "f_mkfs failed:%d", res); 
        }
    }
}

void schedular::run(void* parameter)
{    
    //tell driver os is start.
    set_os_on();
    
    //ff_work();
    
    while(1)
    {
        //PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "LED Task Run!");
        LED0_ON;
        vTaskDelay(100);
        LED0_OFF;
        vTaskDelay(100);
        
        schedular::get_instance()->wwdg_reload();   
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
