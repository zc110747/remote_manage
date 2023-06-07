/*
 * File      : kernal_i2c_ap.c
 * This file is i2C ap3216 driver
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
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "kernal_i2c_ap.h"

//全局的指令定义
#define AP3216_NAME            		"ap3216"
#define I2C_AP_CNT					1

//ap3216设备信息
struct ap3216_dev {
	dev_t devid;				/* 设备号 	 */
	struct cdev cdev;			/* cdev 	*/
	struct class *class;		/* 类 		*/
	struct device *device;		/* 设备 	 */
	struct device_node	*nd; 	/* 设备节点 */
	int major;					/* 主设备号 */
	void *private_data;			/* 私有数据 		*/
	int cs_gpio;				/* 片选所使用的GPIO编号		*/
};

//ap3216应用信息
struct ap3216_info{
	struct ap3216_dev dev;
	unsigned short ir;
	unsigned short als;
	unsigned short ps;
};

static struct ap3216_info ap_info;

/**
 * 连续读取ap3216设备寄存器的值
 * 
 * @param dev ap3216设备信息
 * @param reg 待读取设备寄存器的首地址
 * @param buf 读取数据缓冲区首地址
 * @param len 待读取数据长度
 *
 * @return 读取寄存的操作结果
 */
static int ap3216_read_regs(struct ap3216_dev *dev, u8 reg, void *buf, int len)
{
	int ret;
	struct i2c_msg msg[2];
	struct i2c_client *client = (struct i2c_client *)dev->private_data;

	/* msg[0]为发送要读取的首地址 */
	msg[0].addr = client->addr;			/* ap3216c地址 */
	msg[0].flags = 0;					/* 标记为发送数据 */
	msg[0].buf = &reg;					/* 读取的首地址 */
	msg[0].len = 1;						/* reg长度*/

	/* msg[1]读取数据 */
	msg[1].addr = client->addr;			/* ap3216c地址 */
	msg[1].flags = I2C_M_RD;			/* 标记为读取数据*/
	msg[1].buf = buf;					/* 读取数据缓冲区 */
	msg[1].len = len;					/* 要读取的数据长度*/

	ret = i2c_transfer(client->adapter, msg, 2);
	if(ret == 2) {
		ret = 0;
	} else {
		printk("i2c rd failed=%d reg=%06x len=%d\n",ret, reg, len);
		ret = -EREMOTEIO;
	}
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
static unsigned char ap3216_read_reg(struct ap3216_dev *dev, u8 reg)
{
	u8 data = 0;

	ap3216_read_regs(dev, reg, &data, 1);
	return data;
}

/**
 * 读取AP3216的设备数据
 * 
 * @param p_info AP3216的设备信息
 *
 * @return NULL
 */
void ap3216_readdata(struct ap3216_info *p_info)
{
	unsigned char i =0;
    unsigned char buf[6];
	
	/* 循环读取所有传感器数据 */
    for(i = 0; i < 6; i++)	
    {
        buf[i] = ap3216_read_reg(&p_info->dev, AP3216C_IRDATALOW + i);	
    }

    if(buf[0]&(1<<7)) 	/* IR_OF位为1,则数据无效 */
		p_info->ir = 0;					
	else 				/* 读取IR传感器的数据   		*/
		p_info->ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0X03); 			
	
	p_info->als = ((unsigned short)buf[3] << 8) | buf[2];	/* 读取ALS传感器的数据 			 */  
	
    if(buf[4]&(1<<6))	/* IR_OF位为1,则数据无效 			*/
		p_info->ps = 0;    													
	else 				/* 读取PS传感器的数据    */
		p_info->ps = ((unsigned short)(buf[5] & 0X3F) << 4) | (buf[4] & 0X0F); 
	
}

/**
 *	向ap3216设备连续写入数据
 * 
 * @param dev ap3216的设备信息
 * @param reg 待写入设备寄存器的首地址
 * @param buf 待写入数据缓冲区首地址
 * @param len 待写入数据长度
 *
 * @return 写入寄存的操作结果
 */
static s32 ap3216_write_regs(struct ap3216_dev *dev, u8 reg, u8 *buf, u8 len)
{
	u8 b[256];
	struct i2c_msg msg;
	struct i2c_client *client = (struct i2c_client *)dev->private_data;
	
	b[0] = reg;					/* 寄存器首地址 */
	memcpy(&b[1], buf, len);		/* 将要写入的数据拷贝到数组b里面 */
		
	msg.addr = client->addr;	/* ap3216c地址 */
	msg.flags = 0;				/* 标记为写数据 */

	msg.buf = b;				/* 要写入的数据缓冲区 */
	msg.len = len + 1;			/* 要写入的数据长度 */

	return i2c_transfer(client->adapter, &msg, 1);
}

/**
 * 向ap3216指定寄存器地址写入值
 * 
 * @param dev ap3216的设备信息
 * @param reg 写入设备寄存器的地址
 * @param data 写入设备寄存的值
 *
 * @return NULL
 */
static void ap3216_write_onereg(struct ap3216_dev *dev, u8 reg, u8 data)
{
	u8 buf = 0;
	buf = data;
	ap3216_write_regs(dev, reg, &buf, 1);
}

