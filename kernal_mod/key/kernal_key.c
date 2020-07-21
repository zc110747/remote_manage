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
    atomic_long_t lock;         /*不允许LED被其它应用访问的lock函数*/
};

struct key_info key_driver_info;
#define DEFAULT_MAJOR                   0          /*默认主设备号*/
#define DEFAULT_MINOR                   0          /*默认从设备号*/
#define DEVICE_key_CNT			        1		   /* 设备号个数 */
#define DEVICE_key_NAME			        "led"      /* 设备名 */

#define key_OFF                         0
#define key_ON                          1

#define TREE_NODE_NAME                  "/key"
#define TREE_GPIO_NAME                  "key-gpio"

/*内部接口*/
static int key_gpio_init(void);
static void key_gpio_release(void);

/**
 * 打开LED，获取LED资源
 * 
 * @param inode  驱动内的节点信息
 * @param filp   要处理的设备文件(文件描述符)
 *
 * @return 设备打开处理结果，0表示正常
 */
int key_open(struct inode *inode, struct file *filp)
{
    if(!atomic_dec_and_test(&key_driver_info.lock))
    {
        atomic_inc(&key_driver_info.lock); //atomic_dec_and_test会执行减操作，此处恢复
        return -EBUSY;
    }
    key_gpio_init();
    filp->private_data = &key_driver_info;
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
int key_release(struct inode *inode, struct file *filp)
{
    key_gpio_release();
    atomic_inc(&key_driver_info.lock);
    return 0;
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
    if(key_driver_info.nd == NULL){
        printk(KERN_INFO"%s node no find\n", TREE_NODE_NAME);
        return -EINVAL;
    }

    /*2.获取设备树中的gpio属性编号*/
    key_driver_info.key_gpio = of_get_named_gpio(key_driver_info.nd, TREE_GPIO_NAME, 0);
    if(key_driver_info.key_gpio < 0){
        printk(KERN_INFO"%s no find\n", TREE_GPIO_NAME);
        return -EINVAL;
    }

    /*3.设置key对应GPIO为输入状态*/
    gpio_request(key_driver_info.key_gpio, "key0");
    ret = gpio_direction_input(key_driver_info.key_gpio);
    if(ret<0){
        printk(KERN_INFO"key gpio config error\n");
        return -EINVAL;
    }
    
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
    gpio_free(key_driver_info.key_gpio);
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
    struct key_info* p_key_info = (struct key_info*)filp->private_data;

    //读取引脚的状态
    databuf = gpio_get_value(p_key_info->key_gpio);

    result = copy_to_user(buf, &databuf, 1);
    if(result < 0) {
		printk(KERN_INFO"kernel read failed!\r\n");
		return -EFAULT;
	}
    return 1;
}

/* 设备操作函数 */
static struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .open = key_open,
    .read = key_read,
    .release = key_release,
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
    int result;

    /*设置原子的保护操作*/
    atomic_set(&key_driver_info.lock, 1);

    key_driver_info.major = DEFAULT_MAJOR;
    key_driver_info.minor = DEFAULT_MINOR;

    /*在总线上创建设备*/    
    /*1.申请字符设备号*/
    if(key_driver_info.major){
        key_driver_info.dev_id = MKDEV(key_driver_info.major, key_driver_info.minor);
        result = register_chrdev_region(key_driver_info.dev_id, DEVICE_key_CNT, DEVICE_key_NAME);
    }
    else{
        result = alloc_chrdev_region(&key_driver_info.dev_id, 0, DEVICE_key_CNT, DEVICE_key_NAME);
        key_driver_info.major = MAJOR(key_driver_info.dev_id);
        key_driver_info.minor = MINOR(key_driver_info.dev_id);
    }
    if(result < 0){
        printk(KERN_INFO"dev alloc or set failed\r\n");	
        return result;
    }
    else{
        printk(KERN_INFO"dev alloc or set ok, major:%d, minor:%d\r\n", key_driver_info.major,  key_driver_info.minor);	
    }
    
    /*2.初始化设备信息，将设备接口和设备号进行关联*/
    cdev_init(&key_driver_info.cdev, &key_fops);
    key_driver_info.cdev.owner = THIS_MODULE;
    result = cdev_add(&key_driver_info.cdev, key_driver_info.dev_id, DEVICE_key_CNT);
    if(result != 0){
        unregister_chrdev_region(key_driver_info.dev_id, DEVICE_key_CNT);
        printk(KERN_INFO"cdev add failed\r\n");
        return result;
    }else{
	    printk(KERN_INFO"device add Success!\r\n");	
    }

    /* 3、创建类 */
	key_driver_info.class = class_create(THIS_MODULE, DEVICE_key_NAME);
	if (IS_ERR(key_driver_info.class)) {
		printk(KERN_INFO"class create failed!\r\n");
		unregister_chrdev_region(key_driver_info.dev_id, DEVICE_key_CNT);
		cdev_del(&key_driver_info.cdev);	
		return PTR_ERR(key_driver_info.class);
	}
	else{
		printk(KERN_INFO"class create successed!\r\n");
	}

	/* 4、创建设备 */
	key_driver_info.device = device_create(key_driver_info.class, NULL, key_driver_info.dev_id, NULL, DEVICE_key_NAME);
	if (IS_ERR(key_driver_info.device)) {
		printk(KERN_INFO"device create failed!\r\n");
                unregister_chrdev_region(key_driver_info.dev_id, DEVICE_key_CNT);       
                cdev_del(&key_driver_info.cdev);
		
		class_destroy(key_driver_info.class);
		return PTR_ERR(key_driver_info.device);
	}
	else{
		printk(KERN_INFO"device create successed!\r\n");
	}

    printk(KERN_INFO"Led Driver Init Ok!\r\n");
    return 0;
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
    /* 注销字符设备驱动 */
    device_destroy(key_driver_info.class, key_driver_info.dev_id);
	class_destroy(key_driver_info.class);

	cdev_del(&key_driver_info.cdev);
	unregister_chrdev_region(key_driver_info.dev_id, DEVICE_key_CNT);
}

module_init(key_module_init);
module_exit(key_module_exit);
MODULE_AUTHOR("zc");				            //模块作者
MODULE_LICENSE("GPL v2");                       //模块许可协议
MODULE_DESCRIPTION("led driver");               //模块许描述
MODULE_ALIAS("key_driver");                     //模块别名
