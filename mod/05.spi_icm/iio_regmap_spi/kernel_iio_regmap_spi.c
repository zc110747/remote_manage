////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      kernel_iio_regmap_spi.c
//
//  Purpose:
//      spi regmap iio驱动。
//
// Author:
//     @听心跳的声音
//
// Revision History:
//      4/3/2022   Create New Version
////////////////////////////////////////////////////////////////////////////

/*
设备树说明

&ecspi3 {
	fsl,spi-num-chipselects = <1>;
	pinctrl-names = "default";
	cs-gpios = <&gpio1 20 GPIO_ACTIVE_LOW>;
	pinctrl-0 = <&pinctrl_ecspi3>;
	status = "okay";

	icm20608@0 {
		compatible = "rmk,icm20608";
		spi-max-frequency = <8000000>;
		reg = <0>;

		smplrt_div 		= /bits/ 8 <0x00>;
		gyro_config 	= /bits/ 8 <0x18>;
		accel_config 	= /bits/ 8 <0x18>;
		config			= /bits/ 8 <0x04>;
		accel_config2 	= /bits/ 8 <0x04>;
		pwr_mgmt_2 		= /bits/ 8 <0x00>;
		lp_mode_cfg 	= /bits/ 8 <0x00>;
		fifo_en			= /bits/ 8 <0x00>;
	};
};

*/

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

#include "kernel_regmap_spi.h"

#define DEVICE_NAME                 "icm20608"

struct spi_reg_config
{
    u8 smplrt_div;
    u8 gyro_config;
    u8 accel_config;
    u8 config;
    u8 accel_config2;
    u8 pwr_mgmt_2;
    u8 lp_mode_cfg;
    u8 fifo_en;
};

struct spi_icm_data
{
    struct spi_device *private_data;
    struct regmap *map;
    struct spi_reg_config reg_config;
    struct iio_dev *indio_dev;
};

enum icm20608_channel_index
{
    ICM20608_ACCEL_X,
    ICM20608_ACCEL_Y,
    ICM20608_ACCEL_Z,
    ICM20608_TEMP,
    ICM20608_GYRO_X,
    ICM20608_GYRO_Y,
    ICM20608_GYRO_Z,
};

