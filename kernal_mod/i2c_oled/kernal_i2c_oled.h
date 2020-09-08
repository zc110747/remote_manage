/*
 * File      : kernal_i2c_oled.h
 * This file is oled sh1106 driver 
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-27     zc           the first version
 */
#ifndef _KERNAL_I2C_OLED_H
#define _KERNAL_I2C_OLED_H

//--------------OLED参数定义---------------------
#define OLED_PAGE_SIZE    8
#define XLevelL		    0x02
#define XLevelH		    0x10
#define YLevel       0xB0
#define	Brightness	 0xFF 
#define WIDTH 	     128
#define HEIGHT 	     64	

//-------------写命令和数据定义-------------------
#define OLED_CMD     0	//写命令
#define OLED_DATA    1	//写数据    						  						

#endif
