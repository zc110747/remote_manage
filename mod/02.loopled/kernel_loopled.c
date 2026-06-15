////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      kernel_loopled.c
//
//  Purpose:
//      三色灯控制输出(3-pin).
//      LED硬件接口: 
//          GPIO4_21 -- CSI-D0
//          GPIO4_23 -- CSI-D2
//          GPIO4_25 -- CSI-D4
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
/*
设备树说明
usr_loopled {
    compatible = "rmk,usr-loopled";
    pinctrl-names = "default";
    pinctrl-0 = <&pinctrl_loopled>;
    leds-gpios = <&gpio4 21 GPIO_ACTIVE_HIGH>,
                <&gpio4 23 GPIO_ACTIVE_HIGH>,
                <&gpio4 25 GPIO_ACTIVE_HIGH>;
    status = "okay";
};


pinctrl_loopled: gpio-loopleds {
    fsl,pins = <
        MX6UL_PAD_CSI_DATA00__GPIO4_IO21        0x40017059
        MX6UL_PAD_CSI_DATA02__GPIO4_IO23        0x40017059
        MX6UL_PAD_CSI_DATA04__GPIO4_IO25        0x40017059
    >;
};

可通过修改leds-gpio和gpio-loople来支持不同数量的led
*/

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
#include <linux/of.h>

#define DEVICE_MAX_NUM                         16

struct loopled_data
{
    /*device info*/
    dev_t dev_id;     
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct platform_device *pdev;
    
    /* hardware info */
    int leds_num;
    struct gpio_desc *desc[DEVICE_MAX_NUM];
};

#define LED_OFF                            0
#define LED_ON                             1

//自定义设备号
#define DEFAULT_MAJOR                      0 
#define DEFAULT_MINOR                      0    
#define DEVICE_NAME                        "loopled"

static int led_hardware_set(struct loopled_data *chip, u8 index, u8 status)
{
    struct platform_device *pdev;
    int ret = 0;

    pdev = chip->pdev;

    if (index >= chip->leds_num) {
        dev_err(&pdev->dev, "index invalid:%d!\n", index);
        return -EINVAL;
    }

    switch (status)
    {
        case LED_ON:
            dev_info(&pdev->dev, "index: %d, on\n", index);
            gpiod_set_value_cansleep(chip->desc[index], 1);
            break;
        case LED_OFF:
            dev_info(&pdev->dev, "index: %d, off\n", index);
            gpiod_set_value_cansleep(chip->desc[index], 0);
            break;
        default:
            dev_err(&pdev->dev, "status invalid:%d!\n", status);
            ret = -EINVAL;
            break;
    }

    return ret;
}

static int led_open(struct inode *inode, struct file *filp)
{
    struct loopled_data *chip;
    
    chip = container_of(inode->i_cdev, struct loopled_data, cdev);
    filp->private_data = chip;
    return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t led_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int ret;
    struct loopled_data *chip;
    struct platform_device *pdev;
    u8 led_status[DEVICE_MAX_NUM];
    u8 index;
    
    chip = (struct loopled_data *)filp->private_data;
    pdev = chip->pdev;

    count = min_t(size_t, count, chip->leds_num);

    for (index=0; index<chip->leds_num; index++) {
        led_status[index] = gpiod_get_value_cansleep(chip->desc[index]);
    }
    
    ret = copy_to_user(buf, led_status, count);
    if (ret) {
        dev_err(&pdev->dev, "read failed!\n");
        return -EFAULT;
    }
    
    return count;
}

// 设置 LED0: echo "00" > /dev/loopled
// 设置 LED1: echo "11" > /dev/loopled
ssize_t led_write(struct file *filp, const char __user *buf, size_t size, loff_t *f_pos)
{
    int ret;
    u8 data[2];
    struct loopled_data *chip;
    struct platform_device *pdev;

    chip = (struct loopled_data *)filp->private_data;
    pdev = chip->pdev;

    if (size < 2) {
        dev_err(&pdev->dev, "size invalid:%d!\n", size);
        return -EINVAL;   
    }

    ret = copy_from_user(data, buf, 2);
    if (ret){
        dev_err(&pdev->dev, "copy_from_user failed!\n");
        return -EFAULT;
    }

    if (data[0] >= '0') {
        data[0] -= '0';
    }

    if (data[1] >= '0') {
        data[1] -= '0';
    }

    ret = led_hardware_set(chip, data[0], data[1]);
    if (ret){
        return -EINVAL;
    }
    
    return size;
}

#define LED_IOC_MAGIC      'L'

#define LED_SET             _IO(LED_IOC_MAGIC, 0)

//ioctl格式为
//(fd, cmd, [led_index, led_stats])
long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct platform_device *pdev;
    struct loopled_data *chip;
    unsigned int arglen;
    u8 pbuf[2];
    int ret;

    arglen = cmd;
    if (arglen != 2)
        return -EINVAL;

    chip = (struct loopled_data *)filp->private_data;
    pdev = chip->pdev;

    ret = copy_from_user(pbuf, (char *)arg, arglen);
    if (ret) {
        dev_err(&pdev->dev, "copy_from_user failed!\n");
        return -EFAULT;  
    }
    
    ret = led_hardware_set(chip, pbuf[0], pbuf[1]);
    if (ret) {
        dev_err(&pdev->dev, "led_hardware_set!\n");
        return -EINVAL;  
    }

    return 0;
}

