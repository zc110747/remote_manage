//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      fsl-imx6ul-register.c
//
//  Purpose:
//      提供线程/进程间通讯的数据结构，需要满足copyable
//      按键key: GPIO1_18
//      
//
// Author:
//     @微信公众号 <嵌入式技术总结>
//
//  Assumptions:
//
//  Revision History:
//      12/06/2026   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/arm/fsl-imx6ul-register.h"
#include "hw/misc/unimp.h"
#include "hw/usb/imx-usb-phy.h"
#include "hw/core/boards.h"
#include "system/system.h"
#include "qemu/error-report.h"
#include "qemu/module.h"
#include "target/arm/cpu-qom.h"
#include "hw/i2c/i2c.h"
#include "hw/core/qdev.h"
#include "qemu/log.h"

// internal defined
#define KEY_TIMER_DEBUG         0
#define LOGGER_DEBUG            1
#define FSL_REGISTER_VERSION    "1.0.0.6"

#if LOGGER_DEBUG == 1
#define PRINT_LOG   printf
#else
#define PRINT_LOG(...)
#endif

// internal variable
typedef struct fsl_device_info
{
    qemu_irq gpio_irq[IRQ_LINE_NUMS];
#if KEY_TIMER_DEBUG == 1
    QEMUTimer *press_timer;
    QEMUTimer *release_timer;
#endif
    
    MemoryRegion iomux;
    uint32_t iomux_regs[FSL_IMX6UL_IOMUXC_SIZE/4];
}FSL_DEVICE_INFO;
static FSL_DEVICE_INFO g_device_info = {0};

// internal function prototype
static void i2c_device_register(FslIMX6ULState *s);
static void key_gpio_register(FslIMX6ULState *s);
static void iomuxc_register(DeviceState *dev, FslIMX6ULState *s);
static void ecspi_device_register(FslIMX6ULState *s);

// global function
void fsl_imx6ul_device_register(DeviceState *dev, FslIMX6ULState *s)
{
    PRINT_LOG("fsl version %s register.\n", FSL_REGISTER_VERSION);
    
    // 注册iomuxc区域读写
    iomuxc_register(dev, s);

    // i2c器件注册
    i2c_device_register(s);

    // 注册gpio interrupt设备，按键输入
    key_gpio_register(s);

    // ecspi设备注册
    ecspi_device_register(s);
}

// 命令: 设置gpio interrupt level
void imx6ul_gpio_irq_set(int pin, int level)
{   
    if (pin == KEY_IRQ_LINE) {
        if (g_device_info.gpio_irq[KEY_IRQ_LINE])
            qemu_set_irq(g_device_info.gpio_irq[KEY_IRQ_LINE], level);
    } else if (pin == AP3216_IRQ_LINE) {
        if (g_device_info.gpio_irq[AP3216_IRQ_LINE])
            qemu_set_irq(g_device_info.gpio_irq[AP3216_IRQ_LINE], level);
    } else if (pin == PCF8563_IRQ_LINE) {
        if (g_device_info.gpio_irq[PCF8563_IRQ_LINE])
            qemu_set_irq(g_device_info.gpio_irq[PCF8563_IRQ_LINE], level);
    } else if (pin == GT9147_IRQ_LINE) {
        if (g_device_info.gpio_irq[GT9147_IRQ_LINE]) {
            fprintf(stderr, "imx6ul_gpio_irq_set pin %d:%d!\n", g_device_info.gpio_irq[GT9147_IRQ_LINE]->n, level);
            qemu_set_irq(g_device_info.gpio_irq[GT9147_IRQ_LINE], level);
        }
    } else {
        fprintf(stderr, "imx6ul_gpio_irq_set pin %d invalid!\n", pin);
    }
}

// internal function
static void i2c_device_register(FslIMX6ULState *s)
{
    // 注册i2c设备
    if (!s->i2c[0].bus
    || !s->i2c[1].bus) {
        return;
    }

    i2c_slave_create_simple(
            I2C_BUS(s->i2c[0].bus),
            "ap3216",
            0x1e);
    fprintf(stderr, "AP3216 device create!\n");

    i2c_slave_create_simple(
            I2C_BUS(s->i2c[1].bus),
            "pcf8563",
            0x51);
    fprintf(stderr, "PCF8563 device create!\n");

    
    i2c_slave_create_simple(
            I2C_BUS(s->i2c[1].bus),
            "gt9147",
            0x14);
    fprintf(stderr, "GT9147 device create!\n");
}

#if KEY_TIMER_DEBUG
static void key_release_cb(void *opaque)
{
    imx6ul_gpio_irq_set(KEY_IRQ_LINE, 0);

    timer_mod(g_device_info.press_timer,
              qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 1000);
}

static void key_press_cb(void *opaque)
{ 
    imx6ul_gpio_irq_set(KEY_IRQ_LINE, 1);

    timer_mod(g_device_info.release_timer,
              qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 1000);
}
#endif

