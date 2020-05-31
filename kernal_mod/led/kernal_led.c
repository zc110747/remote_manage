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
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/*设备相关参数*/
struct led_info
{
    dev_t dev_id;           /*总设备号*/
    int major;              /*主设备号*/
    int minor;              /*从设备号*/
    struct cdev cdev;       /*设备接口*/
    struct class *class;	/*设备类指针*/
	struct device *device;	/*设备指针*/
}led_driver_info;

#define DEFAULT_MAJOR                   0          /*默认主设备号*/
#define DEFAULT_MINOR                   0          /*默认从设备号*/
#define DEVICE_LED_CNT			        1		   /* 设备号个数 */
#define DEVICE_LED_NAME			        "led"      /* 设备名 */

#define LED_OFF                         0
#define LED_ON                          1

/*GPIO控制相关的寄存器*/
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;  

/*内部接口*/
static void led_gpio_init(void);
static void led_gpio_release(void);
static void led_switch(u8 status);

MODULE_AUTHOR("zc");				        //模块作者
MODULE_LICENSE("GPL v2");                     //模块许可协议
MODULE_DESCRIPTION("led driver");             //模块许描述
MODULE_ALIAS("led_driver");                   //模块别名

/**
 * 打开LED，获取LED资源
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 *
 * @return 设备打开处理结果，0表示正常
 */
int led_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &led_driver_info;
    return 0;
}

/**
 * 释放LED设备资源
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 * 
 * @return 设备关闭处理结果，0表示正常
 */
int led_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/**
 * 从LED设备读取数据
 * 
 * @param filp  要打开的设备文件(文件描述符)
 * @param buf   待读取数据缓冲的首地址
 * @param count 待读取数据的长度
 * @param f_ops 待读取数据的偏移值
 *
 * @return 正常返回读取的长度，负值表示读取失败
 */
ssize_t led_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

/**
 * 向LED设备写入数据
 * 
 * @param filp  要打开的设备文件(文件描述符)
 * @param buf   待写入数据缓冲的首地址
 * @param count 待写入数据的长度
 * @param f_ops 待写入数据的偏移值
 *
 * @return 正常返回写入的长度，负值表示写入失败
 */
