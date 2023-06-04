
#include "application.hpp"
#include "logger.hpp"
#include "led.hpp"
#include "schedular.hpp"
#include "monitor.hpp"
#include "i2c_monitor.hpp"


void application_init(void)
{
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