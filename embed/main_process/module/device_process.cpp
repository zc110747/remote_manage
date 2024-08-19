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

            device_loop_update();

            update_device_status();
        }
        else
        {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "device rx failed:%d", size);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void device_process::device_loop_update(void)
{
    get_disk_info();

    get_ram_info();

    get_cpu_used();

    update_device_info();
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

    if (size > data.size-1)
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

    if (size > data.size)
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

CpuStats readCpuStats() {  
    std::ifstream file("/proc/stat");  
    std::string line;  
    while (std::getline(file, line)) {  
        if (line.substr(0, 3) == "cpu") {  
            return CpuStats(line);  
        }  
    }  
    throw std::runtime_error("Failed to read CPU stats");  
}  

bool device_process::get_cpu_used()
{
    static uint8_t step = 0;
    static CpuStats prevStat;
    static CpuStats curStat;
    static auto time = xGetCurrentTimes();

    if((xGetCurrentTimes() - time) > 1)
    {
        time = xGetCurrentTimes();
        if(step == 0)
        {
            prevStat = readCpuStats();
            step = 1;
        }
        else
        {
            curStat = readCpuStats();
            sysinfo_.cpu_used_precent = static_cast<uint8_t>(curStat.calculateUsage(prevStat));
            PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "cpu percent:%d", sysinfo_.cpu_used_precent);
            step = 0;
        }
    }

    return true;
}

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
                PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "cpu info:%s", sysinfo_.cpu_info.c_str());
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
    //获取内核信息
    struct utsname uname_info;
    if (uname(&uname_info) == -1)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "uname open failed!");
        return false;
    }
    sysinfo_.kernel_info = fmt::format("{0}", uname_info.release);

    //获取hostname
    std::array<char, 1024> buffer;  
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("hostname", "r"), pclose);  
    if (!pipe) {  
        throw std::runtime_error("popen() failed!");  
    }  
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {  
        size_t len = strlen(buffer.data());  
        if (len > 0 && buffer[len-1] == '\n') 
        {  
            buffer[len-1] = '\0';  
        }  
        sysinfo_.host_name = std::string(buffer.data());
        break;
    }  
    PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "kernel info:%s", sysinfo_.kernel_info.c_str());
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
    PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "disk:%d, %d", sysinfo_.disk_total, sysinfo_.disk_used);
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

    PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "ram:%d, %d", sysinfo_.ram_total, sysinfo_.ram_used);
    return true;
}

void device_process::update_device_status()
{
    Json::Value root;

    root["command"] = "rep_getstat";
    root["replay"] = "OK";

    root["data"]["key_area"]["led_status"] = info_.led_io_;
    root["data"]["key_area"]["beep_status"] = info_.beep_io_;
    root["data"]["pwm_area"]["pwm_duty"] = info_.pwm_duty_;
    
    root["data"]["time_area"]["rtc_time"] = std::string(info_.rtc_timer.buffer, info_.rtc_timer.size);

    root["data"]["status_area"]["angle"] = info_.angle_;
    root["data"]["status_area"]["temperature"] = info_.icm_info_.temp_act;
    root["data"]["status_area"]["voltage"] = info_.vf610_adc_;
    root["data"]["status_area"]["als"] = info_.ap_info_.als;

    root["data"]["ap"]["ir"] = info_.ap_info_.ir;
    root["data"]["ap"]["als"] = info_.ap_info_.als;
    root["data"]["ap"]["ps"] = info_.ap_info_.ps;

    {
        std::lock_guard<std::mutex> lock{mutex_};
        info_str_[DATA_DEVICE_STATUS] = root.toStyledString();
    }
}

void device_process::update_device_info()
{
    Json::Value root;

    root["command"] = "rep_getinfo";
    root["replay"] = "OK";

    root["data"]["hw_area"]["cpuinfo"] = sysinfo_.cpu_info;
    root["data"]["hw_area"]["cpuused"] = sysinfo_.cpu_used_precent;
    root["data"]["hw_area"]["diskused"] = sysinfo_.disk_used;
    root["data"]["hw_area"]["disktotal"] = sysinfo_.disk_total;
    root["data"]["hw_area"]["ramused"] = sysinfo_.ram_used;
    root["data"]["hw_area"]["ramtotal"] = sysinfo_.ram_total;

    root["data"]["kernel_area"]["kernelinfo"] = sysinfo_.kernel_info;
    root["data"]["kernel_area"]["hostname"] = sysinfo_.host_name;
    root["data"]["kernel_area"]["ipaddress"] = system_config::get_instance()->get_ipaddress();

        
    {
        std::lock_guard<std::mutex> lock{mutex_};
        info_str_[DATA_DEVICE_INFO] = root.toStyledString();
    }
}

std::string device_process::get_dev_info()
{
    std::string str;

    {
        std::lock_guard<std::mutex> lock{mutex_};
        str = info_str_[DATA_DEVICE_INFO];      
    }
    return str;
}

std::string device_process::get_dev_status()
{
    std::string str;

    {
        std::lock_guard<std::mutex> lock{mutex_};
        str = info_str_[DATA_DEVICE_STATUS];
    }
    return str;
}
