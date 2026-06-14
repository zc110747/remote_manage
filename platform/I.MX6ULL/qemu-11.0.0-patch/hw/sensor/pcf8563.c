/*
 * PCF8563 RTC emulator for QEMU
 *
 * I2C Address : 0x51
 *
 * Supported:
 *   - register read/write
 *   - repeated start
 *   - RTC time read
 *   - RTC time write
 *
 * Author: ChatGPT
 */

#include "qemu/osdep.h"
#include "hw/i2c/i2c.h"
#include "migration/vmstate.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "qemu/timer.h"
#include "hw/arm/fsl-imx6ul-register.h"
#include <time.h>

#define TYPE_PCF8563 "pcf8563"

OBJECT_DECLARE_SIMPLE_TYPE(PCF8563State, PCF8563)

/* ------------------------------------------------ */
/* Registers                                        */
/* ------------------------------------------------ */

#define PCF8563_REG_MAX     0x0F

#define REG_CTRL1           0x00
#define REG_CTRL2           0x01

#define REG_SEC             0x02
#define REG_MIN             0x03
#define REG_HOUR            0x04
#define REG_DAY             0x05
#define REG_WEEKDAY         0x06
#define REG_MONTH           0x07
#define REG_YEAR            0x08

#define REG_MIN_ALARM       0x09
#define REG_HOUR_ALARM      0x0A
#define REG_DAY_ALARM       0x0B
#define REG_WEEKDAY_ALARM   0x0C

#define REG_CLKOUT          0x0D
#define REG_TIMER_CTRL      0x0E
#define REG_TIMER           0x0F

/* ------------------------------------------------ */
/* Device State                                     */
/* ------------------------------------------------ */

typedef struct PCF8563State {
    I2CSlave parent_obj;

    uint8_t regs[PCF8563_REG_MAX + 1];

    uint8_t reg;

    bool waiting_reg;

    time_t timestamp;

    QEMUTimer *timer;
} PCF8563State;

/* ------------------------------------------------ */
/* BCD Helpers                                      */
/* ------------------------------------------------ */

static uint8_t to_bcd(uint8_t value)
{
    return ((value / 10) << 4) | (value % 10);
}

static uint8_t from_bcd(uint8_t value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}

static void pcf8563_update_time(PCF8563State *s)
{
    struct tm *tm;

    tm = localtime(&s->timestamp);

    s->regs[REG_SEC] = to_bcd(tm->tm_sec);
    s->regs[REG_MIN] = to_bcd(tm->tm_min);
    s->regs[REG_HOUR] = to_bcd(tm->tm_hour);
    s->regs[REG_DAY] = to_bcd(tm->tm_mday);
    s->regs[REG_WEEKDAY] = tm->tm_wday;
    s->regs[REG_MONTH] = to_bcd(tm->tm_mon + 1);
    if (tm->tm_year >= 100) {
        s->regs[REG_MONTH] |= 1<<7;
    }
    s->regs[REG_YEAR] = to_bcd(tm->tm_year % 100);
}

/* ------------------------------------------------ */
/* RTC Store                                        */
/* ------------------------------------------------ */

static void pcf8563_store_time(PCF8563State *s)
{
    struct tm rtc_tm;

    rtc_tm.tm_sec = from_bcd(s->regs[REG_SEC] & 0x7F);
    rtc_tm.tm_min = from_bcd(s->regs[REG_MIN] & 0x7F);
    rtc_tm.tm_hour = from_bcd(s->regs[REG_HOUR] & 0x3F);
    rtc_tm.tm_mday = from_bcd(s->regs[REG_DAY] & 0x3F);
    rtc_tm.tm_mon = from_bcd(s->regs[REG_MONTH] & 0x1F) - 1;
    rtc_tm.tm_year = from_bcd(s->regs[REG_YEAR]);
    if (s->regs[REG_MONTH] & 1<<7) {
        rtc_tm.tm_year += 100;
    }
    
    /* 设置其他字段 */
    rtc_tm.tm_wday = 0;      /* mktime 会自动计算 */
    rtc_tm.tm_yday = 0;      /* mktime 会自动计算 */
    rtc_tm.tm_isdst = -1;    /* -1 表示让系统自动判断夏令时 */

    s->timestamp = mktime(&rtc_tm);
}

/* ------------------------------------------------ */
/* Reset                                            */
/* ------------------------------------------------ */

static void timer_cb(void *opaque)
{ 
    PCF8563State *s = opaque;
    
    s->timestamp++;

    timer_mod(s->timer,
            qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 1000);
}

