////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      kernel_key_rwio.c
//          GPIO1_18
//
//  Purpose:
//      按键输入检测驱动。
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      4/3/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
/*
设备树说明
usr_key {
    compatible = "rmk,usr-key";
    pinctrl-0 = <&pinctrl_gpio_key>;
    key-gpios = <&gpio1 18 GPIO_ACTIVE_LOW>;
    interrupt-parent = <&gpio1>;
    interrupts = <18 IRQ_TYPE_EDGE_FALLING>;
    status = "okay";
};

pinctrl_gpio_key: gpio-key {
    fsl,pins = <
        MX6UL_PAD_UART1_CTS_B__GPIO1_IO18        0x40000000
    >;
};
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
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/poll.h>

enum key_status {
    KEY_PRESS = 0,      
    KEY_RELEASE,      
    KEY_KEEP,         
};

/*设备相关参数*/
struct key_data
{      
    struct platform_device *pdev;
    dev_t dev_id;     
    struct cdev cdev;
    struct class *class;
    struct device *device;

    int key_gpio;             
    int irq;    

    struct timer_list key_timer;
    atomic_t protect;
    atomic_t status;
    wait_queue_head_t r_wait;
    int key_value;
};

#define DEFAULT_MAJOR                   0         
#define DEFAULT_MINOR                   0         
#define DEVICE_KEY_CNT                  1         
#define DEVICE_NAME                     "key"

#define KEY0_VALUE                      0x01
#define KEYINVALD_VALUE                 0xFF

#define KEY_OFF                         0
#define KEY_ON                          1

int key_open(struct inode *inode, struct file *filp)
{
    static struct key_data *chip;
    
    chip = container_of(inode->i_cdev, struct key_data, cdev);
    filp->private_data = chip;
    return 0;
}

int key_release(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t key_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int ret;
    struct key_data *chip;
    struct platform_device *pdev;

    chip = (struct key_data *)filp->private_data;
    pdev = chip->pdev;

    if (filp->f_flags & O_NONBLOCK) {
        if (KEY_KEEP == atomic_read(&chip->status)) {
            return -EAGAIN;
        }
    } else {
        ret = wait_event_interruptible(chip->r_wait, KEY_KEEP != atomic_read(&chip->status));
        if (ret) {
            return ret;
        }
    }

    ret = copy_to_user(buf, &chip->status, sizeof(chip->status));
    if (ret) {
        dev_err(&pdev->dev, "read failed!\n");
        return -EFAULT;
    }
    
    atomic_set(&chip->status, KEY_KEEP);
    return ret;
}

static unsigned int key_poll(struct file *filp, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
    struct key_data *chip;
    struct platform_device *pdev;

    chip = (struct key_data *)filp->private_data;
    pdev = chip->pdev;

    poll_wait(filp, &chip->r_wait, wait);

    if(KEY_KEEP != atomic_read(&chip->status)) {
		mask = POLLIN | POLLRDNORM;	// 返回PLLIN
    }
    return mask;
}

static struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .open = key_open,
    .read = key_read,
    .poll = key_poll,
    .release = key_release,
};

static irqreturn_t key_handler(int irq, void *data)
{
    struct key_data* chip = (struct key_data*)data;

    if (atomic_read(&chip->protect) == 0) {
        atomic_set(&chip->protect, 1);
        mod_timer(&chip->key_timer, jiffies + msecs_to_jiffies(50));
    }

    return IRQ_RETVAL(IRQ_HANDLED);
}

void key_timer_func(struct timer_list *arg)
{
    unsigned char value;
    struct key_data *chip;
    struct platform_device *pdev;

    chip = container_of(arg, struct key_data, key_timer);
    pdev = chip->pdev;
    
    value = gpio_get_value(chip->key_gpio);
    if(value != chip->key_value) {
        chip->key_value = value;
        if(value == 1) {
            atomic_set(&chip->status, KEY_PRESS);
        } else {
            atomic_set(&chip->status, KEY_RELEASE);
        }
        wake_up_interruptible(&chip->r_wait);
    } else {
        atomic_set(&chip->status, KEY_KEEP);
    }

    dev_info(&pdev->dev, "key timer interrupt!");
    atomic_set(&chip->protect, 0);
}

