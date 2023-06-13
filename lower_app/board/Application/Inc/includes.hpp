
#pragma once

//c++ header
#include <atomic>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>
#include <type_traits>

//stm32 hal header
#include "main.h"

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