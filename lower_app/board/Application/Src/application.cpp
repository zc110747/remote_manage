
#include "application.hpp"
#include "logger.hpp"
#include "led.hpp"
#include "schedular.hpp"
#include "monitor.hpp"
#include "i2c_monitor.hpp"
#include "ff.h"

BYTE work[FF_MAX_SS];

void fatfs_app(void)
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
        res = f_mkfs("1:", 0, work, FF_MAX_SS);
        if(res == FR_OK)
        {
            res = f_mount(&fs, "1:", 1);

            if(res == FR_OK)
            {
 __mount:
                res = f_open(&fil, "1:hello.txt", FA_READ | FA_WRITE);
                if(res != FR_OK)
                {
                   PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "f_mount open failed!") 
                }
                else
                {
                    f_read(&fil, work, 64, &bw);
                    if(bw != 0)
                    {
                        work[bw] = 0;
                        PRINT_LOG(LOG_INFO, xTaskGetTickCount(),"%s", work); 
                    }
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

void application_init(void)
{
    //test fatfs application    
    fatfs_app();
    
    //logger interface init
    logger_manage::get_instance()->init();
    
    //schedular task init
    schedular::get_instance()->init();

    //motion_manage task init
    monitor_manage::get_instance()->init();
        
    //i2c motion key and output
    i2c_monitor::get_instance()->init();
    
    //PRINT_NOW("application init\r\n");
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "application init success");
                   

    //start the rtos schedular.
    vTaskStartScheduler();
}

