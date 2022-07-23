#include "driver.hpp"

bool hardware_driver_init()
{
    ledTheOne::getInstance()->open(O_RDWR | O_NDELAY);
    beepTheOne::getInstance()->open(O_RDWR | O_NDELAY);

}

void hardware_driver_release()
{
    ledTheOne::getInstance()->release();
    beepTheOne::getInstance()->release();
}
	