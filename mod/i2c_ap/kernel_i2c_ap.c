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

#define DEVICE_NAME            "ap3216"
#define DEVICE_CNT              1

#define DEFAULT_MAJOR           0         
#define DEFAULT_MINOR           0  

struct read_data
{
    unsigned short ir;
    unsigned short als;
    unsigned short ps;
};

struct ap3216_data
{
    //设备信息
    dev_t dev_id;            
    struct cdev cdev;      
    struct class *class;   
    struct device *device;

    //硬件信息     
    int cs_gpio;          
    void *private_data;

    //i2c获取信息
    struct read_data data;
};

static int ap3216_read_regs(struct ap3216_data *chip, u8 reg, void *buf, int len)
{
    int ret;
    struct i2c_msg msg[2];
    struct i2c_client *client = (struct i2c_client *)chip->private_data;

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

static unsigned char ap3216_read_reg(struct ap3216_data *chip, u8 reg)
{
    u8 data = 0;

    ap3216_read_regs(chip, reg, &data, 1);
    return data;
}

void ap3216_readdata(struct ap3216_data *chip)
{
    unsigned char i = 0;
    unsigned char buf[6];

    for (i = 0; i < 6; i++){
        buf[i] = ap3216_read_reg(chip, AP3216C_IRDATALOW + i);
    }

    if (buf[0]&(1<<7)){
        chip->data.ir = 0;
    } else{
        chip->data.ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03);
    }

    chip->data.als = ((unsigned short)buf[3] << 8) | buf[2];
    if (buf[4]&(1<<6)) {
        chip->data.ps = 0;
    } else {
        chip->data.ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F); 
    }
}

static s32 ap3216_write_regs(struct ap3216_data *chip, u8 reg, u8 *buf, u8 len)
{
    u8 b[256];
    struct i2c_msg msg;
    struct i2c_client *client = (struct i2c_client *)chip->private_data;

    b[0] = reg;
    memcpy(&b[1], buf, len);

    msg.addr = client->addr;
    msg.flags = 0;

    msg.buf = b;
    msg.len = len + 1;

    return i2c_transfer(client->adapter, &msg, 1);
}

static void ap3216_write_onereg(struct ap3216_data *chip, u8 reg, u8 data)
{
    u8 buf = 0;
    buf = data;
    ap3216_write_regs(chip, reg, &buf, 1);
}

static int ap3216_open(struct inode *inode, struct file *filp)
{
    struct ap3216_data *chip;

    chip = container_of(inode->i_cdev, struct ap3216_data, cdev);
    filp->private_data = chip;
    return 0;
}

static ssize_t ap3216_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    short data[3];
    long err = 0;

    struct ap3216_data *chip = (struct ap3216_data *)filp->private_data;

    ap3216_readdata(chip);

    data[0] = chip->data.ir;
    data[1] = chip->data.als;
    data[2] = chip->data.ps;
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

static int i2c_device_create(struct ap3216_data *chip)
{
    int result;
    int major = DEFAULT_MAJOR;
    int minor = DEFAULT_MINOR;
    struct i2c_client *client = (struct i2c_client *)chip->private_data;

    if (major) {
        chip->dev_id= MKDEV(major, minor);
        result = register_chrdev_region(chip->dev_id, 1, DEVICE_NAME);
    } else {
        result = alloc_chrdev_region(&chip->dev_id, 0, 1, DEVICE_NAME);
        major = MAJOR(chip->dev_id);
        minor = MINOR(chip->dev_id);
    }
    if (result < 0){
        dev_err(&client->dev, "dev alloc id failed\n");
        goto exit;
    }

    cdev_init(&chip->cdev, &ap3216_ops);
    chip->cdev.owner = THIS_MODULE;
    result = cdev_add(&chip->cdev, chip->dev_id, 1);
    if (result != 0){
        dev_err(&client->dev, "cdev add failed\n");
        goto exit_cdev_add;
    }

    chip->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(chip->class)){
        dev_err(&client->dev, "class create failed!\r\n");
        result = PTR_ERR(chip->class);
        goto exit_class_create;
    }

    chip->device = device_create(chip->class, NULL, chip->dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(chip->device)){
        dev_err(&client->dev, "device create failed!\r\n");
        result = PTR_ERR(chip->device);
        goto exit_device_create;
    }

    dev_info(&client->dev, "dev create ok, major:%d, minor:%d\r\n", major, minor);
    return 0;

exit_device_create:
    class_destroy(chip->class);
exit_class_create:
    cdev_del(&chip->cdev);
exit_cdev_add:
    unregister_chrdev_region(chip->dev_id, 1);
exit:
    return result;
}

static int i2c_chip_init(struct ap3216_data *chip)
{
    ap3216_write_onereg(chip, AP3216C_SYSTEMCONG, 0x04);
    mdelay(50);
    ap3216_write_onereg(chip, AP3216C_SYSTEMCONG, 0X03);

    return 0;
}

static int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int result;
    struct ap3216_data *chip = NULL;

    chip = devm_kzalloc(&client->dev, sizeof(struct ap3216_data), GFP_KERNEL);
    if (!chip){
        dev_err(&client->dev, "malloc error\n");
        return -ENOMEM;
    }
    chip->private_data = (void *)client;
    i2c_set_clientdata(client, chip);

    result = i2c_device_create(chip);
    if (result){
        dev_err(&client->dev, "device create failed!\n");
        return result;   
    }

    result = i2c_chip_init(chip);
    if (result){
        dev_err(&client->dev, "device create failed!\n");
        return result;   
    }

    dev_info(&client->dev, "i2c driver init ok!\r\n");
    return 0;
}

static void i2c_remove(struct i2c_client *client)
{
    struct ap3216_data *chip = i2c_get_clientdata(client);

    device_destroy(chip->class, chip->dev_id);
    class_destroy(chip->class);
    cdev_del(&chip->cdev);
    unregister_chrdev_region(chip->dev_id, DEVICE_CNT);
}

static const struct of_device_id ap3216_of_match[] = {
    { .compatible = "rmk,ap3216" },
    { /* Sentinel */ }
};

static struct i2c_driver ap3216_driver = {
    .probe = i2c_probe,
    .remove = i2c_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "ap3216",
        .of_match_table = ap3216_of_match, 
    },
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
