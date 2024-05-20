//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_manage.cpp
//
//  Purpose:
//      包含应用配置信息的文件
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "device_manage.hpp"
#include "common_unit.hpp"
#include "include/productConfig.hpp"

device_manage* device_manage::instance_pointer_ = nullptr;
device_manage* device_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) device_manage();
        if (instance_pointer_ == nullptr)
        {
            PRINT_NOW("%s:device_manage new failed\r\n", PRINT_NOW_HEAD_STR);
        }
    }
    return instance_pointer_;
}

bool device_manage::init()
{
    inter_info_.clear();
    outer_info_.clear();

    //command receive fifo
    device_cmd_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_CMD_FIFO,
                                                        S_FIFO_WORK_MODE,
                                                        FIFO_MODE_WR_CREATE);
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

    //info translate fifo
    device_info_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_INFO_FIFO,
                                                            S_FIFO_WORK_MODE,
                                                            FIFO_MODE_W_CREATE);
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

    //start thread
    std::thread(std::bind(&device_manage::run, this)).detach();
    PRINT_NOW("%s:device manage init success!\r\n", PRINT_NOW_HEAD_STR);
    return true;
}

#define MOCK_DEBUG  1
uint32_t test_start = MOCK_DEBUG;

void device_manage::update()
{
    int size;
    char buf[72];

    //device hardware protect
    auto led_ptr = driver_manage::get_instance()->get_led_zero();
    if (led_ptr->read_io_status())
    {
        inter_info_.led_io_ = led_ptr->get_io_status();
    }
    else
    {
#if MOCK_DEBUG
        inter_info_.led_io_ = 1;
#endif
    }

    auto beep_ptr = driver_manage::get_instance()->get_beep_zero();
    if (beep_ptr->read_io_status())
    {
        inter_info_.beep_io_ = beep_ptr->get_io_status();
    }
    else
    {
#if MOCK_DEBUG
        inter_info_.beep_io_ = 1;
#endif
    }

    auto ap_dev_ptr = driver_manage::get_instance()->get_ap3126_dev();
    if (ap_dev_ptr->read_info())
    {
        inter_info_.ap_info_ = ap_dev_ptr->get_info();
    }
    else
    {
#if MOCK_DEBUG
        inter_info_.ap_info_.als = test_start;
        test_start++;
        inter_info_.ap_info_.ir = test_start;
        test_start++;
        inter_info_.ap_info_.ps = test_start;
        test_start++;
#endif
    }

    auto icm_dev_ptr = driver_manage::get_instance()->get_icm20608_dev();
    if (icm_dev_ptr->read_info())
    {
        icm_dev_ptr->calculate_angle();
        inter_info_.icm_info_ = icm_dev_ptr->get_icm_info();
        inter_info_.angle_ = icm_dev_ptr->get_angle();
        //PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "Angle:%d!", icm_dev_ptr->get_angle());
    }
    else
    {
#if MOCK_DEBUG
        inter_info_.icm_info_.accel_x_act = test_start;
        test_start++;
        inter_info_.icm_info_.accel_y_act = test_start;
        test_start++;
        inter_info_.icm_info_.accel_z_act = test_start;
        test_start++;
        inter_info_.icm_info_.gyro_x_act = test_start;
        test_start++;
        inter_info_.icm_info_.gyro_y_act = test_start;
        test_start++;
        inter_info_.icm_info_.gyro_z_act = test_start;
        test_start++;
        inter_info_.angle_ = test_start;
        test_start++;
#endif
    }

    auto vf610_ptr = driver_manage::get_instance()->get_vf610_dev();
    size = vf610_ptr->read_string(buf, sizeof(buf));
    if (size > 0)
    {
        buf[size] = '\0';
        auto convert_data = atoi(buf);
        inter_info_.vf610_adc_ = (float)convert_data/4096*3.3;
    }
    else
    {
#if MOCK_DEBUG
        inter_info_.vf610_adc_ = test_start;
        test_start++;
#endif
    }

    auto hx711_ptr = driver_manage::get_instance()->get_hx711_dev();
    size = hx711_ptr->read_string(buf, sizeof(buf));
    if (size > 0)
    {
        buf[size] = '\0';
        inter_info_.hx711_ = atoi(buf);
    }
    else
    {
#if MOCK_DEBUG
        inter_info_.hx711_ = test_start;
        test_start++;
#endif
    }

    inter_info_.rtc_timer.size = 0;
    memset(inter_info_.rtc_timer.buffer, 0, STRUCT_STRING_MAX_SIZE);
    auto rtc_ptr = driver_manage::get_instance()->get_rtc_dev();
    size = rtc_ptr->update_rtc_time();
    if(size > 0)
    {
        auto timer_str = rtc_ptr->get_timer_str();
        inter_info_.rtc_timer.size = timer_str.size();
        if(timer_str.size() > 0)
        {
            memcpy(inter_info_.rtc_timer.buffer, timer_str.c_str(), 
                    timer_str.size()<STRUCT_STRING_MAX_SIZE?timer_str.size():STRUCT_STRING_MAX_SIZE);
        }
    }

    if (inter_info_ != outer_info_)
    {
        uint8_t size;

        outer_info_ = inter_info_;
        size = outer_info_.copy_to_buffer(tx_buffer_);

        device_info_fifo_point_->write(tx_buffer_, size);
        PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "update to main porcess, data:%d,%f, size:%d!", 
                                                inter_info_.hx711_, 
                                                inter_info_.icm_info_.gyro_y_act, 
                                                size);
    }
    else
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "device no change!");
    }
}