static const struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .unlocked_ioctl = led_ioctl,
    .release = led_release,
};

static int led_device_create(struct loopled_data *chip)
{
    int ret;
    int major, minor;
    struct platform_device *pdev;

    major = DEFAULT_MAJOR;
    minor = DEFAULT_MINOR;
    pdev = chip->pdev;

    //1.申请设备号
    if (major) {
        chip->dev_id = MKDEV(major, minor);
        ret = register_chrdev_region(chip->dev_id, 1, DEVICE_NAME);
    } else {
        ret = alloc_chrdev_region(&chip->dev_id, 0, 1, DEVICE_NAME);
    }
    if (ret < 0) {
        dev_err(&pdev->dev, "id alloc failed!\n");
        goto exit;
    }
    
    //2.创建字符设备，关联设备号，并添加到内核
    cdev_init(&chip->cdev, &led_fops);
    chip->cdev.owner = THIS_MODULE;
    ret = cdev_add(&chip->cdev, chip->dev_id, 1);
    if (ret) {
        dev_err(&pdev->dev, "cdev add failed:%d!\n", ret);
        goto exit_cdev_add;
    }

    //3.创建设备类和设备文件，关联设备号，用于应用层访问
    chip->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(chip->class)) {
        dev_err(&pdev->dev, "class create failed!\n");
        ret = PTR_ERR(chip->class);
        goto exit_class_create;
    }
    chip->device = device_create(chip->class, NULL, chip->dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(chip->device)) {
        dev_err(&pdev->dev, "device create failed!\r\n");
        ret = PTR_ERR(chip->device);
        goto exit_device_create;
    }

    dev_info(&pdev->dev, "device create success!\r\n");
    return 0;

exit_device_create:
    class_destroy(chip->class);
exit_class_create:
    cdev_del(&chip->cdev);
exit_cdev_add:
    unregister_chrdev_region(chip->dev_id, 1);
exit:
    return ret;
}

static int led_hardware_init(struct loopled_data *chip)
{
    int index;
    struct platform_device *pdev = chip->pdev;

    // 1.获取LED模块中GPIO数目
    chip->leds_num = gpiod_count(&pdev->dev, "leds");
    if (chip->leds_num <= 0 
    || chip->leds_num > DEVICE_MAX_NUM) {
        dev_err(&pdev->dev, "gpio nums error:%d\n", chip->leds_num);
        return -EINVAL;
    }
    
    //2.获取"leds-gpios"属性中LED，设置GPIO输出并控制
    for (index=0; index<chip->leds_num; index++) {
        chip->desc[index] = devm_gpiod_get_index(&pdev->dev, "leds", index, GPIOD_OUT_LOW);
        if (IS_ERR(chip->desc[index])) {
            dev_err(&pdev->dev, "find gpio in dts failed!\n");
            return PTR_ERR(chip->desc[index]);
        }

        dev_info(&pdev->dev, "gpio %d init success", index);
    }

    dev_info(&pdev->dev, "gpio nums %d init success", chip->leds_num);

    return 0;
}

static int led_probe(struct platform_device *pdev)
{
    int ret;
    struct loopled_data *chip;
    
    // 1.申请loopled控制块
    chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
    if (!chip) {
        dev_err(&pdev->dev, "memory alloc failed!\n");
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, chip);
    chip->pdev = pdev;

    // 2.初始化loopled硬件设备
    ret = led_hardware_init(chip);
    if (ret) {
        dev_err(&pdev->dev, "hardware init faile, error:%d!\n", ret);
        return ret;
    }

    // 3.创建内核访问接口
    ret = led_device_create(chip);
    if (ret) {
        dev_err(&pdev->dev, "device create faile, error:%d!\n", ret);
        return ret;
    }

    dev_info(&pdev->dev, "driver init success!\n");
    return 0;
}

static int led_remove(struct platform_device *pdev)
{
    struct loopled_data *chip = platform_get_drvdata(pdev);

    device_destroy(chip->class, chip->dev_id);
    class_destroy(chip->class);

    cdev_del(&chip->cdev);
    unregister_chrdev_region(chip->dev_id, 1);
    
    dev_info(&pdev->dev, "driver release!\n");
    return 0;
}

static const struct of_device_id led_of_match[] = {
    { .compatible = "rmk,usr-loopled"},
    { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver platform_driver = {
    .driver = {
        .name = "loopled",
        .of_match_table = led_of_match,
    },
    .probe = led_probe,
    .remove = led_remove,
};

static int __init led_module_init(void)
{
    return platform_driver_register(&platform_driver);
}

static void __exit led_module_exit(void)
{
    platform_driver_unregister(&platform_driver);
}

module_init(led_module_init);
module_exit(led_module_exit);
MODULE_AUTHOR("wzdxf");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("platform driver for led");
MODULE_ALIAS("loopled driver");
