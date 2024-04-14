/*
 * File      : kernel_gt9147.c
 * This file is the driver for touchkey gt9147.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
 */
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/regmap.h>

#include <linux/gpio/consumer.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/input/touchscreen.h>
#include <asm/unaligned.h>

#define GT_CTRL_REG             0x8040  /* GT9147控制寄存器         */
#define GT_MODSW_REG            0x804D  /* GT9147模式切换寄存器        */
#define GT_9xx_CFGS_REG         0x8047  /* GT9147配置起始地址寄存器    */
#define GT_1xx_CFGS_REG         0x8050  /* GT1151配置起始地址寄存器    */
#define GT_CHECK_REG            0x80FF  /* GT9147校验和寄存器       */
#define GT_PID_REG              0x8140  /* GT9147产品ID寄存器       */

#define GT_GSTID_REG            0x814E  /* GT9147当前检测到的触摸情况 */
#define GT_TP1_REG              0x814F  /* 第一个触摸点数据地址 */
#define GT_TP2_REG              0x8157  /* 第二个触摸点数据地址 */
#define GT_TP3_REG              0x815F  /* 第三个触摸点数据地址 */
#define GT_TP4_REG              0x8167  /* 第四个触摸点数据地址  */
#define GT_TP5_REG              0x816F  /* 第五个触摸点数据地址   */
#define MAX_SUPPORT_POINTS      5       /* 最多5点电容触摸 */
const u8 irq_table[] =
{
    IRQ_TYPE_EDGE_RISING, IRQ_TYPE_EDGE_FALLING, IRQ_TYPE_LEVEL_LOW, IRQ_TYPE_LEVEL_HIGH
};

struct gt9147_data
{
    int reset_pin;
    int irq_pin;
    int irq_num;

    u16 max_x;
    u16 max_y;
    u8 irqflags;

    struct i2c_client *client;
    struct input_dev *input_dev;
};

static int gt9147_i2c_write(struct i2c_client *client, u16 reg, u8 *buf, u8 len)
{
    u8 *addr_buf;
    struct i2c_msg msg;
    int ret;

    addr_buf = kmalloc(len + 2, GFP_KERNEL);
    if (!addr_buf)
        return -ENOMEM;

    addr_buf[0] = reg >> 8;
    addr_buf[1] = reg & 0xFF;
    memcpy(&addr_buf[2], buf, len);

    msg.addr = client->addr;
    msg.flags = 0;
    msg.buf = addr_buf;
    msg.len = len + 2;

    ret = i2c_transfer(client->adapter, &msg, 1);
    if (ret >= 0)
        ret = (ret == 1 ? 0 : -EIO);

    kfree(addr_buf);

    if (ret)
    {
        dev_err(&client->dev, "Error writing %d bytes to 0x%04x: %d\n", len, reg, ret);
    }
    return ret;
}

static int gt9147_i2c_write_u8(struct i2c_client *client, u16 reg, u8 value)
{
    return gt9147_i2c_write(client, reg, &value, sizeof(value));
}

static int gt9147_i2c_read(struct i2c_client *client, u16 reg, u8 *buf, int len)
{
    struct i2c_msg msgs[2];
    __be16 wbuf = cpu_to_be16(reg);
    int ret;

    msgs[0].flags = 0;
    msgs[0].addr  = client->addr;
    msgs[0].len   = 2;
    msgs[0].buf   = (u8 *)&wbuf;

    msgs[1].flags = I2C_M_RD;
    msgs[1].addr  = client->addr;
    msgs[1].len   = len;
    msgs[1].buf   = buf;

    ret = i2c_transfer(client->adapter, msgs, 2);
    if (ret >= 0)
        ret = (ret == ARRAY_SIZE(msgs) ? 0 : -EIO);

    if (ret) {
        dev_err(&client->dev, "Error reading %d bytes from 0x%04x: %d\n", len, reg, ret);
    }
    
    return ret;
}

