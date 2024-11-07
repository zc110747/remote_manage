/*
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
 * File: $Id: portserial.c,v 1.60 2019/07/11 17:04:32 flybreak $
 */

#include "port.h"
#include <string.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbtcp.h"
#include "mbconfig.h"

#include "asio_server.hpp"
#include "common_unit.hpp"

#if MB_TCP_ENABLED > 0
/* ----------------------- Defines  -----------------------------------------*/
static UCHAR rx_tcp_buffer[1024];
static int rx_tcp_size;
std::thread mb_tcp_server_thread;
static asio_server mb_asio_server;

extern void xMBPortLoopRxBuffer(CHAR *pbuffer, int len);

void mb_tcp_server_task(int port)
{
    const auto& ipaddr = system_config::get_instance()->get_ipaddress();

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "mb tcp server info:%s:%d", ipaddr.c_str(), port);

    try
    {
        mb_asio_server.init(ipaddr, std::to_string(port), [](char* ptr, int length){
            memcpy(rx_tcp_buffer, ptr, length);
            rx_tcp_size = length;
            xMBPortEventPost(EV_FRAME_RECEIVED);
        });
        mb_asio_server.run();
    }
    catch (std::exception& e)
    {
        PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "mb_tcp_server_task, Exception:%s", e.what());
    }
}

BOOL
xMBTCPPortInit(USHORT usTCPPort)
{
    mb_tcp_server_thread = std::thread(mb_tcp_server_task, usTCPPort);
    mb_tcp_server_thread.detach();

    return TRUE;
}

void
vMBTCPPortDisable(void)
{
    mb_asio_server.close_all_session();
}

BOOL
xMBTCPPortGetRequest(UCHAR **ppucMBTCPFrame, USHORT *usTCPLength)
{
    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "getrequest, %d", rx_tcp_size);
    *ppucMBTCPFrame = &rx_tcp_buffer[0];
    *usTCPLength = rx_tcp_size;
    return TRUE;
}

BOOL
xMBTCPPortSendResponse(const UCHAR *pucMBTCPFrame, USHORT usTCPLength)
{
    if (mb_asio_server.is_valid())
    {
        mb_asio_server.do_write((const char *)pucMBTCPFrame, usTCPLength);
        return TRUE;
    }

    return FALSE;    
}
#endif
