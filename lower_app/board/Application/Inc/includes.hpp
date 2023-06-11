
#pragma once

//c++ interface
#include <atomic>
#include <stdio.h>
#include <stdarg.h>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>

//stm32 hal interface
#include "stm32f4xx_hal.h"

//freertos interface
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "message_buffer.h"

//task define
#define LOGGER_RX_TASK_STACK        1024   
#define LOGGER_TX_TASK_STACK        1024
#define SCHEDULAR_TASK_STACK        2048
#define MONITOR_TASK_STACK           512
#define I2C_MONITOR_TASK_STACK       512

#define LOGGER_RX_TASK_PROITY       (tskIDLE_PRIORITY+1)
#define LOGGER_TX_TASK_PROITY       (tskIDLE_PRIORITY+1)
#define SCHEDULAR_TASK_PROITY       (tskIDLE_PRIORITY)
#define MONITOR_TASK_PROITY         (tskIDLE_PRIORITY+2)
#define I2C_MONITOR_TASK_PROITY     (tskIDLE_PRIORITY+3)