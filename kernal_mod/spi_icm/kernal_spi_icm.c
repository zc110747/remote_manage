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
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "kernal_spi_icm.h"

//全局的指令定义
#define SPI_ICM_NAME            "icm20608"
#define SPI_ICM_CNT				1

//icm20608设备信息
struct icm20608_dev {
	dev_t devid;				/* 设备号 	 */
	struct cdev cdev;			/* cdev 	*/
	struct class *class;		/* 类 		*/
	struct device *device;		/* 设备 	 */
	struct device_node	*nd; 	/* 设备节点 */
	int major;					/* 主设备号 */
	void *private_data;			/* 私有数据 		*/
	int cs_gpio;				/* 片选所使用的GPIO编号		*/
};

//icm20608应用信息
struct icm20608_info{
	struct icm20608_dev dev;
	signed int gyro_x_adc;		/* 陀螺仪X轴原始值 	 */
	signed int gyro_y_adc;		/* 陀螺仪Y轴原始值		*/
	signed int gyro_z_adc;		/* 陀螺仪Z轴原始值 		*/
	signed int accel_x_adc;		/* 加速度计X轴原始值 	*/
	signed int accel_y_adc;		/* 加速度计Y轴原始值	*/
	signed int accel_z_adc;		/* 加速度计Z轴原始值 	*/
	signed int temp_adc;		/* 温度原始值 			*/
};

static struct icm20608_info icm_info;

/**
 * 连续读取icm20608设备寄存器的值
 * 
 * @param dev ICM20608设备信息
 * @param reg 待读取设备寄存器的首地址
 * @param buf 读取数据缓冲区首地址
 * @param len 待读取数据长度
 *
 * @return 读取寄存的操作结果
 */
static int icm20608_read_regs(struct icm20608_dev *dev, u8 reg, void *buf, int len)
{
	int ret;
	unsigned char txdata[len];
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;

	gpio_set_value(dev->cs_gpio, 0);				/* 片选拉低，选中ICM20608 */
	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/* 申请内存 */

	/* 第1次，发送要读取的寄存地址 */
	txdata[0] = reg | 0x80;		/* 写数据的时候寄存器地址bit8要置1 */
	t->tx_buf = txdata;			/* 要发送的数据 */
	t->len = 1;					/* 1个字节 */
	spi_message_init(&m);		/* 初始化spi_message */
	spi_message_add_tail(t, &m);/* 将spi_transfer添加到spi_message队列 */
	ret = spi_sync(spi, &m);	/* 同步发送 */

	/* 第2次，读取数据 */
	txdata[0] = 0xff;			/* 随便一个值，此处无意义 */
	t->rx_buf = buf;			/* 读取到的数据 */
	t->len = len;				/* 要读取的数据长度 */
	spi_message_init(&m);		/* 初始化spi_message */
	spi_message_add_tail(t, &m);/* 将spi_transfer添加到spi_message队列 */
	ret = spi_sync(spi, &m);	/* 同步发送 */

	kfree(t);									/* 释放内存 */
	gpio_set_value(dev->cs_gpio, 1);			/* 片选拉高，释放ICM20608 */

	return ret;
}

/**
 * 连续向icm20608设备寄存器写入数据
 * 
 * @param dev ICM20608设备信息
 * @param reg 待写入设备寄存器的首地址
 * @param buf 待写入数据缓冲区首地址
 * @param len 待写入数据长度
 *
 * @return 写入寄存的操作结果
 */
static s32 icm20608_write_regs(struct icm20608_dev *dev, u8 reg, u8 *buf, u8 len)
{
	int ret;

	unsigned char txdata[len];
	struct spi_message m;
	struct spi_transfer *t;
	struct spi_device *spi = (struct spi_device *)dev->private_data;

	t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);	/* 申请内存 */
	gpio_set_value(dev->cs_gpio, 0);			/* 片选拉低 */

	/* 第1次，发送要读取的寄存地址 */
	txdata[0] = reg & ~0x80;	/* 写数据的时候寄存器地址bit8要清零 */
	t->tx_buf = txdata;			/* 要发送的数据 */
	t->len = 1;					/* 1个字节 */
	spi_message_init(&m);		/* 初始化spi_message */
	spi_message_add_tail(t, &m);/* 将spi_transfer添加到spi_message队列 */
	ret = spi_sync(spi, &m);	/* 同步发送 */

	/* 第2次，发送要写入的数据 */
	t->tx_buf = buf;			/* 要写入的数据 */
	t->len = len;				/* 写入的字节数 */
	spi_message_init(&m);		/* 初始化spi_message */
	spi_message_add_tail(t, &m);/* 将spi_transfer添加到spi_message队列 */
	ret = spi_sync(spi, &m);	/* 同步发送 */

	kfree(t);					/* 释放内存 */
	gpio_set_value(dev->cs_gpio, 1);/* 片选拉高，释放ICM20608 */
	return ret;
}

