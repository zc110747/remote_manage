//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_process.cpp
//
//  Purpose:
//      local device process.
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <sys/utsname.h>
#include "device_process.hpp"
#include "common_unit.hpp"

#if MODULE_DEFINE_MQTT == 1
#include "mqtt_process.hpp"
#endif

#define FMT_HEADER_ONLY
#include "fmt/core.h"

device_process* device_process::instance_pointer_ = nullptr;
device_process* device_process::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) device_process();
        if (instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "device_process new error!");
        }
    }
    return instance_pointer_;
}

bool device_process::init()
{
    device_info_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_INFO_FIFO,
                                                            S_FIFO_WORK_MODE,
                                                            FIFO_MODE_R);
    if (device_info_fifo_point_ == nullptr)
    {
        PRINT_NOW("%s:fifo %s new failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_INFO_FIFO);
        return false;
    }
    if (!device_info_fifo_point_->create())
    {
        PRINT_NOW("%s:fifo %s create failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_INFO_FIFO);
        return false;
    }

    device_cmd_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_CMD_FIFO,
                                                            S_FIFO_WORK_MODE,
                                                            FIFO_MODE_W);
    if (device_cmd_fifo_point_ == nullptr)
    {
        PRINT_NOW("%s:fifo %s new failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_CMD_FIFO);
        return false;
    }
    if (!device_cmd_fifo_point_->create())
    {
        PRINT_NOW("%s:fifo %s create failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_CMD_FIFO);
        return false;
    }

    //clear thread
    std::thread(std::bind(&device_process::run, this)).detach();
    return true;
}

void device_process::run()
{
    int size;

    //init the system information
    system_info_init();

    while (1)
    {
        size = device_info_fifo_point_->read(rx_buffer_, DEVICE_RX_BUFFER_SIZE);
        if (size > 0)
        {
            //update rx buffer to internal
            info_.copy_to_device(rx_buffer_);

            //buffer to string
            update_info_string();

            #if MODULE_DEFINE_MQTT == 1
            mqtt_manage::get_instance()->mqtt_publish(info_str_);
            #endif
        }
        else
        {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "device rx failed:%d", size);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

int device_process::send_buffer(const char* ptr, int size)
{
    return device_cmd_fifo_point_->write(ptr, size);
}

int device_process::set_device(int dev, char *buf, int size)
{   
    EventBufMessage EventBuf;
    auto &data = EventBuf.get_data();

    EventBuf.set_id(DEVICE_HW_SET_EVENT);

    if(size > data.size-1)
    {
        return -1;
    }

    data.buffer[0] = dev;
    memcpy(&data.buffer[1], buf, size);
    return send_buffer(reinterpret_cast<char *>(&EventBuf), sizeof(EventBuf));
}

int device_process::sync_info(char *buf, int size)
{
    EventBufMessage EventBuf;
    auto &data = EventBuf.get_data();

    EventBuf.set_id(DEVICE_SYNC_EVENT);

    if(size > data.size)
    {
        return -1;
    }
    memcpy(data.buffer, buf, size);
    
    PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "sync_info:%d, %d", size, EventBuf.get_data().get_buffer()[0]);
    return send_buffer(reinterpret_cast<char *>(&EventBuf), sizeof(EventBuf));   
}

bool device_process::system_info_init()
{
    bool result;

    result = get_cpu_info();

    result &= get_kernel_info();

    result &= get_disk_info();

    result &= get_ram_info();

    return result;
}

#define MAX_LINE_LENGTH 512
bool device_process::get_cpu_info()
{
    bool is_success = false;
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *token;
    char *end;

    file = fopen("/proc/cpuinfo", "r");
    if (file == NULL)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "system info open failed!");
        return false;
    }

    while (fgets(line, MAX_LINE_LENGTH, file))
    {
        if (line[0] == '\n' || line[0] == '#')
        {
            continue;
        }

        token = strtok_r(line, ":", &end);
        while (token != NULL)
        {
            if (strncmp(token, "model name", strlen("model name")) == 0)
            {
                token = strtok_r(NULL, ":", &end);
                is_success = true;
                sysinfo_.cpu_info = fmt::format("{}", token);
                sysinfo_.cpu_info.pop_back();
                PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "cpu info:%s", sysinfo_.cpu_info.c_str());
                break;
            }
            token = strtok_r(NULL, ":", &end);
        }

        if (is_success)
        {
            break;
        }
    }
    fclose(file);

    return is_success;
}

bool device_process::get_kernel_info()
{
    struct utsname uname_info;
    if (uname(&uname_info) == -1)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "uname open failed!");
        return false;
    }
    sysinfo_.kernel_info = fmt::format("{0}", uname_info.release);
    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "kernel info:%s", sysinfo_.kernel_info.c_str());
    return true;
}

bool device_process::get_disk_info()
{
    struct statfs diskInfo;
    if (statfs("/", &diskInfo) < 0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "disk info read faile!");
        return false;
    }

    //单位为B, 转换为MB
    sysinfo_.disk_total = ((uint64_t)diskInfo.f_blocks*diskInfo.f_bsize)/(1024*1024);
    sysinfo_.disk_used = ((uint64_t)diskInfo.f_bavail*diskInfo.f_bsize)/(1024*1024);
    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "disk:%d, %d", sysinfo_.disk_total, sysinfo_.disk_used);
    return true;
}

bool device_process::get_ram_info()
{
    struct sysinfo info;

    if (sysinfo(&info) == -1)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "ram info read faile!");
        return -1;
    }

    sysinfo_.ram_total = info.totalram/(1024*1024);
    sysinfo_.ram_used = info.freeram/(1024*1024);

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "ram:%d, %d", sysinfo_.ram_total, sysinfo_.ram_used);
    return true;
}

void device_process::update_info_string()
{
    Json::Value root;
    
    root["command"] = COMMAND_UPDATE_LOCAL; 
    root["data"]["led"] = info_.led_io_;
    root["data"]["beep"] = info_.beep_io_;

    root["data"]["ap"]["ir"] = info_.ap_info_.ir;
    root["data"]["ap"]["als"] = info_.ap_info_.als;
    root["data"]["ap"]["ps"] = info_.ap_info_.ps;

    root["data"]["icm"]["gyro_x"] = info_.icm_info_.gyro_x_act;
    root["data"]["icm"]["gyro_y"] = info_.icm_info_.gyro_y_act;
    root["data"]["icm"]["gyro_z"] = info_.icm_info_.gyro_z_act;
    root["data"]["icm"]["accel_x"] = info_.icm_info_.accel_x_act;
    root["data"]["icm"]["accel_y"] = info_.icm_info_.accel_y_act;
    root["data"]["icm"]["accel_z"] = info_.icm_info_.accel_z_act;
    root["data"]["icm"]["temp_act"] = info_.icm_info_.temp_act;

    root["data"]["angle"] = info_.angle_;
    root["data"]["hx711"] = info_.hx711_;
    root["data"]["vf610_adc"] = info_.vf610_adc_;

    //system information
    root["data"]["sysinfo"]["cpu_info"] = sysinfo_.cpu_info;
    root["data"]["sysinfo"]["kernel_info"] = sysinfo_.kernel_info;
    root["data"]["sysinfo"]["disk_total"] = sysinfo_.disk_total;
    root["data"]["sysinfo"]["disk_used"] = sysinfo_.disk_used;
    root["data"]["sysinfo"]["ram_total"] = sysinfo_.ram_total;
    root["data"]["sysinfo"]["ram_used"] = sysinfo_.ram_used;

    info_str_ = root.toStyledString();
}
