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
void LedDriveInit(void);
uint8_t LedStatusRead(void);
void LedStatusConvert(uint8_t nLedStatus);
#endif