/**
 * 读取icm20608指定寄存器地址的值
 * 
 * @param dev ICM20608设备信息
 * @param reg 待读取设备寄存器的地址
 *
 * @return 读取到的寄存值
 */
static unsigned char icm20608_read_onereg(struct icm20608_dev *dev, u8 reg)
{
	u8 data = 0;
	icm20608_read_regs(dev, reg, &data, 1);
	return data;
}

/**
 * 向icm20608指定寄存器地址写入值
 * 
 * @param dev ICM20608设备信息
 * @param reg 写入设备寄存器的地址
 * @param value 写入设备寄存的值
 *
 * @return NULL
 */
static void icm20608_write_onereg(struct icm20608_dev *dev, u8 reg, u8 value)
{
	u8 buf = value;
	icm20608_write_regs(dev, reg, &buf, 1);
}

/**
 * 读取ICM20608的数据，读取原始数据，包括三轴陀螺仪、
 * 三轴加速度计和内部温度。
 * 
 * @param p_info ICM20608设备信息
 *
 * @return NULL
 */
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

/**
* 打开设备
* 
* @param inode 驱动内的节点信息
* @param filp  要打开的设备文件(文件描述符) 
*
* @return 设备打开处理结果，0表示正常
*/
static int icm20608_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &icm_info; /* 设置私有数据 */
	return 0;
}

/**
 * 从设备中读取数据
 * 
 * @param filp 要打开的设备文件(文件描述符)
 * @param buf  返回给用户空间的数据缓冲区
 * @param cnt  要读取的数据长度
 * @param off  相对于文件首地址的偏移 
 *
 * @return 读取到数据的长度，负值表示读取失败
 */
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

	if(err != 0) 
		return err;
	return cnt;
}

/**
 * icm设备关闭时执行函数
 * 
 * @param inode 驱动内的节点信息
 * @param filp 要打开的设备文件(文件描述符)
 *
 * @return 设备关闭处理结果，0表示正常
 */
static int icm20608_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* icm20608操作函数 */
static const struct file_operations icm20608_ops = {
	.owner = THIS_MODULE,
	.open = icm20608_open,
	.read = icm20608_read,
	.release = icm20608_release,
};

/**
 * icm设备硬件初始化相关函数
 * 
 * @param spi spi硬件结构体指针
 *
 * @return 设备硬件初始化结果
 */
static int icm_hardware_init(struct spi_device *spi)
{
	unsigned char value = 0;

	/* 获取设备树中cs片选信号 */
	icm_info.dev.nd = of_find_node_by_path("/soc/aips-bus@02000000/spba-bus@02000000/ecspi@02010000");
	if(icm_info.dev.nd== NULL) {
		printk("ecspi3 node not find!\r\n");
		return -EINVAL;
	} 

	/* 2、 获取设备树中的gpio属性，得到BEEP所使用的BEEP编号 */
	icm_info.dev.cs_gpio = of_get_named_gpio(icm_info.dev.nd, "cs-gpio", 0);
	if(icm_info.dev.cs_gpio < 0) {
		printk("can't get cs-gpio");
		return -EINVAL;
	}

	/* 3、设置GPIO1_IO20为输出，并且输出高电平 */
	if(gpio_direction_output(icm_info.dev.cs_gpio, 1) < 0) {
		printk("can't set gpio!\r\n");
	}

	/*初始化spi_device */
	spi->mode = SPI_MODE_0;	/*MODE0，CPOL=0，CPHA=0*/
	spi_setup(spi);
	icm_info.dev.private_data = spi; /* 设置私有数据 */

	/*设置spi内部寄存器*/
	icm20608_write_onereg(&icm_info.dev, ICM20_PWR_MGMT_1, 0x80);
	mdelay(50);
	icm20608_write_onereg(&icm_info.dev, ICM20_PWR_MGMT_1, 0x01);
	mdelay(50);

	value = icm20608_read_onereg(&icm_info.dev, ICM20_WHO_AM_I);
	printk("ICM20608 ID = %#X\r\n", value);	

	icm20608_write_onereg(&icm_info.dev, ICM20_SMPLRT_DIV, 0x00); 	/* 输出速率是内部采样率					*/
	icm20608_write_onereg(&icm_info.dev, ICM20_GYRO_CONFIG, 0x18); 	/* 陀螺仪±2000dps量程 				*/
	icm20608_write_onereg(&icm_info.dev, ICM20_ACCEL_CONFIG, 0x18); /* 加速度计±16G量程 					*/
	icm20608_write_onereg(&icm_info.dev, ICM20_CONFIG, 0x04); 		/* 陀螺仪低通滤波BW=20Hz 				*/
	icm20608_write_onereg(&icm_info.dev, ICM20_ACCEL_CONFIG2, 0x04); /* 加速度计低通滤波BW=21.2Hz 			*/
	icm20608_write_onereg(&icm_info.dev, ICM20_PWR_MGMT_2, 0x00); 	/* 打开加速度计和陀螺仪所有轴 				*/
	icm20608_write_onereg(&icm_info.dev, ICM20_LP_MODE_CFG, 0x00); 	/* 关闭低功耗 						*/
	icm20608_write_onereg(&icm_info.dev, ICM20_FIFO_EN, 0x00);		/* 关闭FIFO	*/

	return 0;
}

