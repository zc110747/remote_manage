_Pragma("once")

#include "main.h"

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

#define OUTPUT_BEEP         0
#define INPUT_AP_INT        1
#define OUTPUT_DCMI_PWDN    2
#define OUTPUT_USB_PWR      3
#define INPUT_EXIO          4
#define INPUT_D_INT         5
#define OUTPUT_RS485_SEL    6
#define OUTPUT_ETH_RESET    7

#define IO_EX_OFF           0
#define IO_EX_ON            1

class i2c_driver
{
public:
    bool init();

    static i2c_driver* get_instance(){
        static i2c_driver instance_;
        return &instance_;
    }  
public:
    void set_output_io(uint8_t pin, uint8_t status); 
    bool update_output();
    io_ex_info get_ex_io() { return output_data_; }
    
private:
    void hardware_init(); 
    bool test();
    bool pcf8575_init();
   
private:
    I2C_HandleTypeDef i2c2_handler_;

    uint8_t last_data{0};
    
    io_ex_info output_data_;

};