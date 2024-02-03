/*
 * File      : kernal_spi_icm.c
 * This file is spi icm20608 driver
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-4-30      zc           the first version
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
#include <linux/spi/spi.h>
#include "kernel_spi_icm.h"

//全局的指令定义
#define SPI_ICM_NAME            "icm20608"
#define SPI_ICM_CNT             1

//icm20608设备信息
struct icm20608_dev 
{
    dev_t devid;                /* 设备号 */
    struct cdev cdev;           /* cdev */
    struct class *class;        /* 类 */
    struct device *device;      /* 设备 */
    struct device_node*nd;    /* 设备节点 */
    int major;                  /* 主设备号 */
    void *private_data;         /* 私有数据 */
    int cs_gpio;                /* 片选所使用的GPIO编号 */
};

//icm20608应用信息
struct icm20608_info
{
    struct icm20608_dev dev;
    signed int gyro_x_adc;      /* 陀螺仪X轴原始值 */
    signed int gyro_y_adc;      /* 陀螺仪Y轴原始值 */
    signed int gyro_z_adc;      /* 陀螺仪Z轴原始值 */
    signed int accel_x_adc;     /* 加速度计X轴原始值 */
    signed int accel_y_adc;     /* 加速度计Y轴原始值 */
    signed int accel_z_adc;     /* 加速度计Z轴原始值 */
    signed int temp_adc;        /* 温度原始值 */
};

static struct icm20608_info icm_info;

static int icm20608_read_regs(struct icm20608_dev *dev, u8 reg, void *buf, int len)
{
	int ret = -1;
	unsigned char txdata[2];
	unsigned char *rxdata;
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;
    
	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/* 申请内存 */
	if(!t) {
		return -ENOMEM;
	}

	rxdata = kzalloc(sizeof(char) * len, GFP_KERNEL);	/* 申请内存 */
	if(!rxdata) {
		goto out1;
	}

	txdata[0] = reg | 0x80;				
	t->tx_buf = txdata;			
    t->rx_buf = rxdata;			
	t->len = len+1;				
	spi_message_init(&m);		
	spi_message_add_tail(t, &m);
	ret = spi_sync(spi, &m);	
	if(ret) {
		goto out2;
	}
	
    memcpy(buf , rxdata+1, len);  /* 只需要读取的数据 */

out2:
	kfree(rxdata);					/* 释放内存 */
out1:	
	kfree(t);						/* 释放内存 */
	
	return ret;
}

static s32 icm20608_write_regs(struct icm20608_dev *dev, u8 reg, u8 *buf, u8 len)
{
	int ret = -1;
	unsigned char *txdata;
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;
	
	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/* 申请内存 */
	if(!t) {
		return -ENOMEM;
	}
	
	txdata = kzalloc(sizeof(char)+len, GFP_KERNEL);
	if(!txdata) {
		goto out1;
	}
	
	*txdata = reg & ~0x80;
    memcpy(txdata+1, buf, len);
	t->tx_buf = txdata;		
	t->len = len+1;			
	spi_message_init(&m);		
	spi_message_add_tail(t, &m);
	ret = spi_sync(spi, &m);	
    if(ret) {
        goto out2;
    }
	
out2:
	kfree(txdata);		
out1:
	kfree(t);
	return ret;
}

static unsigned char icm20608_read_onereg(struct icm20608_dev *dev, u8 reg)
{
    u8 data = 0;
    icm20608_read_regs(dev, reg, &data, 1);
    return data;
}

static void icm20608_write_onereg(struct icm20608_dev *dev, u8 reg, u8 value)
{
    u8 buf = value;
    icm20608_write_regs(dev, reg, &buf, 1);
}

void icm20608_readdata(struct icm20608_info *p_info)
{
    unsigned char data[14];
    icm20608_read_regs(&p_info->dev, ICM20_ACCEL_XOUT_H, data, 14);

    p_info->accel_x_adc = (signed short)((data[0] << 8) | data[1]);
    p_info->accel_y_adc = (signed short)((data[2] << 8) | data[3]);
    p_info->accel_z_adc = (signed short)((data[4] << 8) | data[5]);
    p_info->temp_adc    = (signed short)((data[6] << 8) | data[7]);
    p_info->gyro_x_adc  = (signed short)((data[8] << 8) | data[9]);
    p_info->gyro_y_adc  = (signed short)((data[10] << 8) | data[11]);
    p_info->gyro_z_adc  = (signed short)((data[12] << 8) | data[13]);
}

static int icm20608_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &icm_info; /* 设置私有数据 */
    return 0;
}

static ssize_t icm20608_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    signed int data[7];
    int err;
    struct icm20608_info *p_info = (struct icm20608_info *)filp->private_data;

    icm20608_readdata(p_info);
    data[0] = p_info->gyro_x_adc;
    data[1] = p_info->gyro_y_adc;
    data[2] = p_info->gyro_z_adc;
    data[3] = p_info->accel_x_adc;
    data[4] = p_info->accel_y_adc;
    data[5] = p_info->accel_z_adc;
    data[6] = p_info->temp_adc;
    err = copy_to_user(buf, data, sizeof(data));

    if (err != 0)
    {
        return err;
    }
    return cnt;
}

