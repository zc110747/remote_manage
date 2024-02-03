/*
 * File      : kernal_i2c_ap.c
 * This file is i2C ap3216 driver
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-4-30      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include "kernel_i2c_ap.h"

#define AP3216_NAME            "ap3216"
#define I2C_AP_CNT              1

struct ap3216_dev
{
    dev_t devid;            /* 设备号  */
    struct cdev cdev;       /* cdev */
    struct class *class;    /* 类 */
    struct device *device;  /* 设备 */
    struct device_node*nd;  /* 设备节点 */
    int major;              /* 主设备号 */
    void *private_data;     /* 私有数据 */
    int cs_gpio;            /* 片选所使用的GPIO编号*/
};

//ap3216应用信息
struct ap3216_info
{
    struct ap3216_dev dev;
    unsigned short ir;
    unsigned short als;
    unsigned short ps;
};
static struct ap3216_info ap_info;

static int ap3216_read_regs(struct ap3216_dev *dev, u8 reg, void *buf, int len)
{
    int ret;
    struct i2c_msg msg[2];
    struct i2c_client *client = (struct i2c_client *)dev->private_data;

    /* msg[0]为发送要读取的首地址 */
    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = &reg;
    msg[0].len = 1;

    /* msg[1]读取数据 */
    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = buf;
    msg[1].len = len;

    ret = i2c_transfer(client->adapter, msg, 2);
    if (ret == 2)
    {
        ret = 0;
    }
    else
    {
        printk("i2c rd failed=%d reg=%06x len=%d\n",ret, reg, len);
        ret = -EREMOTEIO;
    }
    return ret;
}

static unsigned char ap3216_read_reg(struct ap3216_dev *dev, u8 reg)
{
    u8 data = 0;

    ap3216_read_regs(dev, reg, &data, 1);
    return data;
}

void ap3216_readdata(struct ap3216_info *p_info)
{
    unsigned char i =0;
    unsigned char buf[6];

    /* 循环读取所有传感器数据 */
    for (i = 0; i < 6; i++)
    {
        buf[i] = ap3216_read_reg(&p_info->dev, AP3216C_IRDATALOW + i);
    }

    if (buf[0]&(1<<7))
    {
        p_info->ir = 0;
    }
    else
    {
        p_info->ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03);
    }

    p_info->als = ((unsigned short)buf[3] << 8) | buf[2];

    if (buf[4]&(1<<6))
    {
        p_info->ps = 0;
    }
    else
    {
        p_info->ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F); 
    }
}

static s32 ap3216_write_regs(struct ap3216_dev *dev, u8 reg, u8 *buf, u8 len)
{
    u8 b[256];
    struct i2c_msg msg;
    struct i2c_client *client = (struct i2c_client *)dev->private_data;

    b[0] = reg;
    memcpy(&b[1], buf, len);

    msg.addr = client->addr;
    msg.flags = 0;

    msg.buf = b;
    msg.len = len + 1;

    return i2c_transfer(client->adapter, &msg, 1);
}

static void ap3216_write_onereg(struct ap3216_dev *dev, u8 reg, u8 data)
{
    u8 buf = 0;
    buf = data;
    ap3216_write_regs(dev, reg, &buf, 1);
}


static int ap3216_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &ap_info;
    return 0;
}

static ssize_t ap3216_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    short data[3];
    long err = 0;

    struct ap3216_info *pApInfo = (struct ap3216_info *)filp->private_data;

    ap3216_readdata(pApInfo);

    data[0] = pApInfo->ir;
    data[1] = pApInfo->als;
    data[2] = pApInfo->ps;
    err = copy_to_user(buf, data, sizeof(data));
    return cnt;
}

static int ap3216_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static const struct file_operations ap3216_ops = {
    .owner = THIS_MODULE,
    .open = ap3216_open,
    .read = ap3216_read,
    .release = ap3216_release,
};

static int ap3216_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    if (ap_info.dev.major)
    {
        ap_info.dev.devid = MKDEV(ap_info.dev.major, 0);
        register_chrdev_region(ap_info.dev.devid, I2C_AP_CNT, AP3216_NAME);
    }
    else
    {
        alloc_chrdev_region(&ap_info.dev.devid, 0, I2C_AP_CNT, AP3216_NAME);
        ap_info.dev.major = MAJOR(ap_info.dev.devid);
    }

    cdev_init(&ap_info.dev.cdev, &ap3216_ops);
    cdev_add(&ap_info.dev.cdev, ap_info.dev.devid, I2C_AP_CNT);

    ap_info.dev.class = class_create(THIS_MODULE, AP3216_NAME);
    if (IS_ERR(ap_info.dev.class))
    {
        return PTR_ERR(ap_info.dev.class);
    }

    ap_info.dev.device = device_create(ap_info.dev.class, NULL, ap_info.dev.devid, NULL, AP3216_NAME);
    if (IS_ERR(ap_info.dev.device))
    {
        return PTR_ERR(ap_info.dev.device);
    }

    ap_info.dev.private_data = client;
    ap3216_write_onereg(&ap_info.dev, AP3216C_SYSTEMCONG, 0x04);/* 复位AP3216C */
    mdelay(50);
    ap3216_write_onereg(&ap_info.dev, AP3216C_SYSTEMCONG, 0X03);/* 开启ALS、PS+IR */

    printk(KERN_INFO"I2c-Ap3216 Driver Init Ok!\r\n");
    return 0;
}

static void ap3216_remove(struct i2c_client *client)
{
    cdev_del(&ap_info.dev.cdev);
    unregister_chrdev_region(ap_info.dev.devid, I2C_AP_CNT);

    device_destroy(ap_info.dev.class, ap_info.dev.devid);
    class_destroy(ap_info.dev.class);
}

static const struct i2c_device_id ap3216_id[] = {
    {"ap3216", 0},
    {}
};

static const struct of_device_id ap3216_of_match[] = {
    { .compatible = "rmk,ap3216" },
    { /* Sentinel */ }
};

static struct i2c_driver ap3216_driver = {
    .probe = ap3216_probe,
    .remove = ap3216_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "ap3216",
        .of_match_table = ap3216_of_match, 
    },
    .id_table = ap3216_id,
};

static int __init ap3216_module_init(void)
{
    return i2c_add_driver(&ap3216_driver);
}

static void __exit ap3216_module_exit(void)
{
    return i2c_del_driver(&ap3216_driver);
}

module_init(ap3216_module_init);
module_exit(ap3216_module_exit);
MODULE_AUTHOR("zc");                      
MODULE_LICENSE("GPL v2");                  
MODULE_DESCRIPTION("ap3216 driver");      
MODULE_ALIAS("i2c_ap3216_driver");
