/*
 * File      : kernel_hx711.c
 * This file is the driver for hx711 i/o.
 * COPYRIGHT (C) 2023, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-22     zc           the first version
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
#include <linux/platform_device.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

struct hx711_data
{
    /*device info*/
    struct platform_device *pdev;

    /* hardware info */
    int sck_gpio;
    int sda_gpio;
    uint32_t adc_value;

    uint32_t wait_delay;
};

//自定义设备号
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
	while(1)
	{
		kt = ktime_get();
		last = ktime_to_ns(kt);
		if(last-pre >= us*1000)
		{
			break;
		}
	}
}

uint32_t read_hx711_adc(struct hx711_data *chip)
{
    uint32_t Count;
    unsigned char i;
    
    SCK_LOWER(chip->sck_gpio);
    Count=0;
    while(SDA_VALUE(chip->sda_gpio))
    {
        usleep_range(100, 200);         //200us
        chip->wait_delay++;
        if(chip->wait_delay > 1000)     //more than 200ms, read failed.
        {
            chip->wait_delay = 0;
            dev_err(&chip->pdev->dev, "%s read timeout!\n", __func__);
            return 0;                   
        }
    }
    chip->wait_delay = 0;

    for(i=0; i<24; i++)
    {
        SCK_HIGH(chip->sck_gpio);
        Count = Count<<1;
        SCK_LOWER(chip->sck_gpio)
        delay_us(1);

        if(SDA_VALUE(chip->sda_gpio))
            Count += 1;
        delay_us(1);
    } 
    SCK_HIGH(chip->sck_gpio);
    Count=Count^0x800000;      
    SCK_LOWER(chip->sck_gpio)
    return(Count);
}

static int hx711_read_raw(struct iio_dev *indio_dev,
			   struct iio_chan_spec const *chan,
			   int *val, int *val2, long mask)
{
    struct hx711_data *chip = iio_priv(indio_dev);
    struct platform_device *pdev;

    chip->adc_value = read_hx711_adc(chip);
    pdev = chip->pdev;

    if(chip->adc_value == 0) {
        dev_err(&pdev->dev, "hx711 read issue!");
        return -EIO;
    }

    switch (mask) 
    {
	case IIO_CHAN_INFO_RAW:							
		*val = chip->adc_value;
		*val2 = 0;
        break;
    case IIO_CHAN_INFO_SCALE:
        break;
    }

    dev_info(&pdev->dev, "hx711 read raw:0x%x", chip->adc_value);
    return IIO_VAL_INT_PLUS_MICRO;
}

static const struct iio_info hx711_info = {
	.read_raw		= hx711_read_raw
};

#define HX711_VOLTAGE_CHAN(addr, si, chan, name, info_all, relbits) { \
	.type = IIO_VOLTAGE, \
	.indexed = 1, \
	.channel = (chan), \
	.extend_name = name, \
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | \
		BIT(IIO_CHAN_INFO_SCALE), \
	.info_mask_shared_by_all = info_all, \
	.address = (addr), \
	.scan_index = (si), \
	.scan_type = { \
		.sign = 'u', \
		.realbits = (relbits), \
		.storagebits = 32, \
		.endianness = IIO_BE, \
	}, \
}

#define HX711_AUX_ADC_CHAN(addr, si, info_all, relbits) \
	HX711_VOLTAGE_CHAN(addr, si, 1, NULL, info_all, relbits)

static const struct iio_chan_spec hx711_channels[] = {
	HX711_AUX_ADC_CHAN(0, 0, 0, 24)
};    
static int hx711_hardware_init(struct hx711_data *chip)
{
    int ret;
    struct platform_device *pdev = chip->pdev;
    struct device_node *hx711_nd = pdev->dev.of_node;

    chip->sck_gpio = of_get_named_gpio(hx711_nd, "hx711-gpios", 0);
    if (chip->sck_gpio < 0){
        dev_err(&pdev->dev, "find sck_gpio in dts failed!\n");
        return -EINVAL;
    }
    ret = devm_gpio_request(&pdev->dev, chip->sck_gpio, "hx711-sck");
    if (ret < 0){
        dev_err(&pdev->dev, "request sck_gpio failed!\n");
        return -EINVAL;   
    }
    gpio_direction_output(chip->sck_gpio, 0);


    chip->sda_gpio = of_get_named_gpio(hx711_nd, "hx711-gpios", 1);
    if (chip->sda_gpio < 0){
        dev_err(&pdev->dev, "find sda_gpio in dts failed!\n");
        return -EINVAL;
    }
    ret = devm_gpio_request(&pdev->dev, chip->sda_gpio, "hx711-sda");
    if (ret < 0){
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
    struct hx711_data *chip;
	struct iio_dev *indio_dev;

    indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(struct hx711_data));
	if (!indio_dev)
		return -ENOMEM;

    chip = iio_priv(indio_dev);
    chip->pdev = pdev;
    chip->wait_delay = 0;
    platform_set_drvdata(pdev, indio_dev);

	indio_dev->dev.parent = &pdev->dev;
	indio_dev->info = &hx711_info;
	indio_dev->name = DEVICE_NAME;	
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = hx711_channels;
	indio_dev->num_channels = ARRAY_SIZE(hx711_channels);

	ret = devm_iio_device_register(&pdev->dev, indio_dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "iio_device_register failed\n");
		return -EIO;
	}

    ret = hx711_hardware_init(chip);
    if (ret){
        dev_err(&pdev->dev, "hardware init failed, error:%d!\n", ret);
        return ret;
    }

    dev_info(&pdev->dev, "driver init success!\n");
    return 0;
}

static int hx711_remove(struct platform_device *pdev)
{
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
