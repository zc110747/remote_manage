/*
 * File      : key.c
 * This file is key input driver
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-7-21      zc           the first version
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
//中断和定时器相关
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
//异步事件相关
#include <linux/wait.h> 
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
//总线和硬件相关接口
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define _DBEUG  1
#if _DBEUG == 1
#define DRIVE_DEBUG printk
#else
#define DRIVE_DEBUG(...)
#endif

/*设备相关参数*/
struct key_info
{
    dev_t dev_id;               /*总设备号*/
    int major;                  /*主设备号*/
    int minor;                  /*从设备号*/
    struct cdev cdev;           /*设备接口*/
    struct class *class;	    /*设备类指针*/
	struct device *device;	    /*设备指针*/
    struct device_node *nd;     /*设备节点*/
    int key_gpio;               /*key对应的引脚接口*/
    int key_irq_num;            /*key对应的中断参数*/
    atomic_t key_interrupt;     /*key的中断状态*/
    atomic_t key_value;         /*读取的按键值*/
    atomic_long_t lock;         /*不允许按键被其它应用访问的lock函数*/
    
    struct timer_list key_timer;                /* 定义定时器*/
    wait_queue_head_t key_wait;				    /* 读等待队列头 */
    struct fasync_struct *key_async_queue;		/* 异步相关结构体 */
};

struct key_info key_driver_info;

#define DEFAULT_MAJOR                   0          /*默认主设备号*/
#define DEFAULT_MINOR                   0          /*默认从设备号*/
#define DEVICE_KEY_CNT			        1		   /* 设备号个数 */
#define DEVICE_KEY_NAME			        "key"     /* 设备名 */

#define KEY0_VALUE                      0x01
#define KEYINVALD_VALUE                 0xFF

#define key_OFF                         0
#define key_ON                          1

#define TREE_NODE_NAME                  "/usr_key"
#define TREE_GPIO_NAME                  "key-gpio"

/*内部接口*/
static int key_gpio_init(void);
static void key_gpio_release(void);
static int key_fasync(int fd, struct file *filp, int on);
static void tasklet_do_func(unsigned long data);

/**
 *事件触发的执行函数
 * 
 * @param data  事件触发传递的信息
 * 
 * @return NULL
 */
static void tasklet_do_func(unsigned long data)
{
   printk(KERN_INFO"key interrupt tasklet do:%ld!\r\n", data);
}
DECLARE_TASKLET_OLD(tasklet_func, tasklet_do_func);

/**
 *按键触发中断函数
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 * 
 * @return 设备关闭处理结果，0表示正常
 */
static irqreturn_t key0_handler(int irq, void *dev_id)
{
    key_driver_info.key_timer.expires =jiffies + msecs_to_jiffies(10);
    add_timer(&key_driver_info.key_timer);
    
    /*触发事件*/
    //tasklet_schedule(&tasklet_func);
	return IRQ_RETVAL(IRQ_HANDLED);
}

/**
 *用于去抖动的定时器回调函数
 * 
 * @param arg  定时器传递的参数
 *
 * @return NULL
 */
void key_timer_func(struct timer_list *arg)
{
    unsigned char value;

    value = gpio_get_value(key_driver_info.key_gpio);
    if (value == 0)
    {
        atomic_set(&key_driver_info.key_value, KEY0_VALUE);
        atomic_set(&key_driver_info.key_interrupt, 1);
        printk(KERN_INFO"key interrupt!\r\n");
    }

    if (atomic_read(&key_driver_info.key_interrupt))
    {
        if (key_driver_info.key_async_queue)
            kill_fasync(&key_driver_info.key_async_queue, SIGIO, POLL_IN);
    }
}

/**
 * LED硬件初始化，引脚GPIO1_IO18
 * 
 * @param NULL
 *
 * @return NULL
 */
