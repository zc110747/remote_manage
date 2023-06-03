
#include "i2c.hpp"

#define PCF8574_ADDR 	        0x40
#define PCF8574_I2C_TIMEOUT     100
bool i2c_driver::init()
{
    hardware_init();
    
    pcf8575_init();
    return true;
}

bool i2c_driver::pcf8575_init()
{
    output_data_.data = 0xFF;
    
    if(HAL_I2C_Master_Transmit(&i2c2_handler_, PCF8574_ADDR | 0x00, &(output_data_.data), 1, PCF8574_I2C_TIMEOUT) != HAL_OK)
        return false;
    
    return true;
}

void i2c_driver::set_output_io(uint8_t pin, uint8_t status)
{
    uint8_t io_status = status==IO_EX_ON?0:1;
    
    switch(pin)
    {
        case OUTPUT_BEEP:
             output_data_.u.beep = io_status;
             break;
        
        case OUTPUT_DCMI_PWDN:
            output_data_.u.dcmi_pwdn = io_status;
            break;
        
        case OUTPUT_USB_PWR:
            output_data_.u.usb_pwr = io_status;
            break;
        
        case OUTPUT_RS485_SEL:
            output_data_.u.rs485_sel = io_status;
            break;
        
        case OUTPUT_ETH_RESET:
            output_data_.u.eth_reset = io_status;
            break;
        
        default:
            break;
    }
}

bool i2c_driver::update_output()
{
    if(last_data != output_data_.data)
    {
        if(HAL_I2C_Master_Transmit(&i2c2_handler_, PCF8574_ADDR | 0x00, &(output_data_.data), 1, PCF8574_I2C_TIMEOUT) != HAL_OK)
            return false;
        
        last_data = output_data_.data;
    }
    
    return true;
}

void i2c_driver::hardware_init()
{
    i2c2_handler_.Instance = I2C2;
    i2c2_handler_.Init.ClockSpeed = 100000;
    i2c2_handler_.Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2c2_handler_.Init.OwnAddress1 = 0;
    i2c2_handler_.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c2_handler_.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c2_handler_.Init.OwnAddress2 = 0;
    i2c2_handler_.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c2_handler_.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&i2c2_handler_) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(&i2c2_handler_, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(&i2c2_handler_, 0) != HAL_OK)
    {
        Error_Handler();
    }
}

bool i2c_driver::test()
{
    return true;
}