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
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "common_unit.hpp"

/* ----------------------- static parameter ---------------------------------*/
#define RX_BUFFER_SIZE      1024

static uint32_t tty_rx_index = 0;
static char tty_rx_buffer[RX_BUFFER_SIZE];

static uint32_t tty_tx_index = 0;
static uint8_t tty_tx_buffer[1024];
static std::thread tx_check_thread;
static std::thread tx_rx_thread;
static EVENT::semaphore tx_check_semaphore;
static tty_control port_tty;

/* ----------------------- static functions ---------------------------------*/
static BOOL prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );
static int set_opt(int nFd,  struct serial_opt *opt_info);

/* ----------------------- Start implementation -----------------------------*/
void port_tty_rx_thread()
{
    ssize_t n_size;

    while (1) {
        n_size = port_tty.read(tty_rx_buffer, RX_BUFFER_SIZE);
        if (n_size > 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "tty_rx_buffer, size:%d", n_size);
            
            tty_rx_index = 0;
            for (int index=0; index<n_size; index++)
            {
                prvvUARTRxISR();
            }
        } else if ( n_size == 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "not receive");
            continue;
        } else {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "tty failed");
        }
    }
}

void port_tty_tx_thread()
{
    while (1) {
        if( tx_check_semaphore.wait(TIME_ACTION_ALWAYS) ) {
            do {
                if (prvvUARTTxReadyISR()) {
                    port_tty.write((char *)tty_tx_buffer, tty_tx_index);
                    break;
                }
            }while(1);
        }
    }
}

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if (xTxEnable) {
        tty_tx_index = 0;
        tx_check_semaphore.signal();
    } else {
        tty_tx_index = 0;
    }
}

void
vMBPortClose( void )
{
}

static const char *get_parity_str(eMBParity eParity)
{
    if (eParity == MB_PAR_NONE) {
        return "n";
    } else if (eParity == MB_PAR_ODD) {
        return "o";
    } else {
        return "e";
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    const SerialSysConfig& config = system_config::get_instance()->get_serial_config();

    // init the tty
    if (!port_tty.init(config.dev, O_RDWR | O_NOCTTY)) {
        PRINT_NOW("%s:port_tty %s init failed\n", PRINT_NOW_HEAD_STR, config.dev.c_str());
        return FALSE;
    }

    // set the parity
    if (!port_tty.set_opt(config.baud, config.dataBits, config.parity, config.stopBits)) {
        PRINT_NOW("%s:port_tty set opt failed\n", PRINT_NOW_HEAD_STR);
        return FALSE; 
    }

    tx_check_thread = std::thread(port_tty_tx_thread);
    tx_check_thread.detach();
    tx_rx_thread = std::thread(port_tty_rx_thread);
    tx_rx_thread.detach();
    return TRUE;
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    tty_tx_buffer[tty_tx_index] = ucByte;
    tty_tx_index++;
    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = tty_rx_buffer[tty_rx_index];
    tty_rx_index++;
    return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static BOOL
prvvUARTTxReadyISR( void )
{
    return pxMBFrameCBTransmitterEmpty(  );
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void
prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}
