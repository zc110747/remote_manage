/*
 * File      : kernal_test.c
 * This file is test for devicetree
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


/**
 * 驱动加载时执行的初始化函数
 * 
 * @param NULL
 *
 * @return 驱动加载处理结果
 */
static int __init test_module_init(void)
{
    int beep_gpio;
    int ret;
    struct device_node *nd;
    struct property *proper;

    /*根据路径获取设备节点, 和节点内部属性信息*/
    nd = of_find_node_by_path("/usr_gpios/beep");
    if(nd == NULL){
        printk(KERN_INFO"node find by path failed!\n");
        return -EINVAL;
    }
    else{
        printk(KERN_INFO"node find by path success!\n");
    }
    proper = of_find_property(nd, "compatible", NULL);
    if(proper != NULL)
        printk(KERN_INFO"%s:%s\n", proper->name, (char *)proper->value);
    proper = of_find_property(nd, "name", NULL);
    if(proper != NULL)    
        printk(KERN_INFO"%s:%s\n", proper->name, (char *)proper->value);
    beep_gpio = of_get_named_gpio(nd, "beep-gpio", 0);
    printk(KERN_INFO"beep_gpio:%d\n", beep_gpio);

    /*根据compatible属性查询节点*/
    nd = of_find_compatible_node(NULL, NULL, "gpio-key");
    if(nd == NULL){
        printk(KERN_INFO"node find by compatible failed!\n");
        return -EINVAL;
    }
    else{
        printk(KERN_INFO"node find by compatible success!\n");
    }
    proper = of_find_property(nd, "compatible", NULL);
    if(proper != NULL)
        printk(KERN_INFO"%s:%s\n", proper->name, (char *)proper->value);
    proper = of_find_property(nd, "name", NULL);
    if(proper != NULL)
        printk(KERN_INFO"%s:%s\n", proper->name, (char *)proper->value);

    {
        char *pStr = NULL;

        /*根据匹配表格获取节点，并获取属性*/
        static const struct of_device_id key_of_match[] = {
            { .compatible = "gpio-key" },
            { /* Sentinel */ }
        };

        nd = of_find_matching_node_and_match(NULL, key_of_match, NULL);
        if(nd == NULL){
            printk(KERN_INFO"node find by of_device_id failed!\n");
            return -EINVAL;
        }
        else{
            printk(KERN_INFO"node find by of_device_id success!\n");
        }
        proper = of_find_property(nd, "name", NULL);
        if(proper != NULL)
            printk(KERN_INFO"%s:%s\n", proper->name, (char *)proper->value);
        ret = of_property_read_string(nd, "status", &pStr);
        if(pStr != NULL)
            printk(KERN_INFO"status:%s\n", pStr);
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
static void __exit test_module_exit(void)
{

}

module_init(test_module_init);
module_exit(test_module_exit);
MODULE_AUTHOR("zc");				    //模块作者
MODULE_LICENSE("GPL v2");               //模块许可协议
MODULE_DESCRIPTION("test driver");      //模块许描述
MODULE_ALIAS("test_driver");            //模块别名
