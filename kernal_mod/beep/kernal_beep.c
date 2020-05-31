/*
 * File      : kernal_beep.c
 * This file is beep kernal
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-15     zc           the first version
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
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/*设备相关参数*/
struct beep_info
{
    dev_t dev_id;           /*总设备号*/
    int major;              /*主设备号*/
    int minor;              /*从设备号*/
    struct cdev cdev;       /*设备接口*/
    struct class *class;	/*设备类指针*/
	struct device *device;	/*设备指针*/
    struct device_node *nd; /*设备节点*/
    int beep_gpio;          /*beep使用的GPIO编号*/
};

struct beep_info beep_driver_info;

#define TREE_NODE_NAME                  "/beep"    /*设备树名称*/

#define DEFAULT_MAJOR                   0          /*默认主设备号*/
#define DEFAULT_MINOR                   0          /*默认从设备号*/
#define DEVICE_BEEP_CNT			        1		   /* 设备号个数 */
#define DEVICE_BEEP_NAME			    "beep"     /* 设备名 */

#define BEEP_OFF                         0
#define BEEP_ON                          1

/*内部硬件操作的接口*/
static int beep_gpio_init(void);
static void beep_gpio_release(void);
static void beep_switch(u8 status);

/**
 * 打开BEEP，获取资源
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 *
 * @return 设备打开处理结果，0表示正常
 */
int beep_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &beep_driver_info;
    return 0;
}

/**
 * 释放BEEP设备资源
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 * 
 * @return 设备关闭处理结果，0表示正常
 */
int beep_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/**
 * 从BEEP设备读取数据
 * 
 * @param filp  要打开的设备文件(文件描述符)
 * @param buf   待读取数据缓冲的首地址
 * @param count 待读取数据的长度
 * @param f_ops 待读取数据的偏移值
 *
 * @return 正常返回读取的长度，负值表示读取失败
 */
ssize_t beep_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

/**
 * 向BEEP设备写入数据
 * 
 * @param filp  要打开的设备文件(文件描述符)
 * @param buf   待写入数据缓冲的首地址
 * @param count 待写入数据的长度
 * @param f_ops 待写入数据的偏移值
 *
 * @return 正常返回写入的长度，负值表示写入失败
 */
ssize_t beep_write(struct file *filp, const char __user *buf, size_t count,  loff_t *f_pos)
{
	int result;
	u8 databuf[2];

	result = copy_from_user(databuf, buf, count);
	if(result < 0) {
		printk(KERN_INFO"kernel write failed!\r\n");
		return -EFAULT;
	}
	
    /*利用数据操作BEEP*/
	beep_switch(databuf[0]);
	return 0;
}

/**
 * 执行控制指令的接口
 * 
 * @param filp 要打开的设备文件(文件描述符)
 * @param cmd  操作执行的指令
 * @param arg  操作处理的值
 *  
 * @return 返回控制指令的执行结果，0表示执行正常
 */
long beep_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd){
        case 0:
            beep_switch(0);
            break;
        case 1:
            beep_switch(1);
            break;
        default:
            printk(KERN_INFO"Invalid Cmd!\r\n");
            return -ENOTTY;
    }

    return 0;
}

/* 设备操作函数 */
static struct file_operations beep_fops = {
    .owner = THIS_MODULE,
    .open = beep_open,
    .read = beep_read,
    .write = beep_write,
    .unlocked_ioctl = beep_ioctl,
    .release = beep_release,
};

/**
 * 驱动加载时执行的初始化函数
 * 
 * @param NULL
 *
 * @return 驱动加载处理结果
 */
