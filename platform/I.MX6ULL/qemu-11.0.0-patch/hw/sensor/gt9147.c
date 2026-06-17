
#include "qemu/osdep.h"
#include "hw/i2c/i2c.h"
#include "migration/vmstate.h"
#include "qemu/module.h"
#include "qemu/timer.h"
#include "qemu/log.h"

#define GT_CTRL_REG      0x8040
#define GT_CFG_REG       0x8047
#define GT_CHECK_REG     0x80FF
#define GT_PID_REG       0x8140
#define GT_GSTID_REG     0x814E
#define GT_TP1_REG       0x814F

#define TYPE_GT9147 "gt9147"
OBJECT_DECLARE_SIMPLE_TYPE(GT9147State, GT9147)

struct GT9147State {
    I2CSlave parent_obj;

    uint8_t regs[0x10000];
    bool waiting_reg;
    
    uint16_t addr;
    uint8_t addr_len;

    bool read_phase;

    uint16_t x;
    uint16_t y;
    bool pressed;
};

static GT9147State *s = NULL;

static void gt9147_realize(DeviceState *dev, Error **errp)
{
    memset(s->regs, 0, sizeof(s->regs));

    memcpy(&s->regs[GT_PID_REG], "9147", 4);

    s->regs[GT_PID_REG + 4] = 0x00;
    s->regs[GT_PID_REG + 5] = 0x10;

    /*
     * X=800
     * Y=480
     */

    s->regs[GT_CFG_REG + 1] = 800 & 0xff;
    s->regs[GT_CFG_REG + 2] = 800 >> 8;

    s->regs[GT_CFG_REG + 3] = 480 & 0xff;
    s->regs[GT_CFG_REG + 4] = 480 >> 8;

    s->regs[GT_CFG_REG + 6] = 1;

    s->pressed = false;
    s->x = 0;
    s->y = 0;
}

static int gt9147_send(I2CSlave *i2c, uint8_t data)
{
    // 获取寄存器地址
    if (s->addr_len < 2) {
        if (s->addr_len == 0)
            s->addr = data << 8;
        else
            s->addr |= data;

        s->addr_len++;
        return 0;
    }

    s->regs[s->addr++] = data;

    if (s->addr - 1 == GT_GSTID_REG &&
        data == 0x00) {
    }

    return 0;
}

static uint8_t gt9147_recv(I2CSlave *i2c)
{
    return s->regs[s->addr++];
}

static int gt9147_event(I2CSlave *i2c, enum i2c_event event)
{
    switch (event) {

    case I2C_START_SEND:
        s->addr_len = 0;
        break;

    case I2C_START_RECV:
        break;

    case I2C_FINISH:
        s->addr_len = 0;
        break;

    default:
        break;
    }

    return 0;
}

void gt9147_touch(uint16_t x,
                    uint16_t y,
                    bool down)
{
    if (down) {
        s->regs[GT_GSTID_REG] = 0x81;

        s->regs[GT_TP1_REG + 0] = 0;

        s->regs[GT_TP1_REG + 1] = x & 0xff;
        s->regs[GT_TP1_REG + 2] = x >> 8;

        s->regs[GT_TP1_REG + 3] = y & 0xff;
        s->regs[GT_TP1_REG + 4] = y >> 8;

    } else {
        s->regs[GT_GSTID_REG] = 0;
    }
}

static const VMStateDescription vmstate_gt9147 = {
    .name = TYPE_GT9147,
    .version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT16(addr, GT9147State),
        VMSTATE_UINT8(addr_len, GT9147State),
        VMSTATE_BUFFER(regs, GT9147State),
        VMSTATE_END_OF_LIST()
    }
};

static void gt9147_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);

    dc->realize = gt9147_realize;
    dc->vmsd = &vmstate_gt9147;

    k->send = gt9147_send;
    k->recv = gt9147_recv;
    k->event = gt9147_event;
}

static void gt9147_init(Object *obj)
{
    s = GT9147(obj);
    s->addr_len = 0;
}

static const TypeInfo gt9147_info = {
    .name          = TYPE_GT9147,
    .parent        = TYPE_I2C_SLAVE,
    .instance_size = sizeof(GT9147State),
    .instance_init = gt9147_init,
    .class_init    = gt9147_class_init,
};

static void gt9147_register_types(void)
{
    type_register_static(&gt9147_info);
}

type_init(gt9147_register_types)
