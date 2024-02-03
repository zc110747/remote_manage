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

struct device_info
{
    dev_t dev_id;           /*总设备号*/
    struct cdev cdev;       /*设备接口*/
    struct class *class;    /*设备类指针*/
    struct device *device;  /*设备指针*/
};

struct hardware_info
{
    int gpio;
    int status;    
};

struct led_driver
{
    struct hardware_info   hw;
    struct device_info     dev;
};
static struct led_driver *chip;

#define LED_OFF                            0
#define LED_ON                             1

//自定义设备号
#define DEFAULT_MAJOR                       0       /*默认主设备号*/
#define DEFAULT_MINOR                       0       /*默认从设备号*/
#define DEVICE_NAME                         "led"   /* 设备名, 应用将以/dev/led访问 */

static void led_hardware_set(u8 status)
{
    switch (status)
    {
        case LED_OFF:
            printk(KERN_INFO"led off\n");
            gpio_set_value(chip->hw.gpio, 1);
            chip->hw.status = 0;
            break;
        case LED_ON:
            printk(KERN_INFO"led on\n");
            gpio_set_value(chip->hw.gpio, 0);
            chip->hw.status = 1;
            break;
        default:
            printk(KERN_INFO"Invalid led Set");
            break;
    }
}

int led_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &chip->dev;
    return 0;
}

int led_release(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t led_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int result;
    u8 databuf[2];

    databuf[0] = chip->hw.status;
    result = copy_to_user(buf, databuf, 1);
    if (result < 0) 
    {
        printk(KERN_INFO"kernel read failed!\n");
        return -EFAULT;
    }
    return 1;
}

ssize_t led_write(struct file *filp, const char __user *buf, size_t count,  loff_t *f_pos)
{
    int result;
    u8 databuf[2];

    result = copy_from_user(databuf, buf, count);
    if (result < 0)
    {
        printk(KERN_INFO"kernel write failed!\n");
        return -EFAULT;
    }

    /*硬件操作*/
    led_hardware_set(databuf[0]);
    return 0;
}

long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
        case 0:
            led_hardware_set(0);
            break;
        case 1:
            led_hardware_set(1);
            break;
        default:
            printk(KERN_INFO"Invalid Cmd!\r\n");
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

static int led_device_create(struct led_driver *chip)
{
    int result;
    int major = DEFAULT_MAJOR;
    int minor = DEFAULT_MINOR;

    /*1.申请字符设备号*/
    if (major)
    {
        chip->dev.dev_id = MKDEV(major, minor);
        result = register_chrdev_region(chip->dev.dev_id, 1, DEVICE_NAME);
    }
    else
    {
        result = alloc_chrdev_region(&chip->dev.dev_id, 0, 1, DEVICE_NAME);
    }
    if (result < 0)
    {
        printk(KERN_INFO"dev alloc or set failed\r\n");
        goto exit;
    }
    
    /*2.添加设备到相应总线上*/
    cdev_init(&chip->dev.cdev, &led_fops);
    chip->dev.cdev.owner = THIS_MODULE;
    result = cdev_add(&chip->dev.cdev, chip->dev.dev_id, 1);
    if (result != 0)
    {
        printk(KERN_INFO"cdev add failed\r\n");
        goto exit_cdev_add;
    }
    else
    {
        printk(KERN_INFO"device add Success!\r\n");
    }

    /* 4、创建类 */
    chip->dev.class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(chip->dev.class)) 
    {
        printk(KERN_INFO"class create failed!\r\n");
        result = PTR_ERR(chip->dev.class);
        goto exit_class_create;
    }
    else
    {
        printk(KERN_INFO"class create successed!\r\n");
    }

    /* 5、创建设备 */
    chip->dev.device = device_create(chip->dev.class, NULL, chip->dev.dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(chip->dev.device)) 
    {
        printk(KERN_INFO"device create failed!\r\n");
        result = PTR_ERR(chip->dev.device);
        goto exit_device_create;
    }
    else
    {
        printk(KERN_INFO"device create successed!\r\n");
    }
    return 0;

exit_device_create:
    class_destroy(chip->dev.class);
exit_class_create:
    cdev_del(&chip->dev.cdev);
exit_cdev_add:
    unregister_chrdev_region(chip->dev.dev_id, 1);
exit:
    return result;
}

static int led_hardware_init(struct platform_device *pdev)
{
    struct device_node *led_nd = pdev->dev.of_node;

    chip->hw.gpio = of_get_named_gpio(led_nd, "led-gpio", 0);
    if (chip->hw.gpio < 0)
    {
        printk(KERN_ERR"Chip GPIO DeviceTree not found!\n");
        return -EINVAL;
    }
    printk(KERN_INFO"find node:%s, gpio:%d", led_nd->name, chip->hw.gpio);

    devm_gpio_request(chip->dev.device, chip->hw.gpio, "led");
    gpio_direction_output(chip->hw.gpio, 1);
    led_hardware_set(LED_OFF);

    gpio_export(chip->hw.gpio, true);
    gpio_export_link(chip->dev.device, "gpio-led0", chip->hw.gpio);
    return 0;
}

static int led_probe(struct platform_device *pdev)
{
    int result;

    printk(KERN_INFO"device and driver match, do probe!\r\n");
    
    /*step1: alloc memory for led driver manage*/
    chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
    if(!chip)
    {
        printk(KERN_ERR"memory alloc failed!");
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, chip);

    //step1: device create
    result = led_device_create(chip);
    if (result != 0)
    {
        printk(KERN_INFO"led device create failed, error:%d!\r\n", result);
        return result;
    }

    //step2: hardware init
    result = led_hardware_init(pdev);
    if (result != 0)
    {
        printk(KERN_INFO"led hardware init failed, error:%d!\r\n", result);
        return result;
    }
    return 0;
}

static void led_device_destory(struct led_driver *chip)
{
    device_destroy(chip->dev.class, chip->dev.dev_id);
    class_destroy(chip->dev.class);

    cdev_del(&chip->dev.cdev);
    unregister_chrdev_region(chip->dev.dev_id, 1);   
}

static void led_hardware_release(struct led_driver *chip)
{
    printk(KERN_INFO"led hardware info release!\r\n");
}

static int led_remove(struct platform_device *pdev)
{
    struct led_driver *chip = platform_get_drvdata(pdev);

    led_hardware_release(chip);
    
    led_device_destory(chip);
    return 0;
}

static const struct platform_device_id led_id[] = {
    { .name = "usr_led" },
    { /* sentinel */ },
};

//匹配的是根节点的compatible属性
static const struct of_device_id led_of_match[] = {
    { .compatible = "rmk,usr-led"},
    { /* Sentinel */ }
};

static struct platform_driver platform_driver = {
    .driver = {
        .name = "usr_led",
        .of_match_table = led_of_match,
    },
    .probe = led_probe,
    .remove = led_remove,
    .id_table = led_id,
};

static int __init led_module_init(void)
{
    int status;

    status = platform_driver_register(&platform_driver);
    if (status != 0)
    {
        printk(KERN_ERR"mdoule init failed:%d\r\n", status);
    }
    return status;
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
MODULE_ALIAS("led_driver");                     //模块别名