static int __init beep_module_init(void)
{
    int result;

    beep_driver_info.major = DEFAULT_MAJOR;
    beep_driver_info.minor = DEFAULT_MINOR;

    /*硬件初始化*/
    if((result = beep_gpio_init()) != 0){
        return result;
    }

    /*在总线上创建设备*/    
    /*1.申请字符设备号*/
    if(beep_driver_info.major){
        beep_driver_info.dev_id = MKDEV(beep_driver_info.major, beep_driver_info.minor);
        result = register_chrdev_region(beep_driver_info.dev_id, DEVICE_BEEP_CNT, DEVICE_BEEP_NAME);
    }
    else{
        result = alloc_chrdev_region(&beep_driver_info.dev_id, 0, DEVICE_BEEP_CNT, DEVICE_BEEP_NAME);
        beep_driver_info.major = MAJOR(beep_driver_info.dev_id);
        beep_driver_info.minor = MINOR(beep_driver_info.dev_id);
    }
    if(result < 0){
        printk(KERN_INFO"dev alloc or set failed\r\n");	
        return result;
    }
    else{
        printk(KERN_INFO"dev alloc or set ok, major:%d, minor:%d\r\n", beep_driver_info.major,  beep_driver_info.minor);	
    }
    
    /*2.添加设备到相应总线上*/
    cdev_init(&beep_driver_info.cdev, &beep_fops);
    beep_driver_info.cdev.owner = THIS_MODULE;
    result = cdev_add(&beep_driver_info.cdev, beep_driver_info.dev_id, DEVICE_BEEP_CNT);
    if(result != 0){
        unregister_chrdev_region(beep_driver_info.dev_id, DEVICE_BEEP_CNT);
        printk(KERN_INFO"cdev add failed\r\n");
        return result;
    }else{
	printk(KERN_INFO"device add Success!\r\n");	
    }

    /* 4、创建类 */
	beep_driver_info.class = class_create(THIS_MODULE, DEVICE_BEEP_NAME);
	if (IS_ERR(beep_driver_info.class)) {
		printk(KERN_INFO"class create failed!\r\n");
		unregister_chrdev_region(beep_driver_info.dev_id, DEVICE_BEEP_CNT);
		cdev_del(&beep_driver_info.cdev);	
		return PTR_ERR(beep_driver_info.class);
	}
	else{
		printk(KERN_INFO"class create successed!\r\n");
	}

	/* 5、创建设备 */
	beep_driver_info.device = device_create(beep_driver_info.class, NULL, beep_driver_info.dev_id, NULL, DEVICE_BEEP_NAME);
	if (IS_ERR(beep_driver_info.device)) {
		printk(KERN_INFO"device create failed!\r\n");
                unregister_chrdev_region(beep_driver_info.dev_id, DEVICE_BEEP_CNT);       
                cdev_del(&beep_driver_info.cdev);
		
		class_destroy(beep_driver_info.class);
		return PTR_ERR(beep_driver_info.device);
	}
	else{
		printk(KERN_INFO"device create successed!\r\n");
	}

    return 0;
}

/**
 * 驱动释放时执行的退出函数
 * 
 * @param NULL    
 *
 * @return 驱动退出执行结果
 */
static void __exit beep_module_exit(void)
{
    /* 注销字符设备驱动 */
    device_destroy(beep_driver_info.class, beep_driver_info.dev_id);
	class_destroy(beep_driver_info.class);

	cdev_del(&beep_driver_info.cdev);
	unregister_chrdev_region(beep_driver_info.dev_id, DEVICE_BEEP_CNT);

    /*硬件资源释放*/
    beep_gpio_release();
}

/**
 * BEEP硬件初始化，引脚信息从设备树获取
 * 
 * @param NULL
 *
 * @return 硬件初始化的处理结果，0表示正常
 */
static int beep_gpio_init(void)
{
    int ret;

    /*1.获取设备节点*/
    beep_driver_info.nd = of_find_node_by_path(TREE_NODE_NAME);
    if(beep_driver_info.nd == NULL){
        printk(KERN_INFO"beep node no find\n");
        return -EINVAL;
    }

    /*2.获取设备树中的gpio属性编号*/
    beep_driver_info.beep_gpio = of_get_named_gpio(beep_driver_info.nd, "beep-gpio", 0);
    if(beep_driver_info.beep_gpio < 0){
        printk(KERN_INFO"beep-gpio no find\n");
        return -EINVAL;
    }

    /*3.设置beep对应GPIO输出*/
    ret = gpio_direction_output(beep_driver_info.beep_gpio, 1);
    if(ret<0){
        printk(KERN_INFO"beep gpio config error\n");
        return -EINVAL;
    }

    printk(KERN_INFO"beep hardware init ok\r\n");
    return 0;
}

/**
 * beep资源释放时执行函数
 * 
 * @param NULL
 *
 * @return NULL
 */
static void beep_gpio_release(void)
{
}

/**
 *BEEP开关切换
 * 
 * @param status  BEEP开关状态，1开启，0关闭, 开启时为低电平
 *
 * @return NULL
 */
static void beep_switch(u8 status)
{
    u32 value;

    switch(status)
    {
        case BEEP_OFF:
            printk(KERN_INFO"beep off\r\n");
	        value |= (1 << 3);
            gpio_set_value(beep_driver_info.beep_gpio, 1);	
            break;
        case BEEP_ON:
            printk(KERN_INFO"beep on\r\n");
            gpio_set_value(beep_driver_info.beep_gpio, 0);	
	        value &= ~(1 << 3);	
            break;
        default:
            printk(KERN_INFO"Invalid Beep Set");
            break;
    }
}

module_init(beep_module_init);
module_exit(beep_module_exit);
MODULE_AUTHOR("zc");				    //模块作者
MODULE_LICENSE("GPL v2");               //模块许可协议
MODULE_DESCRIPTION("beep driver");      //模块许描述
MODULE_ALIAS("beep_driver");            //模块别名