void device_manage::process_event(Event *pEvent)
{
    uint16_t id = pEvent->get_id();

    if(id != DEVICE_LOOP_EVENT)
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Device event:%d!", id);
    }

    switch (id)
    {
        case DEVICE_LOOP_EVENT:
            update();
            break;
        case DEVICE_HW_SET_EVENT:
            process_hardware(pEvent);
            break;
        case DEVICE_SYNC_EVENT:
            process_sync(pEvent);
            break;
        default:
            break;
    }
}

void device_manage::process_sync(Event *pEvent)
{
    EventBufMessage *pHardEvent = static_cast<EventBufMessage *>(pEvent);
    auto data = pHardEvent->get_data();

    uint8_t sync_internal = data.buffer[0];

    switch(sync_internal)
    {
        case 0:
            uint8_t level = data.buffer[1];
            log_manage::get_instance()->set_level((LOG_LEVEL)level);
            PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "log level:%d", level);
            break;
    }
}

void device_manage::process_hardware(Event *pEvent)
{
    EventBufMessage *pHardEvent = static_cast<EventBufMessage *>(pEvent);
    auto data = pHardEvent->get_data();

    uint8_t device = data.buffer[0];

    switch (device)
    {
        case DEVICE_LED:
            {
                uint8_t action = data.buffer[1];
                auto led_ptr = driver_manage::get_instance()->get_led_zero();
                led_ptr->write_io_status(action);
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "led process:%d, %d!", device, action);
            }
            break;
        case DEVICE_BEEP:
            {
                uint8_t action = data.buffer[1];
                auto beep_ptr = driver_manage::get_instance()->get_beep_zero();
                beep_ptr->write_io_status(action);
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "beep process:%d, %d!", device, action);
            }
            break;
        default:
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device:%d!", device);
            break;
    }
}

int device_manage::send_message(char* pEvent, int size)
{
    return device_cmd_fifo_point_->write(pEvent, size);
}

void device_manage::run()
{
    int size;

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "device_manage start!");
    time_manage::get_instance()->register_action(DEVICE_LOOP_EVENT, TIME_TICK(1000), TIME_ACTION_ALWAYS, [&](int id){
        Event event(id);
        send_message(reinterpret_cast<char *>(&event), sizeof(event));
    });

    //register action for key process
    driver_manage::get_instance()->get_key_zero()->register_func(KEY_NUM, KEY_PREES, [this](uint16_t num, uint16_t value){
        static uint8_t status = 0;

        PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "key num %d, value:%d", num, value);
        
        status = status == 0?1:0;

        EventBufMessage ebufMsg(DEVICE_HW_SET_EVENT);
        ebufMsg.get_data().buffer[0] = DEVICE_LED;
        ebufMsg.get_data().buffer[1] = status;
        send_message(reinterpret_cast<char *>(&ebufMsg), sizeof(ebufMsg));
    });

    for (;;)
    {
        size = device_cmd_fifo_point_->read(rx_buffer_, DEVICE_RX_BUFFER_SIZE);
        if (size > 0)
        {
            //add command process
            process_event(reinterpret_cast<Event *>(rx_buffer_));
        }
        else
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "device_cmd_fifo_point_ rx failed!");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}
