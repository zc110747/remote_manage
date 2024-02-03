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

/*设备相关参数*/
struct key_info
{
    dev_t dev_id;               /*总设备号*/
    int major;                  /*主设备号*/
    int minor;                  /*从设备号*/
    struct cdev cdev;           /*设备接口*/
    struct class *class;	    /*设备类指针*/
	struct device *device;	    /*设备指针*/
    int key_gpio;               /*key对应的引脚接口*/
    int irq;            /*key对应的中断参数*/
    atomic_t key_interrupt;     /*key的中断状态*/
    atomic_t key_value;         /*读取的按键值*/
    atomic_long_t lock;         /*不允许按键被其它应用访问的lock函数*/
    
    struct timer_list key_timer;                /* 定义定时器*/
    wait_queue_head_t key_wait;				    /* 读等待队列头 */
    struct fasync_struct *key_async_queue;		/* 异步相关结构体 */

    int key_protect;
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

static int key_hw_init(struct platform_device *dev);
static int key_fasync(int fd, struct file *filp, int on);

static irqreturn_t key0_handler(int irq, void *dev_id)
{
    if(key_driver_info.key_protect == 0)
    {
        //printk(KERN_INFO"key0_handler!\r\n");
        key_driver_info.key_protect = 1;
        key_driver_info.key_timer.expires =jiffies + msecs_to_jiffies(10);
        
        add_timer(&key_driver_info.key_timer);
    }

	return IRQ_RETVAL(IRQ_HANDLED);
}

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
        {
            kill_fasync(&key_driver_info.key_async_queue, SIGIO, POLL_IN);
        }
    }

    key_driver_info.key_protect = 0;
}

static int key_open(struct inode *inode, struct file *filp)
{
    if (!atomic_dec_and_test(&key_driver_info.lock))
    {
        atomic_inc(&key_driver_info.lock); //atomic_dec_and_test会执行减操作，此处恢复
        return -EBUSY;
    }
    filp->private_data = &key_driver_info;
    return 0;
}

static int key_release(struct inode *inode, struct file *filp)
{
    atomic_inc(&key_driver_info.lock);
    key_fasync(-1, filp, 0);
    return 0;
}

static ssize_t key_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int result;
    u8 databuf;
    int ret;
    struct key_info *dev = (struct key_info*)filp->private_data;

    if (filp->f_flags & O_NONBLOCK)	
    {
		if (atomic_read(&dev->key_interrupt) == 0)	
			return -EAGAIN;
	}
    else 
    {					
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

static int key_fasync(int fd, struct file *filp, int on)
{
	return fasync_helper(fd, filp, on, &key_driver_info.key_async_queue);
}

static struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .open = key_open,
    .read = key_read,
    .release = key_release,
    .fasync = key_fasync,
};

static int key_hw_init(struct platform_device *pdev)
{
    int ret;
    struct device_node *nd = pdev->dev.of_node;

    key_driver_info.key_gpio = of_get_named_gpio(nd,  "key-gpio", 0);
    if (key_driver_info.key_gpio < 0) 
    {
        printk(KERN_INFO"Gpio %s no find\n",  "key-gpio");
        return -EINVAL;
    }

    devm_gpio_request(key_driver_info.device, key_driver_info.key_gpio, "key0");
    gpio_direction_input(key_driver_info.key_gpio);
    
    //cat /proc/interrupts可以查看是否增加中断向量
    key_driver_info.irq = irq_of_parse_and_map(nd, 0);
    ret = devm_request_irq(key_driver_info.device,
                            key_driver_info.irq, key0_handler, 
                            IRQF_TRIGGER_FALLING,       "key0", 
                            &(key_driver_info.dev_id));
    if (ret<0)
    {
        printk(KERN_INFO"key interrupt config error:%d\n", ret);
        return -EINVAL;
    }
    printk(KERN_INFO"key interrupt num:%d\n", key_driver_info.irq);

    atomic_set(&key_driver_info.key_interrupt, 0);
    atomic_set(&key_driver_info.key_value, KEYINVALD_VALUE);

    timer_setup(&key_driver_info.key_timer, key_timer_func, 0);
    init_waitqueue_head(&key_driver_info.key_wait);
    key_driver_info.key_protect = 0;

    return 0;
}

static int key_probe(struct platform_device *dev)
{
    int result;

    printk(KERN_INFO"Device and driver match, Do probe!\r\n");

    key_driver_info.major = DEFAULT_MAJOR;
    key_driver_info.minor = DEFAULT_MINOR;

    /*step1: make or alloc device id*/
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
        printk(KERN_INFO"Device alloc or set failed!\r\n");	
        goto exit;
    }
    printk(KERN_INFO"Device init ok, Major:%d, Minor:%d!\r\n", key_driver_info.major, key_driver_info.minor);	

    /*step2: add device id to device.*/
    cdev_init(&key_driver_info.cdev, &key_fops);
    key_driver_info.cdev.owner = THIS_MODULE;
    result = cdev_add(&key_driver_info.cdev, key_driver_info.dev_id, DEVICE_KEY_CNT);
    if (result != 0){
        printk(KERN_INFO"Device cdev_add failed!\r\n");
        goto exit_cdev_add;
    }
	printk(KERN_INFO"Device cdev_add success!\r\n");	
    
    /*step3: create device class.*/
	key_driver_info.class = class_create(THIS_MODULE, DEVICE_KEY_NAME);
	if (IS_ERR(key_driver_info.class))
    {
		printk(KERN_INFO"class create failed!\r\n");
        result = PTR_ERR(key_driver_info.class);
		goto exit_class_create;
	}
	printk(KERN_INFO"class create successed!\r\n");

    /*step4: create device by class.*/
	key_driver_info.device = device_create(key_driver_info.class, NULL, key_driver_info.dev_id, 
                                        NULL, DEVICE_KEY_NAME);
	if (IS_ERR(key_driver_info.device))
    {
		printk(KERN_INFO"device create failed!\r\n");
        result = PTR_ERR(key_driver_info.device);
		goto exit_device_create;
	}
	printk(KERN_INFO"device create successed!\r\n");

    /*step5: hardware init.*/
    result = key_hw_init(dev);
    if (result != 0)
    {
        printk(KERN_INFO"Key gpio init failed!\r\n");
        goto exit_key_hw_init;
    }

    atomic_set(&key_driver_info.lock, 1);
    printk(KERN_INFO"key driver init ok!\r\n");
    return 0;

exit_key_hw_init:
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

static void key_hw_release(struct platform_device *dev)
{
    /*release timer*/
    del_timer_sync(&key_driver_info.key_timer);

    /*release irq*/
    devm_free_irq(key_driver_info.device, key_driver_info.irq, &(key_driver_info.dev_id));
    key_driver_info.irq = -1;
    printk(KERN_INFO"key hw release!\r\n");
}

static int key_remove(struct platform_device *dev)
{
    key_hw_release(dev);

    device_destroy(key_driver_info.class, key_driver_info.dev_id);
	class_destroy(key_driver_info.class);

	cdev_del(&key_driver_info.cdev);
	unregister_chrdev_region(key_driver_info.dev_id, DEVICE_KEY_CNT);

    return 0;
}

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

static int __init key_module_init(void)
{
    int status;
    status = platform_driver_register(&key_driver);
    if (status != 0)
    {
        printk(KERN_INFO"%d\r\n", status);
    }

    return status;
}

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
