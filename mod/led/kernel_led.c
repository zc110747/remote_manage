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
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
//总线和硬件相关接口
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

typedef struct 
{
    dev_t dev_id;           /*总设备号*/
    int major;              /*主设备号*/
    int minor;              /*从设备号*/
    struct cdev cdev;       /*设备接口*/
    struct class *class;	/*设备类指针*/
    struct device *device;	/*设备指针*/
}device_info;

typedef struct 
{
    struct device_node *nd; /*设备节点*/
    int gpio;               /*led使用的GPIO编号*/
    int status;    
}hardware_info;

typedef struct 
{
    hardware_info   hw;
    device_info     dev;
}led_driver;
static led_driver driver;

#define LED_OFF                            0
#define LED_ON                             1

//自定义设备号
#define DEFAULT_MAJOR                       0          /*默认主设备号*/
#define DEFAULT_MINOR                       0          /*默认从设备号*/
#define DEVICE_NAME			                "led"     /* 设备名, 应用将以/dev/led访问 */

static void led_hardware_set(u8 status)
{
    switch(status)
    {
        case LED_OFF:
            printk(KERN_INFO"led off\n");
            gpio_set_value(driver.hw.gpio, 1);	
            driver.hw.status = 0;
            break;
        case LED_ON:
            printk(KERN_INFO"led on\n");
            gpio_set_value(driver.hw.gpio, 0);	
            driver.hw.status = 1;
            break;
        default:
            printk(KERN_INFO"Invalid led Set");
            break;
    }
}

int led_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &driver.dev;
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

    databuf[0] = driver.hw.status;
    result = copy_to_user(buf, databuf, 1);
    if(result < 0) {
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
	if(result < 0) {
		printk(KERN_INFO"kernel write failed!\n");
		return -EFAULT;
	}
	
    /*硬件操作*/
	led_hardware_set(databuf[0]);
	return 0;
}

long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd){
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

static int led_device_create(void)
{
    int result;

    driver.dev.major = DEFAULT_MAJOR;
    driver.dev.minor = DEFAULT_MINOR;

    /*1.申请字符设备号*/
    if(driver.dev.major){
        driver.dev.dev_id = MKDEV(driver.dev.major, driver.dev.minor);
        result = register_chrdev_region(driver.dev.dev_id, 1, DEVICE_NAME);
    }
    else{
        result = alloc_chrdev_region(&driver.dev.dev_id, 0, 1, DEVICE_NAME);
        driver.dev.major = MAJOR(driver.dev.dev_id);
        driver.dev.minor = MINOR(driver.dev.dev_id);
    }
    if(result < 0){
        printk(KERN_INFO"dev alloc or set failed\r\n");	
        goto exit;
    }
    else{
        printk(KERN_INFO"dev alloc or set ok, major:%d, minor:%d\r\n", driver.dev.major,  driver.dev.minor);	
    }
    
    /*2.添加设备到相应总线上*/
    cdev_init(&driver.dev.cdev, &led_fops);
    driver.dev.cdev.owner = THIS_MODULE;
    result = cdev_add(&driver.dev.cdev, driver.dev.dev_id, 1);
    if(result != 0){
        printk(KERN_INFO"cdev add failed\r\n");
        goto exit_cdev_add;
    }else{
	    printk(KERN_INFO"device add Success!\r\n");	
    }

    /* 4、创建类 */
	driver.dev.class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(driver.dev.class)) {
		printk(KERN_INFO"class create failed!\r\n");
        result = PTR_ERR(driver.dev.class);
        goto exit_class_create;
	}
	else{
		printk(KERN_INFO"class create successed!\r\n");
	}

	/* 5、创建设备 */
	driver.dev.device = device_create(driver.dev.class, NULL, driver.dev.dev_id, NULL, DEVICE_NAME);
	if (IS_ERR(driver.dev.device)) {
		printk(KERN_INFO"device create failed!\r\n");
        result = PTR_ERR(driver.dev.device);
		goto exit_device_create;
	}
	else{
		printk(KERN_INFO"device create successed!\r\n");
	}
    return 0;

exit_device_create:
	class_destroy(driver.dev.class);
exit_class_create:
    cdev_del(&driver.dev.cdev);
exit_cdev_add:
    unregister_chrdev_region(driver.dev.dev_id, 1);
exit:
    return result;
}

static int led_hardware_init(struct platform_device *pdev)
{
    int result;

    /*1.获取设备节点*/
    driver.hw.nd = of_find_node_by_path("/usr_led");
    if(driver.hw.nd == NULL){
        printk(KERN_INFO"led node no find\n");
        return -EINVAL;
    }

    /*2.获取设备树中的gpio属性编号*/
    driver.hw.gpio = of_get_named_gpio(driver.hw.nd, "led-gpio", 0);
    if(driver.hw.gpio < 0){
        printk(KERN_INFO"led-gpio no find\n");
        return -EINVAL;
    }

    /*3.设置对应GPIO输出*/
    result = gpio_direction_output(driver.hw.gpio, 1);
    if(result<0){
        printk(KERN_INFO"led gpio config error\n");
        return -EINVAL;
    }

    /*配置默认状态*/
    led_hardware_set(LED_OFF);
    return 0;
}

static int led_probe(struct platform_device *dev)
{
    int result;

    printk(KERN_INFO"device and driver match, do probe!\r\n");

    memset((char *)&driver, 0, sizeof(led_driver));

    //硬件初始化
    result = led_hardware_init(dev);
    if(result != 0)
    {
        printk(KERN_INFO"led hardware init failed, error:%d!\r\n", result);
        return result;
    }

    //设备创建!result
    result = led_device_create();
    if(result != 0)
    {
        printk(KERN_INFO"led device create failed, error:%d!\r\n", result);
        return result;
    }
    return 0;
}

static void led_device_destory(void)
{
    device_destroy(driver.dev.class, driver.dev.dev_id);
	class_destroy(driver.dev.class);

	cdev_del(&driver.dev.cdev);
	unregister_chrdev_region(driver.dev.dev_id, 1);   
}

static void led_hardware_release(void)
{
    devm_gpio_free(driver.dev.device, driver.hw.gpio);
}

static int led_remove(struct platform_device *dev)
{
    /*移除硬件驱动*/
    led_hardware_release();
    
    /*注销设备*/
    led_device_destory();
    return 0;
}

/* 查询设备树的匹配函数 */
static const struct of_device_id led_of_match[] = {
	{ .compatible = "usr-led" },
	{ /* Sentinel */ }
};

static struct platform_driver platform_driver = {
    .driver = {
        .name = "led",
        .of_match_table = led_of_match,
    },
    .probe = led_probe,
    .remove = led_remove,
};

static int __init led_module_init(void)
{
    int status;

    status = platform_driver_register(&platform_driver);
    if(status != 0)
    {
        printk(KERN_INFO"mdoule init failed:%d\r\n", status);
    }
    else
    {
        printk(KERN_INFO"mdoule init ok:%d\r\n", status);
    }
    return status;
}

static void __exit led_module_exit(void)
{
    platform_driver_unregister(&platform_driver);
}

module_init(led_module_init);
module_exit(led_module_exit);
MODULE_AUTHOR("wzdxf");				    //模块作者
MODULE_LICENSE("GPL v2");               //模块许可协议
MODULE_DESCRIPTION("platform driver for led");      //模块许描述
MODULE_ALIAS("led_driver");            //模块别名