static const struct iio_chan_spec icm20608_channels[] =
{
    {
        .type = IIO_ACCEL,
        .channel = ICM20608_ACCEL_X,
        .extend_name = "accel_x_adc",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_ACCEL,
        .channel = ICM20608_ACCEL_Y,
        .extend_name = "accel_y_adc",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_ACCEL,
        .channel = ICM20608_ACCEL_Z,
        .extend_name = "accel_z_adc",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_TEMP,
        .channel = ICM20608_TEMP,
        .extend_name = "temp_adc",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_ANGL_VEL,
        .channel = ICM20608_GYRO_X,
        .extend_name = "gyro_x_adc",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_ANGL_VEL,
        .channel = ICM20608_GYRO_Y,
        .extend_name = "gyro_y_adc",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    {
        .type = IIO_ANGL_VEL,
        .channel = ICM20608_GYRO_Z,
        .extend_name = "gyro_z_adc",
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
};

static const struct regmap_config icm20608_regmap_config =
{
    .reg_bits = 8,
    .val_bits = 8,
    .reg_stride = 1,
    .read_flag_mask = BIT(7),
    .max_register = 255,
};

static int icm20608_read_raw(struct iio_dev *indio_dev, struct iio_chan_spec const *chan,
                            int *val, int *val2, long mask)
{
    struct spi_icm_data *chip;
    unsigned char readbuf[14];
    int ret;

    chip = iio_priv(indio_dev);

    if(mask != IIO_CHAN_INFO_RAW)
        return -EINVAL;

    ret = regmap_bulk_read(chip->map,
                           ICM20_ACCEL_XOUT_H,
                           readbuf,
                           14);

    if(ret < 0)
    {
        dev_err(&chip->private_data->dev,
                "icm20608 read failed:%d\n",
                ret);
        return ret;
    }

    switch(chan->channel)
    {
        case ICM20608_ACCEL_X:
            *val = (signed short)((readbuf[0]<<8)|readbuf[1]);
            break;

        case ICM20608_ACCEL_Y:
            *val = (signed short)((readbuf[2]<<8)|readbuf[3]);
            break;

        case ICM20608_ACCEL_Z:
            *val = (signed short)((readbuf[4]<<8)|readbuf[5]);
            break;

        case ICM20608_TEMP:
            *val = (signed short)((readbuf[6]<<8)|readbuf[7]);
            break;

        case ICM20608_GYRO_X:
            *val = (signed short)((readbuf[8]<<8)|readbuf[9]);
            break;

        case ICM20608_GYRO_Y:
            *val = (signed short)((readbuf[10]<<8)|readbuf[11]);
            break;

        case ICM20608_GYRO_Z:
            *val = (signed short)((readbuf[12]<<8)|readbuf[13]);
            break;

        default:
            return -EINVAL;
    }

    return IIO_VAL_INT;
}

static const struct iio_info icm20608_info =
{
    .read_raw = icm20608_read_raw,
};

static int icm20608_parse_dt(struct spi_icm_data *chip)
{
    struct spi_device *spi;
    struct device_node *np;

    spi = chip->private_data;
    np = spi->dev.of_node;

    if(of_property_read_u8(np,"smplrt_div",&chip->reg_config.smplrt_div))
    {
        dev_warn(&spi->dev,"invalid smplrt_div attribute");
        chip->reg_config.smplrt_div = 0x00;
    }

    if(of_property_read_u8(np,"gyro_config",&chip->reg_config.gyro_config))
    {
        dev_warn(&spi->dev,"invalid gyro_config attribute");
        chip->reg_config.gyro_config = 0x18;
    }

    if(of_property_read_u8(np,"accel_config",&chip->reg_config.accel_config))
    {
        dev_warn(&spi->dev,"invalid accel_config attribute");
        chip->reg_config.accel_config = 0x18;
    }

    if(of_property_read_u8(np,"config",&chip->reg_config.config))
    {
        dev_warn(&spi->dev,"invalid config attribute");
        chip->reg_config.config = 0x04;
    }

    if(of_property_read_u8(np,"accel_config2",&chip->reg_config.accel_config2))
    {
        dev_warn(&spi->dev,"invalid accel_config2 attribute");
        chip->reg_config.accel_config2 = 0x04;
    }

    if(of_property_read_u8(np,"pwr_mgmt_2",&chip->reg_config.pwr_mgmt_2))
    {
        dev_warn(&spi->dev,"invalid pwr_mgmt_2 attribute");
        chip->reg_config.pwr_mgmt_2 = 0x00;
    }

    if(of_property_read_u8(np,"lp_mode_cfg",&chip->reg_config.lp_mode_cfg))
    {
        dev_warn(&spi->dev,"invalid lp_mode_cfg attribute");
        chip->reg_config.lp_mode_cfg = 0x00;
    }

    if(of_property_read_u8(np,"fifo_en",&chip->reg_config.fifo_en))
    {
        dev_warn(&spi->dev,"invalid fifo_en attribute");
        chip->reg_config.fifo_en = 0x00;
    }

    return 0;
}

static int spi_hardware_init(struct spi_icm_data *chip)
{
    int data = 0;
    struct spi_device *spi;

    spi = chip->private_data;

    chip->map = devm_regmap_init_spi(spi,&icm20608_regmap_config);

    if(IS_ERR(chip->map))
    {
        dev_err(&spi->dev,"chip map init failed:%ld\n",
                PTR_ERR(chip->map));
        return PTR_ERR(chip->map);
    }

    regmap_write(chip->map, ICM20_PWR_MGMT_1, 0x80);
    mdelay(50);

    regmap_write(chip->map, ICM20_PWR_MGMT_1, 0x01);
    mdelay(50);

    regmap_read(chip->map, ICM20_WHO_AM_I,&data);

    dev_info(&spi->dev, "ICM20608 ID = %#X\n", data);

    icm20608_parse_dt(chip);

    regmap_write(chip->map, ICM20_SMPLRT_DIV, chip->reg_config.smplrt_div);
    regmap_write(chip->map, ICM20_GYRO_CONFIG, chip->reg_config.gyro_config);
    regmap_write(chip->map, ICM20_ACCEL_CONFIG, chip->reg_config.accel_config);
    regmap_write(chip->map, ICM20_CONFIG, chip->reg_config.config);
    regmap_write(chip->map, ICM20_ACCEL_CONFIG2, chip->reg_config.accel_config2);
    regmap_write(chip->map, ICM20_PWR_MGMT_2, chip->reg_config.pwr_mgmt_2);
    regmap_write(chip->map, ICM20_LP_MODE_CFG, chip->reg_config.lp_mode_cfg);
    regmap_write(chip->map, ICM20_FIFO_EN, chip->reg_config.fifo_en);

    dev_info(&spi->dev,
             "icm20608 reg config list:%d,%d,%d,%d,%d,%d,%d,%d",
             chip->reg_config.smplrt_div,
             chip->reg_config.gyro_config,
             chip->reg_config.accel_config,
             chip->reg_config.config,
             chip->reg_config.accel_config2,
             chip->reg_config.pwr_mgmt_2,
             chip->reg_config.lp_mode_cfg,
             chip->reg_config.fifo_en);

    return 0;
}

static int icm20608_probe(struct spi_device *spi)
{
    int ret;
    struct iio_dev *indio_dev;
    struct spi_icm_data *chip;

    indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(struct spi_icm_data));

    if(!indio_dev) {
        dev_err(&spi->dev,"iio device alloc failed\n");
        return -ENOMEM;
    }

    chip = iio_priv(indio_dev);
    chip->private_data = spi;
    chip->indio_dev = indio_dev;

    spi_set_drvdata(spi,indio_dev);

    ret = spi_hardware_init(chip);

    if(ret) {
        dev_err(&spi->dev, "icm hardware init failed!\n");
        return ret;
    }

    indio_dev->name = DEVICE_NAME;
    indio_dev->info = &icm20608_info;
    indio_dev->modes = INDIO_DIRECT_MODE;
    indio_dev->channels = icm20608_channels;
    indio_dev->num_channels = ARRAY_SIZE(icm20608_channels);

    ret = devm_iio_device_register(&spi->dev, indio_dev);

    if(ret) {
        dev_err(&spi->dev, "iio device register failed\n");
        return ret;
    }

    dev_info(&spi->dev, "iio device register success\n");

    return 0;
}

static void icm20608_remove(struct spi_device *spi)
{
    struct iio_dev *indio_dev;
    indio_dev = spi_get_drvdata(spi);

    dev_info(&spi->dev, "spi remove success!\n");
}

static const struct of_device_id icm20608_of_match[] =
{
    {
        .compatible = "rmk,icm20608",
    },
};

static struct spi_driver icm20608_driver =
{
    .probe = icm20608_probe,
    .remove = icm20608_remove,
    .driver =
    {
        .owner = THIS_MODULE,
        .name = "icm20608",
        .of_match_table = icm20608_of_match,
    },
};

static int __init spi_icm_module_init(void)
{
    return spi_register_driver(&icm20608_driver);
}

static void __exit spi_icm_module_exit(void)
{
    spi_unregister_driver(&icm20608_driver);
}

module_init(spi_icm_module_init);
module_exit(spi_icm_module_exit);

MODULE_AUTHOR("zc");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("icm20608 spi iio driver");
MODULE_ALIAS("kernel_iio_regmap_spi");
