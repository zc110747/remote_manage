
#include "logger.hpp"
#include "application.hpp"
#include "usart.hpp"

/// \brief memoryBuffer
/// - memory buffer cache
static char memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE+1];
std::atomic<bool> logger_manage::thread_work_ = {false};

QueueHandle_t logger_manage::tx_queue_ = nullptr;
QueueHandle_t logger_manage::rx_queue_ = nullptr;

bool logger_manage::init()
{
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    bool ret = true;
    
    memory_start_pointer_ = memoryBuffer;
    memory_end_pointer_ = &memoryBuffer[LOGGER_MESSAGE_BUFFER_SIZE];

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(logger_rx_run, "looger rx run", LOGGER_RX_TASK_STACK,                    
                    ( void * ) NULL, LOGGER_RX_TASK_PROITY, &xHandle );     
    if(xReturned != pdPASS)
        ret = false;
    
    xReturned = xTaskCreate(logger_tx_run, "looger tx run", LOGGER_TX_TASK_STACK,                    
                    ( void * ) NULL, LOGGER_TX_TASK_PROITY, &xHandle );     
    if(xReturned != pdPASS)
        ret = false;
    
    mutex_ = xSemaphoreCreateMutex();
    tx_queue_ = xQueueCreate(LOGGER_TX_QUEUE_NUM, sizeof(LOG_MESSAGE));
    rx_queue_ = xQueueCreate(LOGGER_TX_QUEUE_NUM, sizeof(uint8_t));
    
    if(mutex_ == NULL 
    || tx_queue_ == NULL
    || rx_queue_ == NULL)
    {
        ret = false;
    }
    
    if(!ret)
    {
        PRINT_NOW("logger manage init failed!\r\n");
    }
    return ret; 
}

char *logger_manage::get_memory_buffer_pointer(uint16_t size)
{
    char *pCurrentMemBuffer;

    pCurrentMemBuffer = memory_start_pointer_;
    memory_start_pointer_ = pCurrentMemBuffer+size;
	if(memory_start_pointer_ >  memory_end_pointer_)
	{
		pCurrentMemBuffer = memoryBuffer;
		memory_start_pointer_ = pCurrentMemBuffer + size;
	}
	return(pCurrentMemBuffer);
}

int logger_manage::print_log(LOG_LEVEL level, uint32_t time, const char* fmt, ...)
{
    int len, bufferlen;
    char *pbuf, *pstart;
    bool is_thread_work = false;

    if(level < log_level_)
        return 0;

    is_thread_work = thread_work_;

    if( xSemaphoreTake( mutex_, ( TickType_t ) 10 ) == pdTRUE )
    {
        pstart = get_memory_buffer_pointer(LOGGER_MAX_BUFFER_SIZE);
        len = LOGGER_MAX_BUFFER_SIZE;
        bufferlen = len - 1;
        pbuf = pstart;
        logger_message_.length = 0;
        logger_message_.ptr = pstart;

        len = snprintf(pbuf, bufferlen, "LogLevel:%d time:%u info:",level, time);
        if((len<=0) || (len>=bufferlen))
        {
            xSemaphoreGive(mutex_);
            return 0;
        }

        logger_message_.length += len;
        pbuf = &pbuf[len];
        bufferlen -= len;
        
        va_list	valist;
        va_start(valist, fmt);
        len = vsnprintf(pbuf, bufferlen, fmt, valist);
        va_end(valist);
        xSemaphoreGive(mutex_);

        if((len<=0) || (len>=bufferlen))
        {
            return 0;
        }
        
        logger_message_.length += len;
        pbuf = &pbuf[len];
        bufferlen -= len;

        if(bufferlen < 3)
        {
            return 0;
        }
        
        pbuf[0] = '\r';
        pbuf[1] = '\n';
        logger_message_.length += 2;

        if(is_thread_work)
        {
            put_string();
        }
        else
        {
            pbuf[2] = '\0';
            printf("%s", logger_message_.ptr);
        }

    }
    
    return  logger_message_.length;
}

bool logger_manage::put_string(void)
{
    if(xQueueSend(tx_queue_, &logger_message_, (portTickType)1) != pdTRUE)
        return false;
    return true;
}

BaseType_t logger_manage::send_data(uint8_t data)
{
    BaseType_t xReturn;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xReturn = xQueueSendFromISR(rx_queue_, &data, &xHigherPriorityTaskWoken );

    if( xHigherPriorityTaskWoken )
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    
    return xReturn;
}

void logger_manage::logger_rx_run(void *parameter)
{
    uint8_t data;
    uint8_t buffer[256];
    uint8_t index = 0;
    
    while(1)
    {
        if (xQueueReceive(rx_queue_, &data, portMAX_DELAY) == pdPASS)
        {
            if(data != '\n')
            {
                buffer[index++] = data;
            }
            else
            {
                buffer[index] = '\0';
                PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "%s", buffer);
                index = 0;
            }
        }
    }
}

void logger_manage::logger_tx_run(void *parameter)
{
    LOG_MESSAGE msg;
    thread_work_ = true;

    while(1)
    {
        if (xQueueReceive(tx_queue_, &msg, portMAX_DELAY) == pdPASS)
        {
            usart_driver::get_instance()->usart1_translate(msg.ptr, msg.length);  
            vTaskDelay(1);
        }
    }
}

