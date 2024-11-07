/*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "common_unit.hpp"

/* ----------------------- Variables ----------------------------------------*/
#define USE_OS_QUEUE    1

/* ----------------------- Start implementation -----------------------------*/
#if USE_OS_QUEUE == 1
static EVENT::Thread_Queue<eMBEventType> queue;

BOOL xMBPortEventInit( void )
{
    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    queue.send(eEvent);
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;
    eMBEventType event;

    if( queue.receive(event, TIME_ACTION_ALWAYS) )
    {
        *eEvent = event;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
#else
static eMBEventType eQueuedEvent;
static BOOL     xEventInQueue;

BOOL
xMBPortEventInit( void )
{
    xEventInQueue = FALSE;
    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
#endif