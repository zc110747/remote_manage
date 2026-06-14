#include "qemu/osdep.h"
#include "hw/ssi/ssi.h"
#include "qapi/error.h"
#include "qemu/module.h"

#define  TYPE_ICM20608 "icm20608"
OBJECT_DECLARE_SIMPLE_TYPE(ICM20608State, ICM20608)

/* -------------------------------------------------- */
/* ICM20608 Registers */
/* -------------------------------------------------- */

#define ICM20_SMPLRT_DIV      0x19
#define ICM20_CONFIG          0x1A
#define ICM20_GYRO_CONFIG     0x1B
#define ICM20_ACCEL_CONFIG    0x1C
#define ICM20_ACCEL_CONFIG2   0x1D
#define ICM20_FIFO_EN         0x23

#define ICM20_ACCEL_XOUT_H    0x3B
#define ICM20_ACCEL_XOUT_L    0x3C
#define ICM20_ACCEL_YOUT_H    0x3D
#define ICM20_ACCEL_YOUT_L    0x3E
#define ICM20_ACCEL_ZOUT_H    0x3F
#define ICM20_ACCEL_ZOUT_L    0x40

#define ICM20_TEMP_OUT_H      0x41
#define ICM20_TEMP_OUT_L      0x42

#define ICM20_GYRO_XOUT_H     0x43
#define ICM20_GYRO_XOUT_L     0x44
#define ICM20_GYRO_YOUT_H     0x45
#define ICM20_GYRO_YOUT_L     0x46
#define ICM20_GYRO_ZOUT_H     0x47
#define ICM20_GYRO_ZOUT_L     0x48

#define ICM20_PWR_MGMT_1      0x6B
#define ICM20_PWR_MGMT_2      0x6C

#define ICM20_WHO_AM_I        0x75

#define ICM20608_WHOAMI       0xAF

struct ICM20608State {
    SSIPeripheral parent_obj;

    uint8_t regs[256];

    bool cs_select;
    bool is_first_reg;
    bool is_read;

    uint8_t current_reg;
};

/* -------------------------------------------------- */

static void icm20608_init_regs(ICM20608State *s)
{
    memset(s->regs, 0, sizeof(s->regs));

    s->regs[ICM20_WHO_AM_I] = ICM20608_WHOAMI;

    s->regs[ICM20_PWR_MGMT_1] = 0x01;

    s->regs[ICM20_ACCEL_XOUT_H] = 0x01;
    s->regs[ICM20_ACCEL_XOUT_L] = 0x02;

    s->regs[ICM20_ACCEL_YOUT_H] = 0x03;
    s->regs[ICM20_ACCEL_YOUT_L] = 0x04;

    s->regs[ICM20_ACCEL_ZOUT_H] = 0x05;
    s->regs[ICM20_ACCEL_ZOUT_L] = 0x06;

    s->regs[ICM20_TEMP_OUT_H] = 0x10;
    s->regs[ICM20_TEMP_OUT_L] = 0x00;

    s->regs[ICM20_GYRO_XOUT_H] = 0x11;
    s->regs[ICM20_GYRO_XOUT_L] = 0x22;

    s->regs[ICM20_GYRO_YOUT_H] = 0x33;
    s->regs[ICM20_GYRO_YOUT_L] = 0x44;

    s->regs[ICM20_GYRO_ZOUT_H] = 0x55;
    s->regs[ICM20_GYRO_ZOUT_L] = 0x66;

    s->cs_select = false;
    s->is_read = false;
    s->current_reg = 0;
}

/* -------------------------------------------------- */

static uint32_t icm20608_transfer(SSIPeripheral *dev,
                                  uint32_t value)
{
    ICM20608State *s = ICM20608(dev);

    uint8_t data = value & 0xff;

    // cs未片选, 直接返回0
    if (!s->cs_select) {
        return 0;
    } 

    // 判断是否为寄存器位，寄存器位为最高位
    if (s->is_first_reg) {
        s->is_first_reg = false;
        s->is_read = !!(data & 0x80);
        s->current_reg = data & 0x7f;
        return 0;
    }

    if (s->is_read) {
        uint8_t ret = s->regs[s->current_reg];
        s->current_reg++;
        return ret;
    }

    s->regs[s->current_reg] = data;
    if (s->current_reg == ICM20_PWR_MGMT_1 &&
        data == 0x80) {
        icm20608_init_regs(s);
        s->regs[ICM20_PWR_MGMT_1] = 0x80;
    }
    s->current_reg++;

    return 0;
}

/* -------------------------------------------------- */

static int icm20608_set_cs(SSIPeripheral *dev,
                           bool select)
{
    ICM20608State *s = ICM20608(dev);

    // cs片选, 低有效, 表示开始接收
    if (select == 0) {
        s->cs_select = true;
    } else {
        s->cs_select = false;
    }
    s->is_first_reg = true;
    
    return 0;
}

/* -------------------------------------------------- */

static void icm20608_realize(SSIPeripheral *dev,
                             Error **errp)
{
    ICM20608State *s = ICM20608(dev);

    icm20608_init_regs(s);
}

/* -------------------------------------------------- */

static void icm20608_class_init(ObjectClass *klass,
                                const void *data)
{
    SSIPeripheralClass *spc =
        SSI_PERIPHERAL_CLASS(klass);

    spc->realize = icm20608_realize;
    spc->transfer = icm20608_transfer;
    spc->set_cs = icm20608_set_cs;
    spc->cs_polarity = SSI_CS_LOW;
}

/* -------------------------------------------------- */

static const TypeInfo icm20608_type_info = {
    .name          = TYPE_ICM20608,
    .parent        = TYPE_SSI_PERIPHERAL,
    .instance_size = sizeof(ICM20608State),
    .class_init    = icm20608_class_init,
};

static void icm20608_register_types(void)
{
    type_register_static(&icm20608_type_info);
}

type_init(icm20608_register_types);