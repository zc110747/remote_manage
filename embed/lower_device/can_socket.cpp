//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      can_socket.hpp
//
//  Purpose:
//      CAN通讯管理模块，应用处理
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "can_socket.hpp"

bool can_socket::init(void)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    socket_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd_ == -1) {
        LOG_ERROR(xGetCurrentTimes(), "can socket error!");
        return false;
    }

    strcpy(ifr.ifr_name, interface_.c_str());
    if (ioctl(socket_fd_, SIOCGIFINDEX, &ifr) == -1) {
        LOG_ERROR(xGetCurrentTimes(), "can socket ioctl error!");
        return false;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_fd_, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        LOG_ERROR(xGetCurrentTimes(), "can socket bind error!");
        close(socket_fd_);
        return false;
    }

    std::thread(std::bind(&can_socket::run, this)).detach();
    return true;
}

void can_socket::run(void) {
    struct can_frame frame;
    socklen_t len = sizeof(frame);

    while (true) {
        if (recv(socket_fd_, &frame, len, 0) == -1) {
            LOG_ERROR(xGetCurrentTimes(), "can socket recv error!");
            continue;
        }
        // 处理接收到的CAN帧
    }
}

bool can_socket::write_buffer(uint8_t *buffer, uint32_t size)
{
    if (socket_fd_ == -1 
    || buffer == nullptr
    || size == 0)
    {
        LOG_ERROR(xGetCurrentTimes(), "can socket can not write!");
        return false;
    }

    size_t package_nums = size/CAN_MAX_DLEN;
    size_t offset_nums = size%CAN_MAX_DLEN;

    return true;
}