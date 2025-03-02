////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      kernel_dev_hx711.c
//
//  Purpose:
//      双线称重传感器驱动。
//          SCL: GPIO4_22
//          SDA: GPIO4_24   
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
usr_hx711 {
    compatible = "rmk,usr-hx711";
    pinctrl-0 = <&pinctrl_hx711>;
    hx711-gpios = <&gpio4 22 GPIO_ACTIVE_LOW>,
                <&gpio4 24 GPIO_ACTIVE_LOW>;
    status = "okay";
};

pinctrl_hx711: gpio-hx711 {
    fsl,pins = <
        MX6UL_PAD_CSI_DATA01__GPIO4_IO22        0x17059
        MX6UL_PAD_CSI_DATA03__GPIO4_IO24        0x17059
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
#include <linux/semaphore.h>
#include <linux/platform_device.h>

struct hx711_data
{
    /*device info*/
    dev_t dev_id;     
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct platform_device *pdev;

    /* hardware info */
    int sck_gpio;
    int sda_gpio;
    uint32_t adc_value;

    uint32_t wait_delay;
};

//自定义设备号
#define DEFAULT_MAJOR                       0       /*默认主设备号*/
#define DEFAULT_MINOR                       0       /*默认从设备号*/
#define DEVICE_NAME                         "hx711"   /* 设备名, 应用将以/dev/hx711访问 */

#define SCK_HIGH(gpio)                      {gpio_set_value(gpio, 1);}
#define SCK_LOWER(gpio)                     {gpio_set_value(gpio, 0);}
#define SDA_VALUE(gpio)                     (gpio_get_value(gpio))

static void delay_us(int us)
{
    ktime_t kt;
    u64 pre,last;
    kt = ktime_get();
    pre = ktime_to_ns(kt);
    while (1) {
        kt = ktime_get();
        last = ktime_to_ns(kt);
        if (last-pre >= us*1000) {
            break;
        }
    }
}

int hx711_open(struct inode *inode, struct file *filp)
{
    static struct hx711_data *chip;
    
    chip = container_of(inode->i_cdev, struct hx711_data, cdev);
    filp->private_data = chip;
    return 0;
}

int hx711_release(struct inode *inode, struct file *filp)
{
    return 0;
}

uint32_t read_hx711_adc(struct hx711_data *chip)
{
    uint32_t Count;
    unsigned char i;
    
    SCK_LOWER(chip->sck_gpio);
    Count=0;
    while (SDA_VALUE(chip->sda_gpio)) {
        usleep_range(100, 200);             //200us
        chip->wait_delay++;
        if (chip->wait_delay > 1000) {      //more than 200ms, read failed.
            chip->wait_delay = 0;
            dev_err(&chip->pdev->dev, "%s read timeout!\n", __func__);
            return 0;                   
        }
    }
    chip->wait_delay = 0;

    for (i=0; i<24; i++) {
        SCK_HIGH(chip->sck_gpio);
        Count = Count<<1;
        SCK_LOWER(chip->sck_gpio)
        delay_us(1);

        if (SDA_VALUE(chip->sda_gpio))
            Count += 1;
        delay_us(1);
    }
    SCK_HIGH(chip->sck_gpio);
    Count=Count^0x800000;      
    SCK_LOWER(chip->sck_gpio)
    return(Count);
}

ssize_t hx711_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int ret;
    struct hx711_data *chip;
    struct platform_device *pdev;

    chip = (struct hx711_data *)filp->private_data;
    pdev = chip->pdev;

    chip->adc_value = read_hx711_adc(chip);

    ret = copy_to_user(buf, (char *)&chip->adc_value, 4);
    if (ret) {
        dev_err(&pdev->dev, "copy to user failed!\n");
        return -EFAULT;
    }

    dev_info(&pdev->dev, "read hx711 success, data:%d\r\n", chip->adc_value);
    return 1;
}

static struct file_operations hx711_fops = {
    .owner = THIS_MODULE,
    .open = hx711_open,
    .read = hx711_read,
    .release = hx711_release,
};

