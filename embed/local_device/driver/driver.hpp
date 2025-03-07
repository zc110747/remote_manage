//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      driver.hpp
//
//  Purpose:
//      驱动管理模块，用于初始化开启所有硬件使用的外设
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

#include "beep.hpp"
#include "i2c_ap.hpp"
#include "iio_device.hpp"
#include "key.hpp"
#include "led.hpp"
#include "rtc.hpp"
#include "spi_icm.hpp"
#include "pwm_dev.hpp"
#include "loopled.hpp"

class driver_manage
{
public:
    /// \brief constructor
    driver_manage() = default;

    /// - destructor, delete not allow for singleton pattern.
    ~driver_manage() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static driver_manage* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief release
    /// - This method is used to release the object.
    void release();

    /// \brief get_led_zero
    /// - This method is used to get led device point.
    /// \return the led device point.
    led_device *get_led_zero()      {return &led_zero_;}

    /// \brief get_beep_zero
    /// - This method is used to get beep device point.
    /// \return the beep device point.
    beep_device *get_beep_zero()    {return &beep_zero_;}
    
    /// \brief get_rtc_dev
    /// - This method is used to get rtc device point.
    /// \return the rtc device point.
    rtc_device *get_rtc_dev()  {return &rtc_dev_;}

    /// \brief get_key_zero
    /// - This method is used to get key device point.
    /// \return the key device point.
    key_device *get_key_zero()           {return &key_zero_;}

    /// \brief get_ap3126_dev
    /// - This method is used to get ap3126 device point.
    /// \return the i2c ap3216 device point.
    ap_device *get_ap3126_dev()     {return &ap3216_dev_;}

    /// \brief get_icm20608_dev
    /// - This method is used to get ap3126 device point.
    /// \return the spi icm20608 device point.
    icm_device *get_icm20608_dev()  {return &icm20608_dev_;}

    /// \brief get_hx711_dev
    /// - This method is used to get hx711 device point.
    /// \return the spi hx711 device point.
    iio_device *get_hx711_dev()  {return &hx711_dev_;}

    /// \brief get_vf610_dev
    /// - This method is used to get vf610 device point.
    /// \return the vf610 device point.
    iio_device *get_vf610_dev()  {return &vf610_adc_dev_;}

    /// \brief get_pwm_dev
    /// - This method is used to get pwm device point.
    /// \return the pwm device point.
    pwm_device *get_pwm_dev()  {return &pwm_dev_;}

    /// \brief get_loopled_dev
    /// - This method is used to get loopled device point.
    /// \return the loopled device point.
    loopled_device *get_loopled_dev() {return &loopled_dev_;}

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static driver_manage* instance_pointer_;

    /// \brief led_zero_
    /// - led device object.
    led_device led_zero_;

    /// \brief beep_zero_
    /// - beep device object.
    beep_device beep_zero_;

    /// \brief key_zero_
    /// - key device object.
    key_device key_zero_;

    /// \brief ap3216_dev_
    /// - i2c ap3216 device object.
    ap_device ap3216_dev_;

    /// \brief icm20608_dev_
    /// - spi icm20608 device object.
    icm_device icm20608_dev_;

    /// \brief rtc_dev_
    /// - rtc device object.
    rtc_device rtc_dev_;

    /// \brief hx711_dev_
    /// - hx711 device object.
    iio_device hx711_dev_;

    /// \brief vf610_adc_dev_
    /// - vf610 device object.
    iio_device vf610_adc_dev_;

    /// \brief pwm_dev_
    /// - pwm device object.   
    pwm_device pwm_dev_;

    /// \brief loopled_dev_
    /// - loopled device object.  
    loopled_device loopled_dev_;
};