static int key_hw_init(struct key_data *chip)
{
    int ret;
    struct platform_device *pdev = chip->pdev;  
    struct device_node *nd = pdev->dev.of_node;

    chip->key_gpio = of_get_named_gpio(nd, "key-gpios", 0);
    if (chip->key_gpio < 0){
        dev_err(&pdev->dev, "gpio %s no find\n",  "key-gpios");
        return -EINVAL;
    }

    devm_gpio_request(&pdev->dev, chip->key_gpio, "key0");
    gpio_direction_input(chip->key_gpio);
    chip->key_value = gpio_get_value(chip->key_gpio);

    //cat /proc/interrupts可以查看是否增加中断向量
    chip->irq = irq_of_parse_and_map(nd, 0);
    ret = devm_request_threaded_irq(&pdev->dev,
                            chip->irq, 
                            NULL, key_handler, 
                            IRQF_SHARED | IRQF_ONESHOT | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,       
                            "key0", 
                            (void *)chip);
    if (ret < 0) {
        dev_err(&pdev->dev, "key interrupt config error:%d\n", ret);
        return -EINVAL;
    }


    dev_info(&pdev->dev, "key interrupt num:%d\n", chip->irq);
    return 0;
}

static int key_device_create(struct key_data *chip)
{
    int ret;
    int major, minor;
    struct platform_device *pdev;

    major = DEFAULT_MAJOR;
    minor = DEFAULT_MINOR;
    pdev = chip->pdev;

    //1.创建或申请设备号
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

    //2.创建字符型设备，关联设备号并添加到内核中
    cdev_init(&chip->cdev, &key_fops);
    chip->cdev.owner = THIS_MODULE;
    ret = cdev_add(&chip->cdev, chip->dev_id, 1);
    if (ret) {
        dev_err(&pdev->dev, "cdev add failed:%d!\n", ret);
        goto exit_cdev_add;
    }

    //3.创建设备类和设备文件，关联设备号并添加到系统中
    chip->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(chip->class)) {
        dev_err(&pdev->dev, "class create failed!\n");
        ret = PTR_ERR(chip->class);
        goto exit_class_create;
    }
    chip->device = device_create(chip->class, NULL, chip->dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(chip->device)) {
        dev_err(&pdev->dev, "device create failed!\n");
        ret = PTR_ERR(chip->device);
        goto exit_device_create;
    }

    dev_info(&pdev->dev, "device create success!\n");
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

static int key_probe(struct platform_device *pdev)
{
    int result;
    struct key_data *chip = NULL;

    //1.申请按键管理控制块
    chip = devm_kzalloc(&pdev->dev, sizeof(struct key_data), GFP_KERNEL);
    if (!chip) {
        dev_err(&pdev->dev, "malloc error\n");
        return -ENOMEM;
    }
    chip->pdev = pdev;
    platform_set_drvdata(pdev, chip);

    //2.key相关引脚硬件初始化 
    result = key_hw_init(chip);
    if (result != 0) {
        dev_err(&pdev->dev, "Key gpio init failed!\r\n");
        return result;
    }

    //3.创建内核访问接口
    result = key_device_create(chip);
    if (result != 0) {
        dev_err(&pdev->dev, "device create failed!\n");
        return result;
    }

    //初始化全局变量
    atomic_set(&chip->status, KEY_KEEP);
    atomic_set(&chip->protect, 0);
	init_waitqueue_head(&chip->r_wait);
    
    //配置中断保护急促器
    timer_setup(&chip->key_timer, key_timer_func, 0);

    dev_info(&pdev->dev, "key driver init ok!\r\n");
    return 0;
}

static int key_remove(struct platform_device *pdev)
{
    struct key_data *chip = platform_get_drvdata(pdev);

    device_destroy(chip->class, chip->dev_id);
    class_destroy(chip->class);
    cdev_del(&chip->cdev);
    unregister_chrdev_region(chip->dev_id, 1);
    del_timer_sync(&chip->key_timer);

    dev_info(&pdev->dev, "key remove ok!\r\n");
    return 0;
}

static const struct of_device_id key_of_match[] = {
    { .compatible = "rmk,usr-key" },
    { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, key_of_match);

static struct platform_driver key_driver = {
    .driver = {
        .name = "kernel-key",
        .of_match_table = key_of_match,
    },
    .probe = key_probe,
    .remove = key_remove,
};

static int __init key_module_init(void)
{
    platform_driver_register(&key_driver);

    return 0;
}

static void __exit key_module_exit(void)
{
    platform_driver_unregister(&key_driver);
}

module_init(key_module_init);
module_exit(key_module_exit);
MODULE_AUTHOR("zc");                            //模块作者
MODULE_LICENSE("GPL v2");                       //模块许可协议
MODULE_DESCRIPTION("led driver");               //模块许描述
MODULE_ALIAS("key_driver");                     //模块别名