/**
 * spi驱动的probe函数，当驱动与设备匹配以后此函数就会执行
 * 
 * @param spi spi硬件结构体指针   
 *
 * @return 设备probe处理结果
 */
static int icm20608_probe(struct spi_device *spi)
{
	int ret = 0;

	ret = icm_hardware_init(spi);
	if(ret != 0){
		printk("icm hardware init failed!\r\n");
		return -EINVAL;
	}

	/* 1、构建设备号 */
	if (icm_info.dev.major) {
		icm_info.dev.devid = MKDEV(icm_info.dev.major, 0);
		register_chrdev_region(icm_info.dev.devid, SPI_ICM_CNT, SPI_ICM_NAME);
	} else {
		alloc_chrdev_region(&icm_info.dev.devid, 0, SPI_ICM_CNT, SPI_ICM_NAME);
		icm_info.dev.major = MAJOR(icm_info.dev.devid);
	}

	/* 2、注册设备 */
	cdev_init(&icm_info.dev.cdev, &icm20608_ops);
	cdev_add(&icm_info.dev.cdev, icm_info.dev.devid, SPI_ICM_CNT);

	/* 3、创建类 */
	icm_info.dev.class = class_create(THIS_MODULE, SPI_ICM_NAME);
	if (IS_ERR(icm_info.dev.class)) {
		return PTR_ERR(icm_info.dev.class);
	}

	/* 4、创建设备 */
	icm_info.dev.device = device_create(icm_info.dev.class, NULL, icm_info.dev.devid, NULL, SPI_ICM_NAME);
	if (IS_ERR(icm_info.dev.device)) {
		return PTR_ERR(icm_info.dev.device);
	}

	return 0;
}

/**
 * spi驱动移除函数
 * 
 * @param spi spi设备   
 *
 * @return 设备移除处理结果
 */
static int icm20608_remove(struct spi_device *spi)
{
	/* 删除设备 */
	cdev_del(&icm_info.dev.cdev);
	unregister_chrdev_region(icm_info.dev.devid, SPI_ICM_CNT);

	/* 注销掉类和设备 */
	device_destroy(icm_info.dev.class, icm_info.dev.devid);
	class_destroy(icm_info.dev.class);
	return 0;
}

/* 传统匹配方式ID列表 */
static const struct spi_device_id icm20608_id[] = {
	{"alientek,icm20608", 0},  
	{}
};

/* 设备树匹配列表 */
static const struct of_device_id icm20608_of_match[] = {
	{ .compatible = "alientek,icm20608" },
	{ /* Sentinel */ }
};

/* SPI驱动结构体 */	
static struct spi_driver icm20608_driver = {
	.probe = icm20608_probe,
	.remove = icm20608_remove,
	.driver = {
			.owner = THIS_MODULE,
		   	.name = SPI_ICM_NAME,
		   	.of_match_table = icm20608_of_match, 
		   },
	.id_table = icm20608_id,
};

/**
 * 驱动加载时执行的初始化函数
 * 
 * @param NULL    
 *
 * @return 驱动加载执行结果
 */
static int __init icm20608_module_init(void)
{
    return spi_register_driver(&icm20608_driver);
}

/**
 * 驱动释放时执行的退出函数
 * 
 * @param NULL    
 *
 * @return 驱动退出执行结果
 */
static void __exit icm20608_module_exit(void)
{
    return spi_unregister_driver(&icm20608_driver);
}

module_init(icm20608_module_init);                      
module_exit(icm20608_module_exit);
MODULE_AUTHOR("zc");				                //模块作者
MODULE_LICENSE("GPL v2");                           //模块许可协议
MODULE_DESCRIPTION("icm20608 driver");              //模块许描述
MODULE_ALIAS("spi_icm20608_driver");                //模块别名

