/*
 * File      : template.h
 * This file is template file interface
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#ifndef _INCLUDE_LED_H
#define _INCLUDE_LED_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "../include/UsrTypeDef.h"
#include "../include/SystemConfig.h"

/**************************************************************************
* Global Macro Definition
***************************************************************************/

/**************************************************************************
* Global Type Definition
***************************************************************************/

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/

/*配置LED的驱动*/
void LedDriveInit(void);

/*释放LED应用资源*/
void LedDriverRelease(void);

/*获取LED当前的状态*/
uint8_t LedStatusRead(void);

/*修改LED的当前状态*/
void LedStatusConvert(uint8_t nLedStatus);
#endif
