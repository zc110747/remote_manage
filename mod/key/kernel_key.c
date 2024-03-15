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
#include <linux/input.h>

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
struct key_data
{
    dev_t dev_id;              
    struct cdev cdev;         
    struct class *class;        
    struct device *device;       
    struct platform_device *pdev;
    struct input_dev *input_dev;

    int key_gpio;             
    int irq;    
    
    int key_event; 
    struct timer_list key_timer;
    int key_protect;
};

#define DEFAULT_MAJOR                   0         
#define DEFAULT_MINOR                   0         
#define DEVICE_KEY_CNT                  1         
#define DEVICE_NAME                     "key"

#define KEY0_VALUE                      0x01
#define KEYINVALD_VALUE                 0xFF

#define KEY_OFF                         0
#define KEY_ON                          1

static irqreturn_t key_handler(int irq, void *data)
{
    struct key_data* chip = (struct key_data*)data;

    if(chip->key_protect == 0)
    {
        chip->key_protect = 1;
        chip->key_timer.expires =jiffies + msecs_to_jiffies(10); 
        add_timer(&chip->key_timer);
    }

    return IRQ_RETVAL(IRQ_HANDLED);
}

void key_timer_func(struct timer_list *arg)
{
    unsigned char value;
    struct key_data *chip;
    struct platform_device *pdev;

    chip = container_of(arg, struct key_data, key_timer);
    value = gpio_get_value(chip->key_gpio);
    pdev = chip->pdev;

    if (value == 0)
    {
        input_report_key(chip->input_dev, chip->key_event, KEY_ON);
        input_sync(chip->input_dev);
    }
    else
    {
        input_report_key(chip->input_dev, chip->key_event, KEY_OFF);
        input_sync(chip->input_dev);            
    }
    chip->key_protect = 0;
}

static int key_hw_init(struct key_data *chip)
{
    int ret;
    struct platform_device *pdev = chip->pdev;  
    struct device_node *nd = pdev->dev.of_node;

    chip->key_gpio = of_get_named_gpio(nd, "key-gpio", 0);
    if (chip->key_gpio < 0){
        dev_err(&pdev->dev, "gpio %s no find\n",  "key-gpio");
        return -EINVAL;
    }

    devm_gpio_request(chip->device, chip->key_gpio, "key0");
    gpio_direction_input(chip->key_gpio);
    
    //cat /proc/interrupts可以查看是否增加中断向量
    chip->irq = irq_of_parse_and_map(nd, 0);
    ret = devm_request_irq (chip->device,
                            chip->irq, 
                            key_handler, 
                            IRQF_TRIGGER_FALLING,       
                            "key0", 
                            (void *)chip);
    if (ret<0){
        dev_err(&pdev->dev, "key interrupt config error:%d\n", ret);
        return -EINVAL;
    }

    timer_setup(&chip->key_timer, key_timer_func, 0);
    chip->key_protect = 0;

    dev_info(&pdev->dev, "key interrupt num:%d\n", chip->irq);
    return 0;
}

static int key_device_create(struct key_data *chip)
{
    int result;
    struct platform_device *pdev = chip->pdev;

    chip->input_dev = devm_input_allocate_device(&pdev->dev);
    if(!chip->input_dev)
    {
		dev_err(&pdev->dev, "failed to allocate input device\n");
        return -ENOMEM;
    }
    input_set_drvdata(chip->input_dev, chip);

    chip->input_dev->name = pdev->name;
    __set_bit(EV_REP, chip->input_dev->evbit);
    __set_bit(EV_KEY, chip->input_dev->evbit);

    //将EV_KEY定义成按键的动作
    chip->key_event = KEY_0; 
    input_set_capability(chip->input_dev, EV_KEY, KEY_0);
    result = input_register_device(chip->input_dev);
    if (result)
    {
        dev_err(&pdev->dev, "Unable to register input device, error: %d\n", result);
		return result;
    }

    dev_info(&pdev->dev, "input driver create success!");

    return result;
}

static int key_probe(struct platform_device *pdev)
{
    int result;
    struct key_data *chip = NULL;

    chip = devm_kzalloc(&pdev->dev, sizeof(struct key_data), GFP_KERNEL);
    if(!chip){
        dev_err(&pdev->dev, "malloc error\n");
        return -ENOMEM;
    }
    chip->pdev = pdev;
    platform_set_drvdata(pdev, chip);

    result = key_device_create(chip);
    if (result != 0)
    {
        dev_err(&pdev->dev, "device create failed!\n");
        return result;
    }

    result = key_hw_init(chip);
    if (result != 0)
    {
        dev_err(&pdev->dev, "Key gpio init failed!\r\n");
        return result;
    }

    dev_info(&pdev->dev, "key driver init ok!\r\n");
    return 0;
}

static int key_remove(struct platform_device *pdev)
{
    struct key_data *chip = platform_get_drvdata(pdev);

    del_timer_sync(&chip->key_timer);

    device_destroy(chip->class, chip->dev_id);
    class_destroy(chip->class);
    cdev_del(&chip->cdev);
    unregister_chrdev_region(chip->dev_id, DEVICE_KEY_CNT);

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
