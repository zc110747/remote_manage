/*
 * AP3216C emulator for QEMU 10.1-dev
 *
 * I2C address : 0x1e
 *
 * Supported:
 *   - register read/write
 *   - repeated start
 *   - soft reset
 *   - fake ALS/PS/IR data
 *
 * Author: ChatGPT
 */

#include "qemu/osdep.h"
#include "hw/i2c/i2c.h"
#include "migration/vmstate.h"
#include "qapi/error.h"
#include "qemu/module.h"

#define TYPE_AP3216 "ap3216"

OBJECT_DECLARE_SIMPLE_TYPE(AP3216State, AP3216)

/* ------------------------------------------------ */
/* Registers                                        */
/* ------------------------------------------------ */

#define AP3216_REG_MAX      0x40

#define AP3216_SYS_CFG      0x00
#define AP3216_INT_STATUS   0x01

#define AP3216_IRDATALOW    0x0A
#define AP3216_IRDATAHIGH   0x0B

#define AP3216_ALSDATALOW   0x0C
#define AP3216_ALSDATAHIGH  0x0D

#define AP3216_PSDATALOW    0x0E
#define AP3216_PSDATAHIGH   0x0F

/* ------------------------------------------------ */
/* Device State                                     */
/* ------------------------------------------------ */

typedef struct AP3216State {
    I2CSlave parent_obj;

    uint8_t regs[AP3216_REG_MAX + 1];

    uint8_t reg_ptr;

    bool waiting_reg;

    uint16_t als;
    uint16_t ps;
    uint16_t ir;
    
    uint16_t last_ps;
} AP3216State;

/*
IR数据：10位分辨率，存放在 0x0A 和 0x0B。
    0x0A：[7]位表示“IR&PS数据有效，0有效”，[1:0]位是IR的最低2位。- 
    0x0B：[7:0]位是IR的高8位数据。
ALS数据：16位分辨率，存放在 0x0C（低8位）和 0x0D（高8位）。
PS数据：10位分辨率，存放在 0x0E 和 0x0F。
    0x0E：[7]位表示物体接近状态（0：远离，1：靠近），[3:0]位是PS的最低4位，[6]：数据有效, 0有效
    0x0F：[7]位也表示物体接近状态（同上），[5:0]位是PS的高6位。
*/
static void ap3216_update_sensor(AP3216State *s)
{
    // IR register
    s->regs[AP3216_IRDATALOW] = (s->ir&0x03);
    s->regs[AP3216_IRDATAHIGH] = (s->ir>>2)&0xff;

    // ALS register
    s->regs[AP3216_ALSDATALOW] = s->als & 0xff;
    s->regs[AP3216_ALSDATAHIGH] =(s->als >> 8) & 0xff;

    // PS数据
    s->regs[AP3216_PSDATALOW] = s->ps & 0x0f;
    s->regs[AP3216_PSDATAHIGH] = (s->ps >> 4) & 0x3f;

    if (s->last_ps > s->ps) {
        s->regs[AP3216_PSDATALOW] |= 1<<7;
        s->regs[AP3216_PSDATAHIGH] |= 1<<7;
    }
    s->last_ps = s->ps;
}

/* ------------------------------------------------ */
/* Reset                                            */
/* ------------------------------------------------ */

static void ap3216_reset_state(AP3216State *s)
{
    memset(s->regs, 0, sizeof(s->regs));

    s->reg_ptr = 0;
    s->waiting_reg = true;

    s->als = 200;
    s->ps  = 100;
    s->last_ps = s->ps;
    s->ir  = 50;

    s->regs[AP3216_SYS_CFG] = 0x00;

    ap3216_update_sensor(s);
}

static void ap3216_reset_hold(Object *obj,
                              ResetType type)
{
    AP3216State *s = AP3216(obj);

    ap3216_reset_state(s);
}

static int ap3216_send(I2CSlave *i2c,
                       uint8_t data)
{
    AP3216State *s = AP3216(i2c);

    // recv register
    if (s->waiting_reg) {
        s->reg_ptr = data;
        s->waiting_reg = false;
        return 0;
    }

    // register write
    if (s->reg_ptr <= AP3216_REG_MAX) {
        s->regs[s->reg_ptr] = data;
    }

    // soft reset
    if (s->reg_ptr == AP3216_SYS_CFG &&
        data == 0x04) {

        ap3216_reset_state(s);

        s->regs[AP3216_SYS_CFG] = 0x00;
    }

    s->reg_ptr++;

    return 0;
}

/* ------------------------------------------------ */
/* I2C Read                                         */
/* ------------------------------------------------ */

static uint8_t ap3216_recv(I2CSlave *i2c)
{
    AP3216State *s = AP3216(i2c);

    uint8_t value = 0xff;

    if (s->reg_ptr >= AP3216_IRDATALOW &&
        s->reg_ptr <= AP3216_PSDATAHIGH) {
        ap3216_update_sensor(s);
    }

    if (s->reg_ptr <= AP3216_REG_MAX) {
        value = s->regs[s->reg_ptr];
    }

    s->reg_ptr++;

    return value;
}

/* ------------------------------------------------ */
/* I2C Events                                       */
/* ------------------------------------------------ */

static int ap3216_event(I2CSlave *i2c,
                        enum i2c_event event)
{
    AP3216State *s = AP3216(i2c);

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

static void ap3216_realize(DeviceState *dev,
                           Error **errp)
{
    AP3216State *s = AP3216(dev);

    ap3216_reset_state(s);


}

static const VMStateDescription vmstate_ap3216 = {
    .name = "ap3216",
    .version_id = 1,
    .minimum_version_id = 1,

    .fields = (const VMStateField[]) {
        VMSTATE_UINT8_ARRAY(regs, AP3216State, AP3216_REG_MAX + 1),
        VMSTATE_UINT8(reg_ptr,  AP3216State),
        VMSTATE_BOOL(waiting_reg, AP3216State),
        VMSTATE_UINT16(als, AP3216State),
        VMSTATE_UINT16(ps, AP3216State),
        VMSTATE_UINT16(ir, AP3216State),
        VMSTATE_END_OF_LIST()
    }
};


static void ap3216_init(Object *obj)
{
    AP3216State *s = AP3216(obj);

    s->waiting_reg = true;
}

static void ap3216_class_init(ObjectClass *klass,
                              const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    dc->realize = ap3216_realize;

    dc->vmsd = &vmstate_ap3216;

    rc->phases.hold = ap3216_reset_hold;

    k->send = ap3216_send;
    k->recv = ap3216_recv;
    k->event = ap3216_event;
}

static const TypeInfo ap3216_type_info = {
    .name = TYPE_AP3216,
    .parent = TYPE_I2C_SLAVE,
    .instance_size = sizeof(AP3216State),
    .instance_init = ap3216_init,
    .class_init = ap3216_class_init,
};

static void ap3216_register_types(void)
{
    type_register_static(&ap3216_type_info);
}

type_init(ap3216_register_types)
