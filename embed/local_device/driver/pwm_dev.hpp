//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      pwm_dev.hpp
//
//  Purpose:
//      用于处理pwm的接口
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common_unit.hpp"
#include "device_common.hpp"
#define FMT_HEADER_ONLY
#include "fmt/core.h"

#define PWM_EXPORT  "/sys/class/pwm/{0}/export"
#define PWM_ENABLE  "/sys/class/pwm/{0}/pwm0/enable"
#define PWM_PEROID  "/sys/class/pwm/{0}/pwm0/period"
#define PWM_DUTYCLE "/sys/class/pwm/{0}/pwm0/duty_cycle"

class pwm_device
{
public:
    /// \brief constructor
    pwm_device(){
    }

    /// -- destructor
    ~pwm_device(){
    }

    /// \brief init
    /// - init the device.
    /// \param DevicePath - path of the system device.
    /// \param flags - flags process the device.
    /// \return Wheather initialize is success or failed.
    virtual bool init(const std::string &DevicePath) {
        int fd, ret;
        std::string export_file;
        
        device_path_ = DevicePath;
        export_file = fmt::format(PWM_EXPORT, DevicePath);

        fd = open(export_file.c_str(), O_WRONLY);
        if (fd < 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "pwm export open failed:%s", export_file.c_str());
            return false;
        }   

        ret = write(fd, "0", strlen("0"));
        if (ret < 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "pwm export write failed");
            return -1;
        }
        close(fd);
        return true;
    }


    /// \brief release
    /// - release the device.
    virtual void release()
    {
    }

    /// \brief pwm_setup
    /// - setup pwm run state
    /// \param state - pwm run state, "1" means run.
    /// \param peroid - pwm run peroid.
    /// \param duty_cycle - pwm run cycle.
    /// \return success return 0.
    virtual int pwm_setup(int state, uint32_t peroid, uint32_t duty_cycle)
    {
        std::string pwm_state, pwm_peroid, pwm_duty;
        int fd_state, fd_peroid, fd_duty;
        int ret;
        std::string data;

        pwm_state = fmt::format(PWM_ENABLE, device_path_);
        pwm_peroid = fmt::format(PWM_PEROID, device_path_);
        pwm_duty = fmt::format(PWM_DUTYCLE, device_path_);

        fd_state = open(pwm_state.c_str(), O_RDWR);
        fd_peroid = open(pwm_peroid.c_str(), O_RDWR);
        fd_duty = open(pwm_duty.c_str(), O_RDWR);

        if (fd_state<0 || fd_peroid<0 || fd_duty<0)
        {
            ret = -1;
            PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "open failed:%s,%d,\n%s,%d,\n%s,%d", 
                    pwm_state.c_str(),
                    fd_state,
                    pwm_peroid.c_str(),
                    fd_peroid,
                    pwm_duty.c_str(),
                    fd_duty);
            goto __exit;
        }

        data = std::to_string(peroid);
        ret = write(fd_peroid, data.c_str(), data.size());
        if (ret < 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "write peroid failed:%s", 
                    data.c_str());
            goto __exit;       
        }

        data = std::to_string(duty_cycle);
        ret = write(fd_duty, data.c_str(), data.size());
        if (ret < 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "write duty failed:%s", 
                    data.c_str());
            goto __exit;       
        }

        data = std::to_string(state);
        ret = write(fd_state, data.c_str(), data.size());
        if (ret < 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "write state failed:%s", 
                    data.c_str());
            goto __exit;       
        }

        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "pwm init ok, dev:%s, state:%d, peroid:%d, duty:%d",
                device_path_.c_str(),
                state,
                peroid,
                duty_cycle);
__exit:
        close(fd_state);
        close(fd_peroid);
        close(fd_duty);
        return ret;        
    }

    uint8_t get_duty() { return duty_; }
    
private:
    /// \brief device_path_
    /// - the device path in system
    std::string device_path_;

    /// \brief duty_
    /// - the duty for pwm
    uint8_t duty_{0};
};