static int key_gpio_init(void)
{
    int ret;

    /*1.获取设备节点*/
    key_driver_info.nd = of_find_node_by_path(TREE_NODE_NAME);
    if (key_driver_info.nd == NULL)
    {
        printk(KERN_INFO"Node %s no find!\n", TREE_NODE_NAME);
        return -EINVAL;
    }

    /*2.获取设备树中的gpio属性编号*/
    key_driver_info.key_gpio = of_get_named_gpio(key_driver_info.nd, TREE_GPIO_NAME, 0);
    if (key_driver_info.key_gpio < 0) 
    {
        printk(KERN_INFO"Gpio %s no find\n", TREE_GPIO_NAME);
        return -EINVAL;
    }

    /*3.设置key对应GPIO为输入状态*/
    devm_gpio_request(key_driver_info.device, key_driver_info.key_gpio, "key0");
    ret = gpio_direction_input(key_driver_info.key_gpio);
    if (ret < 0)
    {
        printk(KERN_INFO"Key gpio request and config error\n");
        return -EINVAL;
    }
    
    /*4.获取当前的中断向量号,并配置中断向量*/
    //cat /proc/interrupts可以查看是否增加中断向量
    key_driver_info.key_irq_num = irq_of_parse_and_map(key_driver_info.nd, 0);
    ret = devm_request_irq(key_driver_info.device,
                            key_driver_info.key_irq_num, key0_handler, 
                            IRQF_TRIGGER_FALLING,       "key0", 
                            &key_driver_info);
    if (ret<0)
    {
        printk(KERN_INFO"key interrupt config error\n");
        return -EINVAL;
    }
    printk(KERN_INFO"key interrupt num:%d\n", key_driver_info.key_irq_num);

    atomic_set(&key_driver_info.key_interrupt, 0);
    atomic_set(&key_driver_info.key_value, KEYINVALD_VALUE);

    /*5.创建用于去抖动判断的定时器*/
    timer_setup(&key_driver_info.key_timer, key_timer_func, 0);

    /*6.初始化队列*/
    init_waitqueue_head(&key_driver_info.key_wait);
    return 0;
}

/**
 * 释放硬件资源
 * 
 * @param NULL
 *
 * @return NULL
 */
static void key_gpio_release(void)
{
    // /*释放GPIO资源*/
    // devm_gpio_free(key_driver_info.device, key_driver_info.key_gpio);

    // /*释放中断资源*/
    // if (key_driver_info.key_irq_num > 0)
    // {
    //     devm_free_irq(key_driver_info.device, key_driver_info.key_irq_num, &key_driver_info);
    //     key_driver_info.key_irq_num = -1;
    // }
}

/**
 * 打开按键节点
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 *
 * @return 设备打开处理结果，0表示正常
 */
int key_open(struct inode *inode, struct file *filp)
{
    if (!atomic_dec_and_test(&key_driver_info.lock))
    {
        atomic_inc(&key_driver_info.lock); //atomic_dec_and_test会执行减操作，此处恢复
        return -EBUSY;
    }
    filp->private_data = &key_driver_info;
    return 0;
}

/**
 * 释放key设备资源
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 * 
 * @return 设备关闭处理结果，0表示正常
 */
int key_release(struct inode *inode, struct file *filp)
{
    atomic_inc(&key_driver_info.lock);
    key_fasync(-1, filp, 0);
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
ssize_t key_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int result;
    u8 databuf;
    int ret;
    struct key_info *dev = (struct key_info*)filp->private_data;

    if (filp->f_flags & O_NONBLOCK)	
    {
        /* 没有按键按下，返回-EAGAIN */
		if (atomic_read(&dev->key_interrupt) == 0)	
			return -EAGAIN;
	}
    else 
    {					
		/* 加入等待队列，等待被唤醒,也就是有按键按下 */
 		ret = wait_event_interruptible(dev->key_wait, atomic_read(&dev->key_interrupt)); 
		if (ret) 
        {
			return -EAGAIN;
		}
	}

    //读取引脚的状态
    databuf = atomic_read(&dev->key_value);
    atomic_set(&dev->key_value, KEYINVALD_VALUE);
    atomic_set(&dev->key_interrupt, 0);
    result = copy_to_user(buf, &databuf, 1);
    if (result < 0)
    {
		printk(KERN_INFO"kernel read failed!\r\n");
		return -EFAULT;
	}
    
    return 1;
}

/*
 * fasync函数，用于处理异步通知
 * 
 * @param - fd		: 文件描述符
 * @param - filp    : 要打开的设备文件(文件描述符)
 * @param - on      : 模式
 * @return          : 负数表示函数执行失败
 * 
 */
static int key_fasync(int fd, struct file *filp, int on)
{
	return fasync_helper(fd, filp, on, &key_driver_info.key_async_queue);
}

/* 设备操作函数 */
static struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .open = key_open,
    .read = key_read,
    .release = key_release,
    .fasync = key_fasync,
};

/*
 * @description		: flatform驱动的probe函数，当驱动与
 * 					  设备匹配以后此函数就会执行
 * @param - dev 	: platform设备
 * @return 			: 0，成功;其他负值,失败
 */