static void pcf8563_reset_state(PCF8563State *s)
{
    memset(s->regs, 0, sizeof(s->regs));

    s->reg = 0;
    s->waiting_reg = true;
    s->timestamp = time(NULL);  // 获取当前的时间戳

    pcf8563_update_time(s);

    s->timer = timer_new_ms(QEMU_CLOCK_VIRTUAL,
                timer_cb,
                s);
    timer_mod(s->timer,
            qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 1000);
}

static void pcf8563_reset_hold(Object *obj,
                               ResetType type)
{
    PCF8563State *s = PCF8563(obj);

    pcf8563_reset_state(s);
}

/* ------------------------------------------------ */
/* I2C Write                                        */
/* ------------------------------------------------ */

static int pcf8563_send(I2CSlave *i2c, uint8_t data)
{
    PCF8563State *s = PCF8563(i2c);

    /*
     * First byte = register address
     */

    if (s->waiting_reg) {
        s->reg = data;
        s->waiting_reg = false;
        return 0;
    }

    /*
     * Register write
     */

    if (s->reg <= PCF8563_REG_MAX) {
        s->regs[s->reg] = data;
    }

    /*
     * Time registers written
     */

    if (s->reg >= REG_SEC &&
        s->reg <= REG_YEAR) {
        pcf8563_store_time(s);
    }

    s->reg++;

    return 0;
}

/* ------------------------------------------------ */
/* I2C Read                                         */
/* ------------------------------------------------ */

static uint8_t pcf8563_recv(I2CSlave *i2c)
{
    PCF8563State *s = PCF8563(i2c);

    uint8_t value = 0xFF;

    if (s->reg >= REG_SEC
    && s->reg <= REG_YEAR) {
        pcf8563_update_time(s);
    }

    // 读取当前寄存器值
    if (s->reg <= PCF8563_REG_MAX) {
        value = s->regs[s->reg];
    }

    s->reg++;

    return value;
}

/* ------------------------------------------------ */
/* I2C Events                                       */
/* ------------------------------------------------ */

static int pcf8563_event(I2CSlave *i2c,
                         enum i2c_event event)
{
    PCF8563State *s = PCF8563(i2c);

    switch (event) {

    case I2C_START_SEND:
        s->waiting_reg = true;
        break;

    case I2C_START_RECV:
        break;

    case I2C_FINISH:
        break;

    case I2C_NACK:
        break;

    default:
        break;
    }

    return 0;
}

/* ------------------------------------------------ */
/* Realize                                          */
/* ------------------------------------------------ */

static void pcf8563_realize(DeviceState *dev,
                            Error **errp)
{
    PCF8563State *s = PCF8563(dev);

    pcf8563_reset_state(s);
}

/* ------------------------------------------------ */
/* Migration                                        */
/* ------------------------------------------------ */

static const VMStateDescription vmstate_pcf8563 = {
    .name = "pcf8563",
    .version_id = 1,
    .minimum_version_id = 1,

    .fields = (const VMStateField[]) {

        VMSTATE_UINT8_ARRAY(
            regs,
            PCF8563State,
            PCF8563_REG_MAX + 1),

        VMSTATE_UINT8(
            reg,
            PCF8563State),

        VMSTATE_BOOL(
            waiting_reg,
            PCF8563State),

        VMSTATE_END_OF_LIST()
    }
};

/* ------------------------------------------------ */
/* Instance Init                                    */
/* ------------------------------------------------ */

static void pcf8563_init(Object *obj)
{
    PCF8563State *s = PCF8563(obj);

    s->waiting_reg = true;
}

/* ------------------------------------------------ */
/* Class Init                                       */
/* ------------------------------------------------ */

static void pcf8563_class_init(ObjectClass *klass,
                               const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    dc->realize = pcf8563_realize;

    dc->vmsd = &vmstate_pcf8563;

    rc->phases.hold = pcf8563_reset_hold;

    k->send = pcf8563_send;
    k->recv = pcf8563_recv;
    k->event = pcf8563_event;
}

/* ------------------------------------------------ */
/* Type                                             */
/* ------------------------------------------------ */

static const TypeInfo pcf8563_type_info = {
    .name = TYPE_PCF8563,
    .parent = TYPE_I2C_SLAVE,
    .instance_size = sizeof(PCF8563State),
    .instance_init = pcf8563_init,
    .class_init = pcf8563_class_init,
};

static void pcf8563_register_types(void)
{
    type_register_static(&pcf8563_type_info);
}

type_init(pcf8563_register_types)