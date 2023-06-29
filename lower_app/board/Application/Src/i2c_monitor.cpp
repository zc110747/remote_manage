#include "i2c_monitor.hpp"
#include "i2c.hpp"
#include "logger.hpp"


TaskHandle_t i2c_monitor::task_handle_{nullptr};
QueueHandle_t i2c_monitor::queue_{nullptr};
io_ex_info i2c_monitor::read_data_{0x00};
io_ex_info i2c_monitor::write_data_{0xff};
    
BaseType_t i2c_monitor::init()
{
    BaseType_t xReturn;
    
    xReturn = xTaskCreate(
                    run,      
                    "i2c_monitor",        
                    I2C_MONITOR_TASK_STACK,              
                    ( void * ) NULL,   
                    I2C_MONITOR_TASK_PROITY,
                    &task_handle_ );      
  
    queue_ = xQueueCreate(i2c_monitor_MAX_QUEUE, sizeof(i2c_event));
    if(queue_ == nullptr)
    {
        xReturn = pdFAIL;
    }
                    
    return xReturn;
}

BaseType_t i2c_monitor::trigger(uint8_t event, uint8_t *pdata, uint8_t size)
{
    i2c_event xdata;
    
    xdata.id = event;
    if(size > 0)
    {
        xdata.data = pdata[0];
    }
    return xQueueSend(queue_, &xdata, (TickType_t)1);
}

BaseType_t i2c_monitor::trigger_isr(uint8_t event, uint8_t *pdata, uint8_t size)
{
    i2c_event xdata;
    BaseType_t xReturn;
    portBASE_TYPE xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;
    
    xdata.id = event;
    if(size > 0)
    {
        xdata.data = pdata[0];
    }
    
    /*send message*/
	xReturn = xQueueSendFromISR(queue_, &xdata, &xHigherPriorityTaskWoken);

	if(xHigherPriorityTaskWoken){
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	return xReturn;
}

void i2c_monitor::write_io(uint8_t pin, uint8_t status)
{
    uint8_t io_status = status == IO_ON?0:1;
    
    portENTER_CRITICAL();
    switch(pin)
    {
        case OUTPUT_BEEP:
            write_data_.u.beep = io_status;
            break;

        case OUTPUT_DCMI_PWDN:
            write_data_.u.dcmi_pwdn = io_status;
            break;

        case OUTPUT_USB_PWR:
            write_data_.u.usb_pwr = io_status;
            break;

        case OUTPUT_RS485_SEL:
            write_data_.u.rs485_sel = io_status;
            break;

        case OUTPUT_ETH_RESET:
            write_data_.u.eth_reset = io_status;
            break;
        
        default:
            break;
    }
    portEXIT_CRITICAL();
    
    trigger(I2C_EVENT_ID_WRITE, nullptr, 0);
}

void i2c_monitor::run(void* parameter)
{
    i2c_event event;
    
    while(1)
    {
        if(xQueueReceive(queue_, &event, portMAX_DELAY) == pdPASS)
        {
            if(event.id == I2C_EVENT_ID_WRITE)
            {
                i2c_driver::get_instance()->write_i2c(PCF8574_ADDR, write_data_.data);
            }
            else if(event.id == I2C_EVENT_ID_READ)
            {
                uint8_t io_read;
                
                //if read, dealy 5ms to wait io on.
                vTaskDelay(5);
                
                if(i2c_driver::get_instance()->read_i2c(PCF8574_ADDR, &io_read) == pdPASS)
                {
                    read_data_.data = io_read;
                    PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "i2c read:0x%x!", io_read);
                }
                else
                {
                   PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "i2c read failed!");
                }
            }
            else
            {
                //do nothing
            }
        }
    }
}

extern "C"
{
    void EXTI15_10_IRQHandler(void)
    {
        if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
        {
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
            
            i2c_monitor::get_instance()->trigger_isr(I2C_EVENT_ID_READ, nullptr, 0);
        }
    }
}