static int icm20608_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static const struct file_operations icm20608_ops = {
    .owner = THIS_MODULE,
    .open = icm20608_open,
    .read = icm20608_read,
    .release = icm20608_release,
};

static int icm_hardware_init(struct spi_device *spi)
{
    unsigned char value = 0;

    spi->mode = SPI_MODE_0;
    spi_setup(spi);
    icm_info.dev.private_data = spi;

    icm20608_write_onereg(&icm_info.dev, ICM20_PWR_MGMT_1, 0x80);
    mdelay(50);
    icm20608_write_onereg(&icm_info.dev, ICM20_PWR_MGMT_1, 0x01);
    mdelay(50);

    value = icm20608_read_onereg(&icm_info.dev, ICM20_WHO_AM_I);
    printk(KERN_INFO"ICM20608 ID = %#X\r\n", value);

    icm20608_write_onereg(&icm_info.dev, ICM20_SMPLRT_DIV, 0x00);   /* 输出速率是内部采样率 */
    icm20608_write_onereg(&icm_info.dev, ICM20_GYRO_CONFIG, 0x18);  /* 陀螺仪±2000dps量程 */
    icm20608_write_onereg(&icm_info.dev, ICM20_ACCEL_CONFIG, 0x18); /* 加速度计±16G量程 */
    icm20608_write_onereg(&icm_info.dev, ICM20_CONFIG, 0x04);       /* 陀螺仪低通滤波BW=20Hz */
    icm20608_write_onereg(&icm_info.dev, ICM20_ACCEL_CONFIG2, 0x04);/* 加速度计低通滤波BW=21.2Hz */
    icm20608_write_onereg(&icm_info.dev, ICM20_PWR_MGMT_2, 0x00);   /* 打开加速度计和陀螺仪所有轴 */
    icm20608_write_onereg(&icm_info.dev, ICM20_LP_MODE_CFG, 0x00);  /* 关闭低功耗 */
    icm20608_write_onereg(&icm_info.dev, ICM20_FIFO_EN, 0x00);      /* 关闭FIFO*/

    return 0;
}

static int icm20608_probe(struct spi_device *spi)
{
    int ret = 0;

    /* 1、构建设备号 */
    if (icm_info.dev.major)
    {
        icm_info.dev.devid = MKDEV(icm_info.dev.major, 0);
        register_chrdev_region(icm_info.dev.devid, SPI_ICM_CNT, SPI_ICM_NAME);
    } 
    else
    {
        alloc_chrdev_region(&icm_info.dev.devid, 0, SPI_ICM_CNT, SPI_ICM_NAME);
        icm_info.dev.major = MAJOR(icm_info.dev.devid);
    }

    /* 2、注册设备 */
    cdev_init(&icm_info.dev.cdev, &icm20608_ops);
    cdev_add(&icm_info.dev.cdev, icm_info.dev.devid, SPI_ICM_CNT);

    /* 3、创建类 */
    icm_info.dev.class = class_create(THIS_MODULE, SPI_ICM_NAME);
    if (IS_ERR(icm_info.dev.class))
    {
        return PTR_ERR(icm_info.dev.class);
    }

    /* 4、创建设备 */
    icm_info.dev.device = device_create(icm_info.dev.class, NULL, icm_info.dev.devid, NULL, SPI_ICM_NAME);
    if (IS_ERR(icm_info.dev.device))
    {
        return PTR_ERR(icm_info.dev.device);
    }

    ret = icm_hardware_init(spi);
    if (ret != 0)
    {
        printk("icm hardware init failed!\r\n");
        return -EINVAL;
    }
    
    printk(KERN_INFO"SPI Driver Init Ok!\r\n");
    return 0;
}

static void icm20608_remove(struct spi_device *spi)
{
    /* 删除设备 */
    cdev_del(&icm_info.dev.cdev);
    unregister_chrdev_region(icm_info.dev.devid, SPI_ICM_CNT);

    /* 注销掉类和设备 */
    device_destroy(icm_info.dev.class, icm_info.dev.devid);
    class_destroy(icm_info.dev.class);
}

static const struct spi_device_id icm20608_id[] = {
    {"icm20608", 0},
    {}
};

static const struct of_device_id icm20608_of_match[] = {
    { .compatible = "rmk,icm20608" },
    { /* Sentinel */ }
};

static struct spi_driver icm20608_driver = {
    .probe = icm20608_probe,
    .remove = icm20608_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "icm20608",
        .of_match_table = icm20608_of_match, 
    },
    .id_table = icm20608_id,
};

static int __init icm20608_module_init(void)
{
    return spi_register_driver(&icm20608_driver);
}

static void __exit icm20608_module_exit(void)
{
    return spi_unregister_driver(&icm20608_driver);
}

module_init(icm20608_module_init);
module_exit(icm20608_module_exit);
MODULE_AUTHOR("zc");                    //模块作者
MODULE_LICENSE("GPL v2");               //模块许可协议
MODULE_DESCRIPTION("icm20608 driver");  //模块许描述
MODULE_ALIAS("spi_icm20608_driver");    //模块别名