static int hx711_device_create(struct hx711_data *chip)
{
    int ret;
    int major, minor;
    struct platform_device *pdev;

    major = DEFAULT_MAJOR;
    minor = DEFAULT_MINOR;
    pdev = chip->pdev;

    //1.申请设备号
    if (major) {
        chip->dev_id = MKDEV(major, minor);
        ret = register_chrdev_region(chip->dev_id, 1, DEVICE_NAME);
    } else {
        ret = alloc_chrdev_region(&chip->dev_id, 0, 1, DEVICE_NAME);
    }
    if (ret < 0) {
        dev_err(&pdev->dev, "id alloc faihx711!\n");
        goto exit;
    }

    //2.创建字符设备，关联设备号，并添加到内核
    cdev_init(&chip->cdev, &hx711_fops);
    chip->cdev.owner = THIS_MODULE;
    ret = cdev_add(&chip->cdev, chip->dev_id, 1);
    if (ret) {
        dev_err(&pdev->dev, "cdev add faihx711:%d!\n", ret);
        goto exit_cdev_add;
    }

    //3.创建设备类和设备文件，关联设备号，用于应用层访问
    chip->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(chip->class)) {
        dev_err(&pdev->dev, "class create faihx711!\n");
        ret = PTR_ERR(chip->class);
        goto exit_class_create;
    }
    chip->device = device_create(chip->class, NULL, chip->dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(chip->device)) {
        dev_err(&pdev->dev, "device create faihx711!\r\n");
        ret = PTR_ERR(chip->device);
        goto exit_device_create;
    }

    dev_info(&pdev->dev, "device create success!\r\n");
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

static int hx711_hardware_init(struct hx711_data *chip)
{
    int ret;
    struct platform_device *pdev = chip->pdev;
    struct device_node *hx711_nd = pdev->dev.of_node;

    //获取sck指定gpio
    chip->sck_gpio = of_get_named_gpio(hx711_nd, "hx711-gpios", 0);
    if (chip->sck_gpio < 0) {
        dev_err(&pdev->dev, "find sck_gpio in dts failed!\n");
        return -EINVAL;
    }
    ret = devm_gpio_request(&pdev->dev, chip->sck_gpio, "hx711-sck");
    if (ret < 0) {
        dev_err(&pdev->dev, "request sck_gpio failed!\n");
        return -EINVAL;   
    }
    gpio_direction_output(chip->sck_gpio, 0);

    //获取sda指定gpio
    chip->sda_gpio = of_get_named_gpio(hx711_nd, "hx711-gpios", 1);
    if (chip->sda_gpio < 0) {
        dev_err(&pdev->dev, "find sda_gpio in dts failed!\n");
        return -EINVAL;
    }
    ret = devm_gpio_request(&pdev->dev, chip->sda_gpio, "hx711-sda");
    if (ret < 0) {
        dev_err(&pdev->dev, "request sda_gpio failed!\n");
        return -EINVAL;   
    }
    gpio_direction_input(chip->sda_gpio);

    dev_info(&pdev->dev, "%s init success, sck_pin:%d, sda_pin:%d ", __func__, chip->sck_gpio, chip->sda_gpio);
    return 0;
}

static int hx711_probe(struct platform_device *pdev)
{
    int ret;
    static struct hx711_data *chip;

    //1.申请hx711控制块
    chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
    if (!chip) {
        dev_err(&pdev->dev, "memory alloc failedd hx711!\n");
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, chip);
    chip->pdev = pdev;
    chip->wait_delay = 0;
    
    //2.初始化hx711硬件设备
    ret = hx711_hardware_init(chip);
    if (ret) {
        dev_err(&pdev->dev, "hardware init failed, error:%d!\n", ret);
        return ret;
    }

    //3.创建内核访问接口
    ret = hx711_device_create(chip);
    if (ret) {
        dev_err(&pdev->dev, "device create failed, error:%d!\n", ret);
        return ret;
    }

    dev_info(&pdev->dev, "driver init success!\n");
    return 0;
}

static int hx711_remove(struct platform_device *pdev)
{
    struct hx711_data *chip = platform_get_drvdata(pdev);

    device_destroy(chip->class, chip->dev_id);
    class_destroy(chip->class);

    cdev_del(&chip->cdev);
    unregister_chrdev_region(chip->dev_id, 1);
    
    dev_info(&pdev->dev, "driver release!\n");
    return 0;
}

//匹配的是根节点的compatible属性
static const struct of_device_id hx711_of_match[] = {
    { .compatible = "rmk,usr-hx711"},
    { /* Sentinel */ }
};

static struct platform_driver platform_driver = {
    .driver = {
        .name = "hx711",
        .of_match_table = hx711_of_match,
    },
    .probe = hx711_probe,
    .remove = hx711_remove,
};

static int __init hx711_module_init(void)
{
    platform_driver_register(&platform_driver);
    return 0;
}

static void __exit hx711_module_exit(void)
{
    platform_driver_unregister(&platform_driver);
}

module_init(hx711_module_init);
module_exit(hx711_module_exit);
MODULE_AUTHOR("wzdxf");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("platform driver for hx711");
MODULE_ALIAS("hx711_driver");
