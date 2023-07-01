#pragma once

#include "includes.hpp"
#include "driver.hpp"

#define i2c_monitor_MAX_QUEUE    32

#define I2C_EVENT_ID_WRITE      0
#define I2C_EVENT_ID_READ       1


#define OUTPUT_BEEP             0
#define INPUT_AP_INT            1
#define OUTPUT_DCMI_PWDN        2
#define OUTPUT_USB_PWR          3
#define INPUT_EXIO              4
#define INPUT_D_INT             5
#define OUTPUT_RS485_SEL        6
#define OUTPUT_ETH_RESET        7

#define IO_OFF                  0
#define IO_ON                   1

typedef union
{
    uint8_t data;
    
    struct
    {
        uint8_t beep:1;
        uint8_t ap_int:1;
        uint8_t dcmi_pwdn:1;
        uint8_t usb_pwr:1;
        uint8_t exio:1;
        uint8_t d_int:1;
        uint8_t rs485_sel:1;
        uint8_t eth_reset:1;
    }u;
}io_ex_info;

typedef struct
{
    uint8_t id;
    uint8_t data;
}i2c_event;

class i2c_monitor
{
public:  
    BaseType_t init();
    
    static i2c_monitor* get_instance()
    {
        static i2c_monitor instance_;
        return &instance_;
    }
   
    void write_io(uint8_t pin, uint8_t status);
    BaseType_t trigger(uint8_t event, uint8_t *pdata, uint8_t size);
    BaseType_t trigger_isr(uint8_t event, uint8_t *pdata, uint8_t size);

public:
    static io_ex_info *get_write_io() {
        return &write_data_;
    }    
    
    static io_ex_info *get_read_io() {
        return &read_data_;
    }   
    
private:
    static void run(void* parameter);
    
private:
    static TaskHandle_t task_handle_;
    
    static QueueHandle_t queue_;
    
    static io_ex_info read_data_;

    static  io_ex_info write_data_;
};