static irqreturn_t gt9147_irq_handler(int irq, void *pdata)
{
    struct gt9147_data *chip = (struct gt9147_data *)pdata;
    struct i2c_client *client = chip->client;
    int touch_num = 0;
    int input_x, input_y;
    int id = 0;
    int ret = 0;
    u8 data;
    u8 touch_data[5];

    dev_err(&client->dev, "irq handler process\n");
    ret = gt9147_i2c_read(chip->client, GT_GSTID_REG, &data, 1);
    if (data == 0x00) {
        return IRQ_NONE;
    } else { 
        touch_num = data & 0x0f;
    }

    if (touch_num) {
        gt9147_i2c_read(chip->client, GT_TP1_REG, touch_data, 5);
        id = touch_data[0] & 0x0F;
        if(id == 0)
        {
            input_x  = touch_data[1] | (touch_data[2] << 8);
            input_y  = touch_data[3] | (touch_data[4] << 8);

            input_mt_slot(chip->input_dev, id);
            input_mt_report_slot_state(chip->input_dev, MT_TOOL_FINGER, true);
            input_report_abs(chip->input_dev, ABS_MT_POSITION_X, input_x);
            input_report_abs(chip->input_dev, ABS_MT_POSITION_Y, input_y);
        }
    }
    else if (touch_num == 0) {
        input_mt_slot(chip->input_dev, id);
        input_mt_report_slot_state(chip->input_dev, MT_TOOL_FINGER, false);
    }

    input_mt_report_pointer_emulation(chip->input_dev, true);
    input_sync(chip->input_dev);

    gt9147_i2c_write_u8(chip->client, GT_GSTID_REG, 0x00);

    return IRQ_HANDLED;
}


const u8 GT9147_CFG_TBL[]=
{ 
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
}; 

void gt9147_send_cfg(struct gt9147_data *chip, unsigned char mode)
{
	unsigned char buf[2];
	unsigned int i = 0;

	buf[0] = 0;
	buf[1] = mode;	/* 是否写入到GT9147 FLASH */
	for(i = 0; i < (sizeof(GT9147_CFG_TBL)); i++) /* 计算校验和 */
        buf[0] += GT9147_CFG_TBL[i];            
    buf[0] = (~buf[0]) + 1;

    /* 发送寄存器配置 */
    gt9147_i2c_write(chip->client, GT_9xx_CFGS_REG, (u8 *)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL));
    gt9147_i2c_write(chip->client, GT_CHECK_REG, buf, 2);/* 写入校验和,配置更新标记 */
}

static int gt9147_firmware_init(struct gt9147_data *chip)
{
    u8 data[7];
    u16 id = 0;
    int ret = 0;
    int version = 0;
    char id_str[5];
    struct i2c_client *client = chip->client;

    /*write register to reset.*/
    gt9147_i2c_write_u8(client, GT_CTRL_REG, 0x02);
    mdelay(100);
    gt9147_i2c_write_u8(client, GT_CTRL_REG, 0x00);
    mdelay(100);

    gt9147_send_cfg(chip, 0);

    ret = gt9147_i2c_read(client, GT_PID_REG, data, 6);
    if (ret) {
        dev_err(&client->dev, "Unable to read PID. error:%d.\n", ret);
        return ret;
    }
    memcpy(id_str, data, 4);
    id_str[4] = 0;
    if (kstrtou16(id_str, 10, &id)) {
        id = 0x1001;
    }
    version = get_unaligned_le16(&data[4]);
    dev_info(&client->dev, "ID %d, version: %04x\n", id, version);

    ret = gt9147_i2c_read(client, GT_9xx_CFGS_REG, data, 7);
    if (ret)
    {
        dev_err(&client->dev, "Unable to read Firmware, error:%d.\n", ret);
        return ret;
    }
    chip->max_x = (data[2] << 8) + data[1];
    chip->max_y = (data[4] << 8) + data[3];
    chip->irqflags = irq_table[data[6] & 0x3];

    dev_info(&client->dev, "X_MAX: %d, Y_MAX: %d, TRIGGER: 0x%02x", chip->max_x, chip->max_y, chip->irqflags);
    return ret;
}

static int gt9147_gpio_init(struct gt9147_data *chip)
{
    int ret = 0;
    struct i2c_client *client = chip->client;

    chip->reset_pin = of_get_named_gpio(client->dev.of_node, "reset-gpios", 0);
    chip->irq_pin = of_get_named_gpio(client->dev.of_node, "interrupt-gpios", 0);

    dev_info(&client->dev, "of_node:0x%x", (u32)client->dev.of_node);

    /*reset pin init*/
    if (gpio_is_valid(chip->reset_pin)) {
        ret = devm_gpio_request_one(&client->dev, 
                                    chip->reset_pin,
                                    GPIOF_OUT_INIT_HIGH,
                                    "gt9147 reset");
        if (ret) {
            dev_err(&client->dev, "failed to reqeust reset pin.\n");
            return ret;
        }
    } else {
        dev_err(&client->dev, "reset pin is invalid, pin:%d, %d.\n", chip->reset_pin, chip->irq_pin);
        return -1;
    }
    gpio_direction_output(chip->reset_pin, 1);

    /*irq pin init*/
    if (gpio_is_valid(chip->irq_pin)) {
        ret = devm_gpio_request_one(&client->dev, 
                                    chip->irq_pin, 
                                    GPIOF_IN,
                                    "gt9147 int");
        if (ret) {
            dev_err(&client->dev, "failed to reqeust int pin.\n");
            return ret;
        }
    }   else {
        dev_err(&client->dev, "interrupt pin is invalid, pin:%d.\n", chip->irq_pin);
        return -1;
    }

    /*reset sequerance*/
    gpio_set_value(chip->reset_pin, 0);
    msleep(10);
    gpio_set_value(chip->reset_pin, 1);
    msleep(10);

    /*set irq pin input*/
    gpio_direction_input(chip->irq_pin);

    ret = devm_request_threaded_irq(&client->dev,
                                client->irq,
                                NULL,
                                gt9147_irq_handler,
                                chip->irqflags | IRQF_ONESHOT,
                                client->name,
                                chip);
    if (ret) {
        dev_err(&client->dev, "Unable to request touchscreen IRQ.\n");
        return ret;
    }

    dev_info(&client->dev, "gt9147_gpio_init success, irq:%d!\n", client->irq);
    return ret;
}