static void key_gpio_register(FslIMX6ULState *s)
{
    // gpio1_18
    g_device_info.gpio_irq[KEY_IRQ_LINE] = qdev_get_gpio_in(
        DEVICE(&s->gpio[0]),
        18);
    if (!g_device_info.gpio_irq[0]) {
        fprintf(stderr, "gpio1_18 get failed\n");
    }

    // gpio1_1
    g_device_info.gpio_irq[AP3216_IRQ_LINE] = qdev_get_gpio_in(
        DEVICE(&s->gpio[0]),
        1);
    if (!g_device_info.gpio_irq[AP3216_IRQ_LINE]) {
        fprintf(stderr, "gpio1_1 get failed\n");
    }

    // gpio1_2
    g_device_info.gpio_irq[PCF8563_IRQ_LINE] = qdev_get_gpio_in(
        DEVICE(&s->gpio[0]),
        2);
    if (!g_device_info.gpio_irq[PCF8563_IRQ_LINE]) {
        fprintf(stderr, "gpio1_2 get failed\n");
    }

    // gpio1_9
    g_device_info.gpio_irq[GT9147_IRQ_LINE] = qdev_get_gpio_in(
        DEVICE(&s->gpio[0]),
        9);
    if (!g_device_info.gpio_irq[GT9147_IRQ_LINE]) {
        fprintf(stderr, "gpio1_9 get failed\n");
    }

#if KEY_TIMER_DEBUG
    g_device_info.press_timer =
        timer_new_ms(QEMU_CLOCK_VIRTUAL,
                    key_press_cb,
                    NULL);

    g_device_info.release_timer =
        timer_new_ms(QEMU_CLOCK_VIRTUAL,
                    key_release_cb,
                    NULL);

    timer_mod(g_device_info.press_timer,
            qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 1000);
#endif
} 

static uint64_t imx6ul_iomuxc_read(void *opaque,
                                   hwaddr offset,
                                   unsigned size)
{
    if (offset >= FSL_IMX6UL_IOMUXC_SIZE) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "IOMUXC: bad read offset=0x%"
                      HWADDR_PRIx "\n",
                      offset);
        return 0;
    }

    return g_device_info.iomux_regs[offset >> 2];
}

static void imx6ul_iomuxc_write(void *opaque,
                                hwaddr offset,
                                uint64_t value,
                                unsigned size)
{
    if (offset >= FSL_IMX6UL_IOMUXC_SIZE) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "IOMUXC: bad write offset=0x%"
                      HWADDR_PRIx "\n",
                      offset);
        return;
    }

    g_device_info.iomux_regs[offset >> 2] = (uint32_t)value;

    /*
     * GPIO1_IO00 MUX
     */
    if (offset == 0x68) {
        fprintf(stderr,
                "[IOMUXC] GPIO1_IO00 mux=%u\n",
                (uint32_t)(value & 0x7));
    }

    /*
     * GPIO1_IO00 PAD
     */
    if (offset == 0x2F4) {
        fprintf(stderr,
                "[IOMUXC] GPIO1_IO00 pad=0x%08x\n",
                (uint32_t)value);
    }
}

static const MemoryRegionOps imx6ul_iomuxc_ops = {
    .read = imx6ul_iomuxc_read,
    .write = imx6ul_iomuxc_write,

    .endianness = DEVICE_NATIVE_ENDIAN,

    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

static void iomuxc_register(DeviceState *dev, FslIMX6ULState *s)
{
    memory_region_init_io(
        &g_device_info.iomux,
        OBJECT(dev),
        &imx6ul_iomuxc_ops,
        s,
        "imx6ul-iomuxc",
        FSL_IMX6UL_IOMUXC_SIZE);
    
    memory_region_add_subregion(
            get_system_memory(),
            FSL_IMX6UL_IOMUXC_ADDR,
            &g_device_info.iomux);
}

static void ecspi_device_register(FslIMX6ULState *s)
{
    DeviceState *dev;
    SSIBus *bus;
    qemu_irq cs_input;

    /*
     * ECSPI3 的 SPI BUS
     */
    bus = (SSIBus *)qdev_get_child_bus(
                    DEVICE(&s->spi[2]),
                    "spi");

    if (!bus) {
        error_report("cannot find ecspi3 spi bus");
        return;
    }

    dev = qdev_new("icm20608");

    ssi_realize_and_unref(dev,
                          bus,
                          &error_fatal);

    // kernel内部使用的是GPIO1_IO20 I/O访问(非硬件控制CS)
    // 需要关联cs-gpio和spi-cs，才能正确工作
    cs_input = qdev_get_gpio_in_named(dev,
                                    SSI_GPIO_CS,
                                    0);
    if (!cs_input) {
        error_report("cannot get icm20608 CS input");
        return;
    }

    qdev_connect_gpio_out(DEVICE(&s->gpio[0]),  // GPIO1
                                20,             // pin20
                                cs_input);      // SPI slave CS input
}