static int key_probe(struct platform_device *dev)
{
    int result;

    printk(KERN_INFO"Device and driver match, Do probe!\r\n");

    /*设置原子的保护操作*/
    atomic_set(&key_driver_info.lock, 1);

    key_driver_info.major = DEFAULT_MAJOR;
    key_driver_info.minor = DEFAULT_MINOR;

    /*1.申请字符设备号*/
    if (key_driver_info.major)
    {
        key_driver_info.dev_id = MKDEV(key_driver_info.major, key_driver_info.minor);
        result = register_chrdev_region(key_driver_info.dev_id, DEVICE_KEY_CNT, DEVICE_KEY_NAME);
    }
    else
    {
        result = alloc_chrdev_region(&key_driver_info.dev_id, 0, DEVICE_KEY_CNT, DEVICE_KEY_NAME);
        key_driver_info.major = MAJOR(key_driver_info.dev_id);
        key_driver_info.minor = MINOR(key_driver_info.dev_id);
    }

    if (result < 0)
    {
        DRIVE_DEBUG(KERN_INFO"Device alloc or set failed!\r\n");	
        goto exit;
    }
    DRIVE_DEBUG(KERN_INFO"Device init ok, Major:%d, Minor:%d!\r\n", 
                key_driver_info.major, key_driver_info.minor);	

    
    /*2.初始化设备信息，将设备接口和设备号进行关联*/
    cdev_init(&key_driver_info.cdev, &key_fops);
    key_driver_info.cdev.owner = THIS_MODULE;
    result = cdev_add(&key_driver_info.cdev, key_driver_info.dev_id, DEVICE_KEY_CNT);
    if (result != 0){
        DRIVE_DEBUG(KERN_INFO"Device cdev_add failed!\r\n");
        goto exit_cdev_add;
    }
	DRIVE_DEBUG(KERN_INFO"Device cdev_add success!\r\n");	
    
    /* 3、创建类 */
	key_driver_info.class = class_create(THIS_MODULE, DEVICE_KEY_NAME);
	if (IS_ERR(key_driver_info.class))
    {
		DRIVE_DEBUG(KERN_INFO"class create failed!\r\n");
        result = PTR_ERR(key_driver_info.class);
		goto exit_class_create;
	}
	DRIVE_DEBUG(KERN_INFO"class create successed!\r\n");

	/* 4、创建设备 */
	key_driver_info.device = device_create(key_driver_info.class, NULL, key_driver_info.dev_id, 
                                        NULL, DEVICE_KEY_NAME);
	if (IS_ERR(key_driver_info.device))
    {
		DRIVE_DEBUG(KERN_INFO"device create failed!\r\n");
        result = PTR_ERR(key_driver_info.device);
		goto exit_device_create;
	}
	DRIVE_DEBUG(KERN_INFO"device create successed!\r\n");

    //key按键相关初始化(带中断)
    result = key_gpio_init();
    if (result != 0)
    {
        printk(KERN_INFO"Key gpio init failed!\r\n");
        goto exit_key_gpio_init;
    }
    
    printk(KERN_INFO"key driver init ok!\r\n");
    return 0;

exit_key_gpio_init:
    device_destroy(key_driver_info.class, key_driver_info.dev_id);
exit_device_create:
	class_destroy(key_driver_info.class);
exit_class_create:
    cdev_del(&key_driver_info.cdev);
exit_cdev_add:
    unregister_chrdev_region(key_driver_info.dev_id, DEVICE_KEY_CNT);
exit:
    return result;
}

/*
 * @description		: 移除按键执行的函数
 * @param - dev 	: platform设备
 * @return 			: 0，成功;其他负值,失败
 */
static int key_remove(struct platform_device *dev)
{
    /* 注销字符设备驱动 */
    device_destroy(key_driver_info.class, key_driver_info.dev_id);
	class_destroy(key_driver_info.class);

	cdev_del(&key_driver_info.cdev);
	unregister_chrdev_region(key_driver_info.dev_id, DEVICE_KEY_CNT);
    
    key_gpio_release();
    return 0;
}

/* 匹配列表 */
static const struct of_device_id key_of_match[] = {
	{ .compatible = "rmk,usr-key" },
	{ /* Sentinel */ }
};

MODULE_DEVICE_TABLE(of, key_of_match);

static struct platform_driver key_driver = {
    .driver = {
        .name = DEVICE_KEY_NAME,
        .of_match_table = key_of_match,
    },
    .probe = key_probe,
    .remove = key_remove,
};

/**
 * 驱动加载时执行的初始化函数
 * 
 * @param NULL
 *
 * @return 驱动加载处理结果
 */
static int __init key_module_init(void)
{
    int status;
    status = platform_driver_register(&key_driver);
    if (status != 0)
    {
        printk("%d\r\n", status);
    }

    return status;
}

/**
 * 驱动释放时执行的退出函数
 * 
 * @param NULL    
 *
 * @return 驱动退出执行结果
 */
static void __exit key_module_exit(void)
{
    platform_driver_unregister(&key_driver);
}

module_init(key_module_init);
module_exit(key_module_exit);
MODULE_AUTHOR("zc");				            //模块作者
MODULE_LICENSE("GPL v2");                       //模块许可协议
MODULE_DESCRIPTION("led driver");               //模块许描述
MODULE_ALIAS("key_driver");                     //模块别名