static int gt9147_inputdev_create(struct gt9147_data *chip)
{
    int ret = 0;

    chip->input_dev = devm_input_allocate_device(&chip->client->dev);
    if (!chip->input_dev) {
        return -ENOMEM;
    }

    chip->input_dev->name = chip->client->name;
    chip->input_dev->id.bustype = BUS_I2C;
    chip->input_dev->dev.parent = &chip->client->dev;

    input_set_capability(chip->input_dev, EV_ABS, ABS_MT_POSITION_X);
    input_set_capability(chip->input_dev, EV_ABS, ABS_MT_POSITION_Y);
    input_set_capability(chip->input_dev, EV_KEY, BTN_TOUCH);
    input_set_capability(chip->input_dev, EV_ABS, ABS_PRESSURE);

    input_set_abs_params(chip->input_dev, ABS_X, 0, chip->max_x, 0, 0);
    input_set_abs_params(chip->input_dev, ABS_Y, 0, chip->max_y, 0, 0);
    input_set_abs_params(chip->input_dev, ABS_MT_POSITION_X, 0, chip->max_x, 0, 0);
    input_set_abs_params(chip->input_dev, ABS_MT_POSITION_Y, 0, chip->max_y, 0, 0);         
    ret = input_mt_init_slots(chip->input_dev, MAX_SUPPORT_POINTS, INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
    if (ret) {
        dev_err(&chip->client->dev, "failed to input_mt_init_slots, err:%d.\n", ret);
        return ret;
    }
    
    ret = input_register_device(chip->input_dev);
    if (ret) {
        dev_err(&chip->client->dev, "failed to input_register_device, err:%d.\n", ret);
        return ret;
    }

    return ret;
}

static int gt9147_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;
    struct gt9147_data *chip;

    chip = devm_kzalloc(&client->dev, sizeof(*chip), GFP_KERNEL);
    if (!chip) {
        dev_err(&client->dev, "allocate memory failed, error:%d.\n", ret);
        return -ENOMEM;
    }
    chip->client = client;
    i2c_set_clientdata(client, chip);

    ret = gt9147_gpio_init(chip);
    if (ret) {
        dev_err(&client->dev, "reset device failed, error:%d.\n", ret);
        return ret;
    }

    ret = gt9147_firmware_init(chip);
    if (ret){
        dev_err(&client->dev, "firmware init failed, error:%d.\n", ret);
        return ret;
    }

    /*step4: input device register*/
    ret = gt9147_inputdev_create(chip);
    if (ret){
        dev_err(&client->dev, "input dev create failed, error:%d.\n", ret);
        return ret;
    }

    dev_info(&client->dev, "gt9147 driver init success.\n");
    return 0;
}

static void gt9147_remove(struct i2c_client *client)
{
    struct gt9147_data *chip = i2c_get_clientdata(client);

    dev_info(&client->dev, "gt9147 driver release .\n");
    input_unregister_device(chip->input_dev);
}

static const struct of_device_id of_match_gt9147[] = {
    { .compatible = "rmk,gt9147" },
    { /* Sentinel */ }
};

static struct i2c_driver gt9147_driver = {
    .probe = gt9147_probe,
    .remove = gt9147_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "gt9147",
        .of_match_table = of_match_gt9147,
    }
};

static int __init gt9147_module_init(void)
{
    return i2c_add_driver(&gt9147_driver);
}

static void __exit gt9147_module_exit(void)
{
    return i2c_del_driver(&gt9147_driver);
}

module_init(gt9147_module_init);
module_exit(gt9147_module_exit);
MODULE_AUTHOR("zc");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("platform driver for gt9147");
MODULE_ALIAS("gt9147_driver");
