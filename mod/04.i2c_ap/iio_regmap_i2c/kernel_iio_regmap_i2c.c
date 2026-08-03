////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      kernel_iio_regmap_i2c.c
//
//  Purpose:
//      i2c ap3216 iio regmap driver
//
// Author:
//     @听心跳的声音
//
// Revision History:
//      12/19/2022   Create New Version
////////////////////////////////////////////////////////////////////////////

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/of_gpio.h>
#include <linux/mutex.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

#define AP3216C_SYSTEMCONG          0x00
#define AP3216C_INTSTATUS           0X01
#define AP3216C_INTCLEAR            0X02
#define AP3216C_IRDATALOW           0x0A
#define AP3216C_IRDATAHIGH          0x0B
#define AP3216C_ALSDATALOW          0x0C
#define AP3216C_ALSDATAHIGH         0X0D
#define AP3216C_PSDATALOW           0X0E
#define AP3216C_PSDATAHIGH          0X0F

#define DEVICE_NAME                 "ap3216"

struct ap3216_data
{
    struct i2c_client *client;
    struct regmap *map;
    struct iio_dev *indio_dev;
    struct mutex lock;
    u8 sysconf;
};

enum ap3216_channel_index
{
    AP3216_IR,
    AP3216_ALS,
    AP3216_PS,
};

static const struct iio_chan_spec ap3216_channels[] =
{
    {
        .type = IIO_INTENSITY,
        .channel = AP3216_IR,
        .extend_name = "ir",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_LIGHT,
        .channel = AP3216_ALS,
        .extend_name = "als",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_PROXIMITY,
        .channel = AP3216_PS,
        .extend_name = "ps",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
};

static const struct regmap_config ap3216_regmap_config =
{
    .reg_bits = 8,
    .val_bits = 8,
    .reg_stride = 1,
    .max_register = 255,
};

static int ap3216_read_raw(struct iio_dev *indio_dev,
                           struct iio_chan_spec const *chan,
                           int *val,
                           int *val2,
                           long mask)
{
    struct ap3216_data *chip;
    int readbuf[6];
    int ret;
    unsigned short ir;
    unsigned short als;
    unsigned short ps;

    if(mask != IIO_CHAN_INFO_RAW)
        return -EINVAL;

    chip = iio_priv(indio_dev);

    mutex_lock(&chip->lock);
    ret = regmap_bulk_read(chip->map, AP3216C_IRDATALOW, readbuf, 6);
    mutex_unlock(&chip->lock);

    if(ret) {
        dev_err(&chip->client->dev, "ap3216 read failed:%d\n", ret);
        return ret;
    }

    if((readbuf[0]&(1<<7)) || (readbuf[4]&(1<<6))) {
        ir = 0;
        ps = 0;
    } else {
        ir = ((unsigned short)readbuf[1]<<2) | (readbuf[0]&0X03);
        ps = ((unsigned short)(readbuf[5]&0X3F)<<4) | (readbuf[4]&0X0F);
    }

    als = ((unsigned short)readbuf[3]<<8) | readbuf[2];

    switch(chan->channel)
    {
        case AP3216_IR:
            *val = ir;
            break;
        case AP3216_ALS:
            *val = als;
            break;
        case AP3216_PS:
            *val = ps;
            break;
        default:
            return -EINVAL;
    }

    return IIO_VAL_INT;
}

static const struct iio_info ap3216_info =
{
    .read_raw = ap3216_read_raw,
};

static int ap3216_parse_dt(struct ap3216_data *chip)
{
    int ret;
    struct i2c_client *client;
    struct device_node *np;

    client = chip->client;
    np = client->dev.of_node;

    ret = of_property_read_u8(np, "rmk,sysconf", &chip->sysconf);
    if (ret) {
        dev_warn(&client->dev, "read sysconf failed,error:%d\n", ret);
        chip->sysconf = 0x03;
    }

    return 0;
}

static int ap3216_hardware_init(struct ap3216_data *chip)
{
    int ret;
    struct i2c_client *client;

    client = chip->client;

    ret = regmap_write(chip->map, AP3216C_SYSTEMCONG, 0x04);
    if (ret) {
        dev_err(&client->dev, "ap3216 reset failed:%d\n", ret);
        return ret;
    }
    msleep(50);

    ret = regmap_write(chip->map, AP3216C_SYSTEMCONG, chip->sysconf);
    if (ret) {
        dev_err(&client->dev, "ap3216 config failed:%d\n", ret);
        return ret;
    }
    msleep(50);

    return 0;
}

static int ap3216_probe(struct i2c_client *client,
                        const struct i2c_device_id *id)
{
    int ret;
    struct iio_dev *indio_dev;
    struct ap3216_data *chip;

    indio_dev = devm_iio_device_alloc(&client->dev,
                                      sizeof(struct ap3216_data));

    if(!indio_dev) {
        dev_err(&client->dev, "iio device alloc failed\n");
        return -ENOMEM;
    }

    chip = iio_priv(indio_dev);

    chip->client = client;
    chip->indio_dev = indio_dev;

    mutex_init(&chip->lock);

    i2c_set_clientdata(client, indio_dev);

    chip->map = devm_regmap_init_i2c(client, &ap3216_regmap_config);
    if(IS_ERR(chip->map)) {
        dev_err(&client->dev, "chip map init failed\n");
        return -ENXIO;
    }

    ap3216_parse_dt(chip);

    ret = ap3216_hardware_init(chip);

    if(ret) {
        dev_err(&client->dev, "ap3216 hardware init failed\n");
        return ret;
    }

    indio_dev->name = DEVICE_NAME;

    indio_dev->info = &ap3216_info;
    indio_dev->modes = INDIO_DIRECT_MODE;
    indio_dev->channels = ap3216_channels;
    indio_dev->num_channels = ARRAY_SIZE(ap3216_channels);

    ret = devm_iio_device_register(&client->dev, indio_dev);

    if(ret)
    {
        dev_err(&client->dev, "iio device register failed\n");
        return ret;
    }

    dev_info(&client->dev, "ap3216 iio driver init ok,sysconf:%d\n", chip->sysconf);

    return 0;
}

static void ap3216_remove(struct i2c_client *client)
{
    struct iio_dev *indio_dev;

    indio_dev = i2c_get_clientdata(client);

    dev_info(&client->dev, "ap3216 iio driver release ok!\n");
}

static const struct of_device_id ap3216_of_match[] =
{
    {
        .compatible = "rmk,ap3216",
    },
    {
    }
};

static const struct i2c_device_id ap3216_id[] =
{
    {
        "ap3216",
        0
    },
    {
    }
};

static struct i2c_driver ap3216_driver =
{
    .probe = ap3216_probe,
    .remove = ap3216_remove,
    .id_table = ap3216_id,
    .driver =
    {
        .name = "ap3216",
        .of_match_table = ap3216_of_match,
    },
};

static int __init ap3216_module_init(void)
{
    return i2c_add_driver(&ap3216_driver);
}

static void __exit ap3216_module_exit(void)
{
    i2c_del_driver(&ap3216_driver);
}

module_init(ap3216_module_init);
module_exit(ap3216_module_exit);

MODULE_AUTHOR("zc");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("ap3216 iio driver");
MODULE_ALIAS("i2c_ap3216_iio_driver");
