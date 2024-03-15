/*
 * File      : kernel_led.c
 * This file is the driver for led i/o.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
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

#define DEVICE_NUM                         3

struct loopled_data
{
    /*device info*/
    dev_t dev_id;     
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct platform_device *pdev;

    /* hardware info */
    int gpio[DEVICE_NUM];
    int status[DEVICE_NUM];
};

#define LED_OFF                            0
#define LED_ON                             1

//自定义设备号
#define DEFAULT_MAJOR                      0 
#define DEFAULT_MINOR                      0    
#define DEVICE_NAME                        "loopled"

static void led_hardware_set(struct loopled_data *chip, u8 index, u8 status)
{
    struct platform_device *pdev;

    pdev = chip->pdev;

    switch (status)
    {
        case LED_ON:
            dev_info(&pdev->dev, "on\n");
            gpio_set_value(chip->gpio[index], 1);
            chip->status[index] = 1;
            break;
        case LED_OFF:
            dev_info(&pdev->dev, "off\n");
            gpio_set_value(chip->gpio[index], 0);
            chip->status[index] = 0;
            break;
    }
}

int led_open(struct inode *inode, struct file *filp)
{
    static struct loopled_data *chip;
    
    chip = container_of(inode->i_cdev, struct loopled_data, cdev);
    filp->private_data = chip;
    return 0;
}

int led_release(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t led_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int ret;
    struct loopled_data *chip;
    struct platform_device *pdev;

    if(count > DEVICE_NUM)
        return -1;

    chip = (struct loopled_data *)filp->private_data;
    pdev = chip->pdev;

    ret = copy_to_user(buf, chip->status, count);
    if (ret < 0) {
        dev_err(&pdev->dev, "read failed!\n");
        return -EFAULT;
    }
    return 1;
}

ssize_t led_write(struct file *filp, const char __user *buf, size_t size,  loff_t *f_pos)
{
    int ret;
    u8 data[2];
    struct loopled_data *chip;
    struct platform_device *pdev;

    chip = (struct loopled_data *)filp->private_data;
    pdev = chip->pdev;

    ret = copy_from_user(data, buf, 2);
    if (ret < 0 || data[0] >= DEVICE_NUM){
        dev_err(&pdev->dev, "write failed!\n");
        return -EFAULT;
    }

    led_hardware_set(chip, data[0], data[1]);
    return 0;
}

long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct platform_device *pdev;
    struct loopled_data *chip;
    
    chip = (struct loopled_data *)filp->private_data;
    pdev = chip->pdev;

    switch (cmd)
    {
        case 0:
            led_hardware_set(chip, 0, 0);
            break;
        case 1:
            led_hardware_set(chip, 0, 1);
            break;
        default:
            dev_err(&pdev->dev, "invalid command:%d!\n", cmd);
            return -ENOTTY;
    }

    return 0;
}

static struct file_operations led_fops = {
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

    if (major){
        chip->dev_id = MKDEV(major, minor);
        ret = register_chrdev_region(chip->dev_id, 1, DEVICE_NAME);
    }else {
        ret = alloc_chrdev_region(&chip->dev_id, 0, 1, DEVICE_NAME);
    }
    if (ret < 0){
        dev_err(&pdev->dev, "id alloc failed!\n");
        goto exit;
    }
    
    cdev_init(&chip->cdev, &led_fops);
    chip->cdev.owner = THIS_MODULE;
    ret = cdev_add(&chip->cdev, chip->dev_id, 1);
    if (ret){
        dev_err(&pdev->dev, "cdev add failed:%d!\n", ret);
        goto exit_cdev_add;
    }

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
    int ret, index;
    struct platform_device *pdev = chip->pdev;
    struct device_node *led_nd = pdev->dev.of_node;

    for (index=0; index<DEVICE_NUM; index++)
    {
        chip->gpio[index] = of_get_named_gpio(led_nd, "leds-gpio", index);
        if (chip->gpio[index] < 0) {
            dev_err(&pdev->dev, "find gpio in dts failed!\n");
            return -EINVAL;
        }
        ret = devm_gpio_request(&pdev->dev, chip->gpio[index], "led");
        if (ret < 0){
            dev_err(&pdev->dev, "request gpio failed!\n");
            return -EINVAL;   
        }

        gpio_direction_output(chip->gpio[index], 1);
        led_hardware_set(chip, index, LED_ON);

        dev_info(&pdev->dev, "hardware init finished, %s num %d", led_nd->name, chip->gpio[index]);
    }

    return 0;
}

static int led_probe(struct platform_device *pdev)
{
    int ret;
    static struct loopled_data *chip;

    chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
    if (!chip){
        dev_err(&pdev->dev, "memory alloc failed!\n");
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, chip);
    chip->pdev = pdev;

    ret = led_hardware_init(chip);
    if (ret){
        dev_err(&pdev->dev, "hardware init faile, error:%d!\n", ret);
        return ret;
    }

    ret = led_device_create(chip);
    if (ret){
        dev_err(&pdev->dev, "device create faile, error:%d!\n", ret);
        return ret;
    }

    dev_err(&pdev->dev, "driver init success!\n");
    return 0;
}

static int led_remove(struct platform_device *pdev)
{
    struct loopled_data *chip = platform_get_drvdata(pdev);

    device_destroy(chip->class, chip->dev_id);
    class_destroy(chip->class);

    cdev_del(&chip->cdev);
    unregister_chrdev_region(chip->dev_id, 1);
    
    dev_err(&pdev->dev, "driver release!\n");
    return 0;
}

//匹配的是根节点的compatible属性
static const struct of_device_id led_of_match[] = {
    { .compatible = "rmk,usr-loopled"},
    { /* Sentinel */ }
};

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
    platform_driver_register(&platform_driver);
    return 0;
}

static void __exit led_module_exit(void)
{
    platform_driver_unregister(&platform_driver);
}

module_init(led_module_init);
module_exit(led_module_exit);
MODULE_AUTHOR("wzdxf");                         //模块作者
MODULE_LICENSE("GPL v2");                       //模块许可协议
MODULE_DESCRIPTION("platform driver for led");  //模块许描述
MODULE_ALIAS("loopled driver");                  //模块别名