/**
* 打开设备
* 
* @param inode 驱动内的节点信息
* @param filp  要打开的设备文件(文件描述符) 
*
* @return 设备打开处理结果，0表示正常
*/
static int ap3216_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &ap_info; /* 设置私有数据 */

	/* 初始化AP3216C */
	ap3216_write_onereg(&ap_info.dev, AP3216C_SYSTEMCONG, 0x04);		/* 复位AP3216C 			*/
	mdelay(50);															/* AP3216C复位最少10ms 	*/
	ap3216_write_onereg(&ap_info.dev, AP3216C_SYSTEMCONG, 0X03);		/* 开启ALS、PS+IR 		*/
	return 0;
}

/**
 * 从ap3216设备中读取数据
 * 
 * @param filp 要打开的设备文件(文件描述符)
 * @param buf  返回给用户空间的数据缓冲区
 * @param cnt  要读取的数据长度
 * @param off  相对于文件首地址的偏移 
 *
 * @return 读取到数据的长度，负值表示读取失败
 */
static ssize_t ap3216_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
	short data[3];
	long err = 0;

	struct ap3216_info *pApInfo = (struct ap3216_info *)filp->private_data;
	
	ap3216_readdata(pApInfo);

	data[0] = pApInfo->ir;
	data[1] = pApInfo->als;
	data[2] = pApInfo->ps;
	err = copy_to_user(buf, data, sizeof(data));
	return cnt;
}

/**
 * ap设备关闭时执行函数
 * 
 * @param inode 驱动内的节点信息
 * @param filp 要打开的设备文件(文件描述符)
 *
 * @return 设备关闭处理结果，0表示正常
 */
static int ap3216_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/* ap3216操作函数 */
static const struct file_operations ap3216_ops = {
	.owner = THIS_MODULE,
	.open = ap3216_open,
	.read = ap3216_read,
	.release = ap3216_release,
};

/**
 * i2c驱动的probe函数，执行设备的模块加载
 * 
 * @param client	设备端接口函数
 * @param id	  	设备端id信息
 *
 * @return 设备probe处理结果
 */
static int ap3216_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	/* 1、构建设备号 */
	if (ap_info.dev.major) {
		ap_info.dev.devid = MKDEV(ap_info.dev.major, 0);
		register_chrdev_region(ap_info.dev.devid, I2C_AP_CNT, AP3216_NAME);
	} else {
		alloc_chrdev_region(&ap_info.dev.devid, 0, I2C_AP_CNT, AP3216_NAME);
		ap_info.dev.major = MAJOR(ap_info.dev.devid);
	}

	/* 2、注册设备 */
	cdev_init(&ap_info.dev.cdev, &ap3216_ops);
	cdev_add(&ap_info.dev.cdev, ap_info.dev.devid, I2C_AP_CNT);

	/* 3、创建类 */
	ap_info.dev.class = class_create(THIS_MODULE, AP3216_NAME);
	if (IS_ERR(ap_info.dev.class)) {
		return PTR_ERR(ap_info.dev.class);
	}

	/* 4、创建设备 */
	ap_info.dev.device = device_create(ap_info.dev.class, NULL, ap_info.dev.devid, NULL, AP3216_NAME);
	if (IS_ERR(ap_info.dev.device)) {
		return PTR_ERR(ap_info.dev.device);
	}

	//获取client的信息
	ap_info.dev.private_data = client;

	printk(KERN_INFO"I2c-Ap3216 Driver Init Ok!\r\n");
	return 0;
}

/**
 * iic驱动移除函数
 * 
 * @param spi spi设备   
 *
 * @return 设备移除处理结果
 */
static int ap3216_remove(struct i2c_client *client)
{
	/* 删除设备 */
	cdev_del(&ap_info.dev.cdev);
	unregister_chrdev_region(ap_info.dev.devid, I2C_AP_CNT);

	/* 注销掉类和设备 */
	device_destroy(ap_info.dev.class, ap_info.dev.devid);
	class_destroy(ap_info.dev.class);
	return 0;
}

/* 传统匹配方式ID列表 -- name不重要，但必须存在 */
static const struct i2c_device_id ap3216_id[] = {
	{"ap3216", 0},  
	{}
};

/* 设备树匹配列表 */
static const struct of_device_id ap3216_of_match[] = {
	{ .compatible = "usr,ap3216" },
	{ /* Sentinel */ }
};

/* I2C驱动结构体 */	
static struct i2c_driver ap3216_driver = {
	.probe = ap3216_probe,
	.remove = ap3216_remove,
	.driver = {
			.owner = THIS_MODULE,
		   	.name = AP3216_NAME,
		   	.of_match_table = ap3216_of_match, 
		   },
	.id_table = ap3216_id,
};

/**
 * 驱动加载时执行的初始化函数
 * 
 * @param NULL    
 *
 * @return 驱动加载执行结果
 */
static int __init ap3216_module_init(void)
{
    return i2c_add_driver(&ap3216_driver);
}

/**
 * 驱动释放时执行的退出函数
 * 
 * @param NULL    
 *
 * @return 驱动退出执行结果
 */
static void __exit ap3216_module_exit(void)
{
    return i2c_del_driver(&ap3216_driver);
}

module_init(ap3216_module_init);                      
module_exit(ap3216_module_exit);
MODULE_AUTHOR("zc");				                //模块作者
MODULE_LICENSE("GPL v2");                           //模块许可协议
MODULE_DESCRIPTION("ap3216 driver");                //模块许描述
MODULE_ALIAS("i2c_ap3216_driver");                  //模块别名

