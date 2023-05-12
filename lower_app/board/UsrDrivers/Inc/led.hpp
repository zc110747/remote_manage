
#pragma once

#include "main.h"
#include "driver.hpp"

class led_driver
{
public:
		void init(void);
		bool test(void);
		void set(led_device dev, led_status status);
		static led_driver* get_instance(){
			static led_driver driver;
			return &driver;
	}
		
private:
		void hardware_init(void);
};