ssize_t led_write(struct file *filp, const char __user *buf, size_t count,  loff_t *f_pos)
{
	int result;
	u8 databuf[2];

	result = copy_from_user(databuf, buf, count);
	if(result < 0) {
		printk(KERN_INFO"kernel write failed!\r\n");
		return -EFAULT;
	}
	
    /*利用数据操作LED*/
	led_switch(databuf[0]);
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

/**
 * 驱动加载时执行的初始化函数
 * 
 * @param NULL
 *
 * @return 驱动加载处理结果
 */
static int __init led_module_init(void)
{
    int result;

    led_driver_info.major = DEFAULT_MAJOR;
    led_driver_info.minor = DEFAULT_MINOR;

    /*在总线上创建设备*/    
    /*1.申请字符设备号*/
    if(led_driver_info.major){
        led_driver_info.dev_id = MKDEV(led_driver_info.major, led_driver_info.minor);
        result = register_chrdev_region(led_driver_info.dev_id, DEVICE_LED_CNT, DEVICE_LED_NAME);
    }
    else{
        result = alloc_chrdev_region(&led_driver_info.dev_id, 0, DEVICE_LED_CNT, DEVICE_LED_NAME);
        led_driver_info.major = MAJOR(led_driver_info.dev_id);
        led_driver_info.minor = MINOR(led_driver_info.dev_id);
    }
    if(result < 0){
        printk(KERN_INFO"dev alloc or set failed\r\n");	
        return result;
    }
    else{
        printk(KERN_INFO"dev alloc or set ok, major:%d, minor:%d\r\n", led_driver_info.major,  led_driver_info.minor);	
    }
    
    /*2.添加设备到相应总线上*/
    cdev_init(&led_driver_info.cdev, &led_fops);
    led_driver_info.cdev.owner = THIS_MODULE;
    result = cdev_add(&led_driver_info.cdev, led_driver_info.dev_id, DEVICE_LED_CNT);
    if(result != 0){
        unregister_chrdev_region(led_driver_info.dev_id, DEVICE_LED_CNT);
        printk(KERN_INFO"cdev add failed\r\n");
        return result;
    }else{
	printk(KERN_INFO"device add Success!\r\n");	
    }

    /* 4、创建类 */
	led_driver_info.class = class_create(THIS_MODULE, DEVICE_LED_NAME);
	if (IS_ERR(led_driver_info.class)) {
		printk(KERN_INFO"class create failed!\r\n");
		unregister_chrdev_region(led_driver_info.dev_id, DEVICE_LED_CNT);
		cdev_del(&led_driver_info.cdev);	
		return PTR_ERR(led_driver_info.class);
	}
	else{
		printk(KERN_INFO"class create successed!\r\n");
	}

	/* 5、创建设备 */
	led_driver_info.device = device_create(led_driver_info.class, NULL, led_driver_info.dev_id, NULL, DEVICE_LED_NAME);
	if (IS_ERR(led_driver_info.device)) {
		printk(KERN_INFO"device create failed!\r\n");
                unregister_chrdev_region(led_driver_info.dev_id, DEVICE_LED_CNT);       
                cdev_del(&led_driver_info.cdev);
		
		class_destroy(led_driver_info.class);
		return PTR_ERR(led_driver_info.device);
	}
	else{
		printk(KERN_INFO"device create successed!\r\n");
	}

    /*硬件初始化*/
    led_gpio_init();

    return 0;

}
module_init(led_module_init);

/**
 * 驱动释放时执行的退出函数
 * 
 * @param NULL    
 *
 * @return 驱动退出执行结果
 */
static void __exit led_module_exit(void)
{
    /* 注销字符设备驱动 */
    device_destroy(led_driver_info.class, led_driver_info.dev_id);
	class_destroy(led_driver_info.class);

	cdev_del(&led_driver_info.cdev);
	unregister_chrdev_region(led_driver_info.dev_id, DEVICE_LED_CNT);

    /*硬件资源释放*/
    led_gpio_release();
}
module_exit(led_module_exit);


/**
 * LED硬件初始化，引脚GPIO1_IO03
 * 
 * @param NULL
 *
 * @return NULL
 */
static void led_gpio_init(void)
{
    u32  value;

    /*1. 寄存器地址映射*/
    IMX6U_CCM_CCGR1 = ioremap(0X020C406C, 4);     //时钟使能 
	SW_MUX_GPIO1_IO03 = ioremap(0X020E0068, 4);   //复用功能设置
  	SW_PAD_GPIO1_IO03 = ioremap(0X020E02F4, 4);   //设置PAD的输出状态
	GPIO1_DR = ioremap(0X0209C000, 4);            //设置LED输出
	GPIO1_GDIR = ioremap(0X0209C004, 4);          //设置GPIO的状态

    /*2.时钟使能*/
    value = readl(IMX6U_CCM_CCGR1);
	value &= ~(3 << 26);	
	value |= (3 << 26);
	writel(value, IMX6U_CCM_CCGR1);
	printk("led write 0");

    /*3.复用功能设置*/
    writel(5, SW_MUX_GPIO1_IO03);

    /*4.引脚IO功能设置*/
    writel(0x10B0, SW_PAD_GPIO1_IO03);

    /*5.引脚输出功能配置*/
    value = readl(GPIO1_GDIR);
	value |= (1 << 3);	/* 设置新值 */
	writel(value, GPIO1_GDIR); 

    /*5.关闭LED显示，高电平关闭*/
    value = readl(GPIO1_DR);
	value |= (1 << 3);	
	writel(value, GPIO1_DR);

    printk(KERN_INFO"led hardware init ok\r\n");
}

/**
 * 释放硬件资源
 * 
 * @param NULL
 *
 * @return NULL
 */
static void led_gpio_release(void)
{
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03);
    iounmap(SW_PAD_GPIO1_IO03);
    iounmap(GPIO1_DR);
    iounmap(GPIO1_GDIR);
}

/**
 *LED灯开关切换
 * 
 * @param status  LED开关状态，1开启，0关闭
 *
 * @return NULL
 */
static void led_switch(u8 status)
{
    u32 value;
    value = readl(GPIO1_DR);

    switch(status)
    {
        case LED_OFF:
            printk(KERN_INFO"led off\r\n");
	        value |= (1 << 3);	
	        writel(value, GPIO1_DR);
            break;
        case LED_ON:
            printk(KERN_INFO"led on\r\n");
	        value &= ~(1 << 3);	
	        writel(value, GPIO1_DR);
            break;
        default:
            printk(KERN_INFO"Invalid LED Set");
            break;
    }
}
