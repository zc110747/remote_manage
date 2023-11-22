/*
 * File      : led.c
 * This file is led driver
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
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/*led device information*/
struct led_device
{
    /* device node in dts */
    struct device_node *nd;    

    /* gpio number by dts */
    int led_gpio;         

    /* led show status saved */
    int led_status;  

    /* lock for device */        
    atomic_long_t lock;        
};

/*led device information for ker*/
struct led_kernal
{
    dev_t dev_id;             
    int major;                 
    int minor;                 
    struct cdev cdev;          
    struct class *class;	  
	struct device *device;	    
};

static struct led_device device_info;
static struct led_kernal led_kernal_info;

#define DEFAULT_MAJOR                   0          /*默认主设备号*/
#define DEFAULT_MINOR                   0          /*默认从设备号*/
#define DEVICE_LED_CNT			        1		   /* 设备号个数 */
#define DEVICE_LED_NAME			        "led"      /* 设备名 */

#define LED_OFF                         0
#define LED_ON                          1

#define TREE_NODE_NAME                  "/usr_gpios/led"
#define TREE_GPIO_NAME                  "led-gpio"

static int led_gpio_init(void);
static void led_gpio_release(void);
static void led_switch(u8 status);

int led_open(struct inode *inode, struct file *filp)
{
    if(!atomic_dec_and_test(&device_info.lock))
    {
        atomic_inc(&device_info.lock); //atomic_dec_and_test会执行减操作，此处恢复
        return -EBUSY;
    }
    filp->private_data = &device_info;
    return 0;
}

int led_release(struct inode *inode, struct file *filp)
{
    atomic_inc(&device_info.lock);
    return 0;
}

ssize_t led_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int result;
    u8 databuf[2];

    databuf[0] = device_info.led_status;

    result = copy_to_user(buf, databuf, 1);
    if(result < 0) {
		printk(KERN_INFO"kernel read failed!\r\n");
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
		printk(KERN_INFO"kernel write failed!\r\n");
		return -EFAULT;
	}

	led_switch(databuf[0]);

	return 0;
}

long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd){
        case 0:
            led_switch(0);
            break;
        case 1:
            led_switch(1);
            break;
        default:
            printk(KERN_INFO"Invalid Cmd!\r\n");
            return -ENOTTY;
    }

    return 0;
}

/* 设备操作函数 */
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .unlocked_ioctl = led_ioctl,
    .release = led_release,
};

static int __init led_module_init(void)
{
    int result;

    led_kernal_info.major = DEFAULT_MAJOR;
    led_kernal_info.minor = DEFAULT_MINOR;

    result = led_gpio_init();
    if(result != 0)
    {
        printk(KERN_INFO"led gpio init failed\n0");
        return result;
    }

    /*在总线上创建设备*/
    /*1.申请字符设备号*/
    if(led_kernal_info.major){
        led_kernal_info.dev_id = MKDEV(led_kernal_info.major, led_kernal_info.minor);
        result = register_chrdev_region(led_kernal_info.dev_id, DEVICE_LED_CNT, DEVICE_LED_NAME);
    }
    else{
        result = alloc_chrdev_region(&led_kernal_info.dev_id, 0, DEVICE_LED_CNT, DEVICE_LED_NAME);
        led_kernal_info.major = MAJOR(led_kernal_info.dev_id);
        led_kernal_info.minor = MINOR(led_kernal_info.dev_id);
    }
    if(result < 0){
        printk(KERN_INFO"dev alloc or set failed\r\n");
        goto fail_;
    }
    else{
        printk(KERN_INFO"dev alloc or set ok, major:%d, minor:%d\r\n", led_kernal_info.major,  led_kernal_info.minor);
    }

    /*2.初始化设备信息，将设备接口和设备号进行关联*/
    cdev_init(&led_kernal_info.cdev, &led_fops);
    led_kernal_info.cdev.owner = THIS_MODULE;
    result = cdev_add(&led_kernal_info.cdev, led_kernal_info.dev_id, DEVICE_LED_CNT);
    if(result != 0){
        printk(KERN_INFO"cdev add failed\r\n");
        goto cdev_add_fail_;
    }else{
	    printk(KERN_INFO"device add Success!\r\n");
    }

    /* 3、创建类 */
	led_kernal_info.class = class_create(THIS_MODULE, DEVICE_LED_NAME);
	if (IS_ERR(led_kernal_info.class)) {
		printk(KERN_INFO"class create failed!\r\n");
        goto class_create_fail_;
	}
	else{
		printk(KERN_INFO"class create successed!\r\n");
	}

	/* 4、创建设备 */
	led_kernal_info.device = device_create(led_kernal_info.class, NULL, led_kernal_info.dev_id, NULL, DEVICE_LED_NAME);
	if (IS_ERR(led_kernal_info.device)) {
		printk(KERN_INFO"device create failed!\r\n");
        goto device_create_fail_;
	}
	else{
		printk(KERN_INFO"device create successed!\r\n");
	}

    printk(KERN_INFO"Led Driver Init Ok!\r\n");
    return 0;

device_create_fail_:
    class_destroy(led_kernal_info.class);
class_create_fail_:
    cdev_del(&led_kernal_info.cdev);
cdev_add_fail_:
    unregister_chrdev_region(led_kernal_info.dev_id, DEVICE_LED_CNT);
fail_:
    return -1;
}

static void __exit led_module_exit(void)
{
    device_destroy(led_kernal_info.class, led_kernal_info.dev_id);
	class_destroy(led_kernal_info.class);
	cdev_del(&led_kernal_info.cdev);
	unregister_chrdev_region(led_kernal_info.dev_id, DEVICE_LED_CNT);

    led_gpio_release();
}

static int led_gpio_init(void)
{
    int ret;

    atomic_set(&device_info.lock, 1);

    /*1.获取设备节点*/
    device_info.nd = of_find_node_by_path(TREE_NODE_NAME);
    if(device_info.nd == NULL){
        printk(KERN_INFO"led node no find\n");
        return -EINVAL;
    }

    /*2.获取设备树中的gpio属性编号*/
    device_info.led_gpio = of_get_named_gpio(device_info.nd, TREE_GPIO_NAME, 0);
    if(device_info.led_gpio < 0){
        printk(KERN_INFO"led-gpio no find\n");    /* 注销字符设备驱动 */
        return -EINVAL;
    }

    /*3.设置beep对应GPIO输出*/
    ret = gpio_direction_output(device_info.led_gpio, 1);
    if(ret<0){
        printk(KERN_INFO"led gpio config error\n");
        return -EINVAL;
    }

    led_switch(LED_OFF);

    printk(KERN_INFO"led tree hardware init ok\r\n");    /* 注销字符设备驱动 */

    return 0;
}

/* 注销字符设备驱动 */
static void led_gpio_release(void)
{
}

static void led_switch(u8 status)
{
    switch(status)
    {
        case LED_OFF:
            printk(KERN_INFO"led off\r\n");
            gpio_set_value(device_info.led_gpio, 1);
            device_info.led_status = 0;
            break;
        case LED_ON:
            printk(KERN_INFO"led on\r\n");
	        gpio_set_value(device_info.led_gpio, 0);
	        device_info.led_status = 1;
            break;
        default:
            printk(KERN_INFO"Invalid LED Set");
            break;
    }
}

module_init(led_module_init);
module_exit(led_module_exit);
MODULE_AUTHOR("zc");				            //模块作者
MODULE_LICENSE("GPL v2");                       //模块许可协议
MODULE_DESCRIPTION("led driver");               //模块许描述
MODULE_ALIAS("led_driver");                     //模块别名
