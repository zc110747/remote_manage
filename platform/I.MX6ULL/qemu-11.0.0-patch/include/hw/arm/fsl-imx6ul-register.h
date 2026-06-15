#ifndef FSL_IMX6UL_REGISTER_H
#define FSL_IMX6UL_REGISTER_H

#include "hw/arm/fsl-imx6ul.h"

#define IRQ_LINE_NUMS               4
#define KEY_IRQ_LINE                0
#define AP3216_IRQ_LINE             1
#define PCF8563_IRQ_LINE            2
#define GT9147_IRQ_LINE             3

void fsl_imx6ul_device_register(DeviceState *dev, FslIMX6ULState *s);
void imx6ul_gpio_irq_set(int pin, int level);
#endif
