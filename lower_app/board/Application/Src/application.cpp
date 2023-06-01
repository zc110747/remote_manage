
#include "application.hpp"
#include "logger.hpp"
#include "led.hpp"
#include "schedular.hpp"
#include "motion.hpp"


void application_init(void)
{
    //logger interface init
    logger_manage::get_instance()->init();
    
    //schedular task init
    schedular::get_instance()->init();

    //motion_manage task init
    motion_manage::get_instance()->init();
    
    //PRINT_NOW("application init\r\n");
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "application init success");
                   
    //start the rtos schedular.
    vTaskStartScheduler();
}