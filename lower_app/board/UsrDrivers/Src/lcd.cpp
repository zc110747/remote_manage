//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      lcd.cpp
//
//  Purpose:
//      lcd driver use fmc.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "lcd.hpp"
#include "font.h"

uint32_t POINT_COLOR = 0xFF000000;		
uint32_t BACK_COLOR = 0xFFFFFFFF;  	 

BaseType_t lcd_driver::init(void)
{
    BaseType_t result;
    
    result = hardware_init();
    if(result == pdPASS)
    {
        HAL_Delay(20);

        config_init(); 
        
        test();     
    }
    else
    {
        printf("lcd hardware_init failed\r\n");
    }
    return result;
}

void lcd_driver::config_init(void)
{
    //read device id
    lcd_wr_reg(0XDA00);	
    id_ = lcd_rd_data(); 
    lcd_wr_reg(0XDB00);	
    id_ = lcd_rd_data();
    id_<<=8;	
    lcd_wr_reg(0XDC00);	
    id_ |= lcd_rd_data();

    if(id_ == 0x8000)
    {
        lcd_wr_reg_data(0xF000,0x55);
        lcd_wr_reg_data(0xF001,0xAA);
        lcd_wr_reg_data(0xF002,0x52);
        lcd_wr_reg_data(0xF003,0x08);
        lcd_wr_reg_data(0xF004,0x01);
        //AVDD Set AVDD 5.2V
        lcd_wr_reg_data(0xB000,0x0D);
        lcd_wr_reg_data(0xB001,0x0D);
        lcd_wr_reg_data(0xB002,0x0D);
        //AVDD ratio
        lcd_wr_reg_data(0xB600,0x34);
        lcd_wr_reg_data(0xB601,0x34);
        lcd_wr_reg_data(0xB602,0x34);
        //AVEE -5.2V
        lcd_wr_reg_data(0xB100,0x0D);
        lcd_wr_reg_data(0xB101,0x0D);
        lcd_wr_reg_data(0xB102,0x0D);
        //AVEE ratio
        lcd_wr_reg_data(0xB700,0x34);
        lcd_wr_reg_data(0xB701,0x34);
        lcd_wr_reg_data(0xB702,0x34);
        //VCL -2.5V
        lcd_wr_reg_data(0xB200,0x00);
        lcd_wr_reg_data(0xB201,0x00);
        lcd_wr_reg_data(0xB202,0x00);
        //VCL ratio
        lcd_wr_reg_data(0xB800,0x24);
        lcd_wr_reg_data(0xB801,0x24);
        lcd_wr_reg_data(0xB802,0x24);
        //VGH 15V (Free pump)
        lcd_wr_reg_data(0xBF00,0x01);
        lcd_wr_reg_data(0xB300,0x0F);
        lcd_wr_reg_data(0xB301,0x0F);
        lcd_wr_reg_data(0xB302,0x0F);
        //VGH ratio
        lcd_wr_reg_data(0xB900,0x34);
        lcd_wr_reg_data(0xB901,0x34);
        lcd_wr_reg_data(0xB902,0x34);
        //VGL_REG -10V
        lcd_wr_reg_data(0xB500,0x08);
        lcd_wr_reg_data(0xB501,0x08);
        lcd_wr_reg_data(0xB502,0x08);
        lcd_wr_reg_data(0xC200,0x03);
        //VGLX ratio
        lcd_wr_reg_data(0xBA00,0x24);
        lcd_wr_reg_data(0xBA01,0x24);
        lcd_wr_reg_data(0xBA02,0x24);
        //VGMP/VGSP 4.5V/0V
        lcd_wr_reg_data(0xBC00,0x00);
        lcd_wr_reg_data(0xBC01,0x78);
        lcd_wr_reg_data(0xBC02,0x00);
        //VGMN/VGSN -4.5V/0V
        lcd_wr_reg_data(0xBD00,0x00);
        lcd_wr_reg_data(0xBD01,0x78);
        lcd_wr_reg_data(0xBD02,0x00);
        //VCOM
        lcd_wr_reg_data(0xBE00,0x00);
        lcd_wr_reg_data(0xBE01,0x64);
        //Gamma Setting
        lcd_wr_reg_data(0xD100,0x00);
        lcd_wr_reg_data(0xD101,0x33);
        lcd_wr_reg_data(0xD102,0x00);
        lcd_wr_reg_data(0xD103,0x34);
        lcd_wr_reg_data(0xD104,0x00);
        lcd_wr_reg_data(0xD105,0x3A);
        lcd_wr_reg_data(0xD106,0x00);
        lcd_wr_reg_data(0xD107,0x4A);
        lcd_wr_reg_data(0xD108,0x00);
        lcd_wr_reg_data(0xD109,0x5C);
        lcd_wr_reg_data(0xD10A,0x00);
        lcd_wr_reg_data(0xD10B,0x81);
        lcd_wr_reg_data(0xD10C,0x00);
        lcd_wr_reg_data(0xD10D,0xA6);
        lcd_wr_reg_data(0xD10E,0x00);
        lcd_wr_reg_data(0xD10F,0xE5);
        lcd_wr_reg_data(0xD110,0x01);
        lcd_wr_reg_data(0xD111,0x13);
        lcd_wr_reg_data(0xD112,0x01);
        lcd_wr_reg_data(0xD113,0x54);
        lcd_wr_reg_data(0xD114,0x01);
        lcd_wr_reg_data(0xD115,0x82);
        lcd_wr_reg_data(0xD116,0x01);
        lcd_wr_reg_data(0xD117,0xCA);
        lcd_wr_reg_data(0xD118,0x02);
        lcd_wr_reg_data(0xD119,0x00);
        lcd_wr_reg_data(0xD11A,0x02);
        lcd_wr_reg_data(0xD11B,0x01);
        lcd_wr_reg_data(0xD11C,0x02);
        lcd_wr_reg_data(0xD11D,0x34);
        lcd_wr_reg_data(0xD11E,0x02);
        lcd_wr_reg_data(0xD11F,0x67);
        lcd_wr_reg_data(0xD120,0x02);
        lcd_wr_reg_data(0xD121,0x84);
        lcd_wr_reg_data(0xD122,0x02);
        lcd_wr_reg_data(0xD123,0xA4);
        lcd_wr_reg_data(0xD124,0x02);
        lcd_wr_reg_data(0xD125,0xB7);
        lcd_wr_reg_data(0xD126,0x02);
        lcd_wr_reg_data(0xD127,0xCF);
        lcd_wr_reg_data(0xD128,0x02);
        lcd_wr_reg_data(0xD129,0xDE);
        lcd_wr_reg_data(0xD12A,0x02);
        lcd_wr_reg_data(0xD12B,0xF2);
        lcd_wr_reg_data(0xD12C,0x02);
        lcd_wr_reg_data(0xD12D,0xFE);
        lcd_wr_reg_data(0xD12E,0x03);
        lcd_wr_reg_data(0xD12F,0x10);
        lcd_wr_reg_data(0xD130,0x03);
        lcd_wr_reg_data(0xD131,0x33);
        lcd_wr_reg_data(0xD132,0x03);
        lcd_wr_reg_data(0xD133,0x6D);
        lcd_wr_reg_data(0xD200,0x00);
        lcd_wr_reg_data(0xD201,0x33);
        lcd_wr_reg_data(0xD202,0x00);
        lcd_wr_reg_data(0xD203,0x34);
        lcd_wr_reg_data(0xD204,0x00);
        lcd_wr_reg_data(0xD205,0x3A);
        lcd_wr_reg_data(0xD206,0x00);
        lcd_wr_reg_data(0xD207,0x4A);
        lcd_wr_reg_data(0xD208,0x00);
        lcd_wr_reg_data(0xD209,0x5C);
        lcd_wr_reg_data(0xD20A,0x00);

        lcd_wr_reg_data(0xD20B,0x81);
        lcd_wr_reg_data(0xD20C,0x00);
        lcd_wr_reg_data(0xD20D,0xA6);
        lcd_wr_reg_data(0xD20E,0x00);
        lcd_wr_reg_data(0xD20F,0xE5);
        lcd_wr_reg_data(0xD210,0x01);
        lcd_wr_reg_data(0xD211,0x13);
        lcd_wr_reg_data(0xD212,0x01);
        lcd_wr_reg_data(0xD213,0x54);
        lcd_wr_reg_data(0xD214,0x01);
        lcd_wr_reg_data(0xD215,0x82);
        lcd_wr_reg_data(0xD216,0x01);
        lcd_wr_reg_data(0xD217,0xCA);
        lcd_wr_reg_data(0xD218,0x02);
        lcd_wr_reg_data(0xD219,0x00);
        lcd_wr_reg_data(0xD21A,0x02);
        lcd_wr_reg_data(0xD21B,0x01);
        lcd_wr_reg_data(0xD21C,0x02);
        lcd_wr_reg_data(0xD21D,0x34);
        lcd_wr_reg_data(0xD21E,0x02);
        lcd_wr_reg_data(0xD21F,0x67);
        lcd_wr_reg_data(0xD220,0x02);
        lcd_wr_reg_data(0xD221,0x84);
        lcd_wr_reg_data(0xD222,0x02);
        lcd_wr_reg_data(0xD223,0xA4);
        lcd_wr_reg_data(0xD224,0x02);
        lcd_wr_reg_data(0xD225,0xB7);
        lcd_wr_reg_data(0xD226,0x02);
        lcd_wr_reg_data(0xD227,0xCF);
        lcd_wr_reg_data(0xD228,0x02);
        lcd_wr_reg_data(0xD229,0xDE);
        lcd_wr_reg_data(0xD22A,0x02);
        lcd_wr_reg_data(0xD22B,0xF2);
        lcd_wr_reg_data(0xD22C,0x02);
        lcd_wr_reg_data(0xD22D,0xFE);
        lcd_wr_reg_data(0xD22E,0x03);
        lcd_wr_reg_data(0xD22F,0x10);
        lcd_wr_reg_data(0xD230,0x03);
        lcd_wr_reg_data(0xD231,0x33);
        lcd_wr_reg_data(0xD232,0x03);
        lcd_wr_reg_data(0xD233,0x6D);
        lcd_wr_reg_data(0xD300,0x00);
        lcd_wr_reg_data(0xD301,0x33);
        lcd_wr_reg_data(0xD302,0x00);
        lcd_wr_reg_data(0xD303,0x34);
        lcd_wr_reg_data(0xD304,0x00);
        lcd_wr_reg_data(0xD305,0x3A);
        lcd_wr_reg_data(0xD306,0x00);
        lcd_wr_reg_data(0xD307,0x4A);
        lcd_wr_reg_data(0xD308,0x00);
        lcd_wr_reg_data(0xD309,0x5C);
        lcd_wr_reg_data(0xD30A,0x00);

        lcd_wr_reg_data(0xD30B,0x81);
        lcd_wr_reg_data(0xD30C,0x00);
        lcd_wr_reg_data(0xD30D,0xA6);
        lcd_wr_reg_data(0xD30E,0x00);
        lcd_wr_reg_data(0xD30F,0xE5);
        lcd_wr_reg_data(0xD310,0x01);
        lcd_wr_reg_data(0xD311,0x13);
        lcd_wr_reg_data(0xD312,0x01);
        lcd_wr_reg_data(0xD313,0x54);
        lcd_wr_reg_data(0xD314,0x01);
        lcd_wr_reg_data(0xD315,0x82);
        lcd_wr_reg_data(0xD316,0x01);
        lcd_wr_reg_data(0xD317,0xCA);
        lcd_wr_reg_data(0xD318,0x02);
        lcd_wr_reg_data(0xD319,0x00);
        lcd_wr_reg_data(0xD31A,0x02);
        lcd_wr_reg_data(0xD31B,0x01);
        lcd_wr_reg_data(0xD31C,0x02);
        lcd_wr_reg_data(0xD31D,0x34);
        lcd_wr_reg_data(0xD31E,0x02);
        lcd_wr_reg_data(0xD31F,0x67);
        lcd_wr_reg_data(0xD320,0x02);
        lcd_wr_reg_data(0xD321,0x84);
        lcd_wr_reg_data(0xD322,0x02);
        lcd_wr_reg_data(0xD323,0xA4);
        lcd_wr_reg_data(0xD324,0x02);
        lcd_wr_reg_data(0xD325,0xB7);
        lcd_wr_reg_data(0xD326,0x02);
        lcd_wr_reg_data(0xD327,0xCF);
        lcd_wr_reg_data(0xD328,0x02);
        lcd_wr_reg_data(0xD329,0xDE);
        lcd_wr_reg_data(0xD32A,0x02);
        lcd_wr_reg_data(0xD32B,0xF2);
        lcd_wr_reg_data(0xD32C,0x02);
        lcd_wr_reg_data(0xD32D,0xFE);
        lcd_wr_reg_data(0xD32E,0x03);
        lcd_wr_reg_data(0xD32F,0x10);
        lcd_wr_reg_data(0xD330,0x03);
        lcd_wr_reg_data(0xD331,0x33);
        lcd_wr_reg_data(0xD332,0x03);
        lcd_wr_reg_data(0xD333,0x6D);
        lcd_wr_reg_data(0xD400,0x00);
        lcd_wr_reg_data(0xD401,0x33);
        lcd_wr_reg_data(0xD402,0x00);
        lcd_wr_reg_data(0xD403,0x34);
        lcd_wr_reg_data(0xD404,0x00);
        lcd_wr_reg_data(0xD405,0x3A);
        lcd_wr_reg_data(0xD406,0x00);
        lcd_wr_reg_data(0xD407,0x4A);
        lcd_wr_reg_data(0xD408,0x00);
        lcd_wr_reg_data(0xD409,0x5C);
        lcd_wr_reg_data(0xD40A,0x00);
        lcd_wr_reg_data(0xD40B,0x81);

        lcd_wr_reg_data(0xD40C,0x00);
        lcd_wr_reg_data(0xD40D,0xA6);
        lcd_wr_reg_data(0xD40E,0x00);
        lcd_wr_reg_data(0xD40F,0xE5);
        lcd_wr_reg_data(0xD410,0x01);
        lcd_wr_reg_data(0xD411,0x13);
        lcd_wr_reg_data(0xD412,0x01);
        lcd_wr_reg_data(0xD413,0x54);
        lcd_wr_reg_data(0xD414,0x01);
        lcd_wr_reg_data(0xD415,0x82);
        lcd_wr_reg_data(0xD416,0x01);
        lcd_wr_reg_data(0xD417,0xCA);
        lcd_wr_reg_data(0xD418,0x02);
        lcd_wr_reg_data(0xD419,0x00);
        lcd_wr_reg_data(0xD41A,0x02);
        lcd_wr_reg_data(0xD41B,0x01);
        lcd_wr_reg_data(0xD41C,0x02);
        lcd_wr_reg_data(0xD41D,0x34);
        lcd_wr_reg_data(0xD41E,0x02);
        lcd_wr_reg_data(0xD41F,0x67);
        lcd_wr_reg_data(0xD420,0x02);
        lcd_wr_reg_data(0xD421,0x84);
        lcd_wr_reg_data(0xD422,0x02);
        lcd_wr_reg_data(0xD423,0xA4);
        lcd_wr_reg_data(0xD424,0x02);
        lcd_wr_reg_data(0xD425,0xB7);
        lcd_wr_reg_data(0xD426,0x02);
        lcd_wr_reg_data(0xD427,0xCF);
        lcd_wr_reg_data(0xD428,0x02);
        lcd_wr_reg_data(0xD429,0xDE);
        lcd_wr_reg_data(0xD42A,0x02);
        lcd_wr_reg_data(0xD42B,0xF2);
        lcd_wr_reg_data(0xD42C,0x02);
        lcd_wr_reg_data(0xD42D,0xFE);
        lcd_wr_reg_data(0xD42E,0x03);
        lcd_wr_reg_data(0xD42F,0x10);
        lcd_wr_reg_data(0xD430,0x03);
        lcd_wr_reg_data(0xD431,0x33);
        lcd_wr_reg_data(0xD432,0x03);
        lcd_wr_reg_data(0xD433,0x6D);
        lcd_wr_reg_data(0xD500,0x00);
        lcd_wr_reg_data(0xD501,0x33);
        lcd_wr_reg_data(0xD502,0x00);
        lcd_wr_reg_data(0xD503,0x34);
        lcd_wr_reg_data(0xD504,0x00);
        lcd_wr_reg_data(0xD505,0x3A);
        lcd_wr_reg_data(0xD506,0x00);
        lcd_wr_reg_data(0xD507,0x4A);
        lcd_wr_reg_data(0xD508,0x00);
        lcd_wr_reg_data(0xD509,0x5C);
        lcd_wr_reg_data(0xD50A,0x00);
        lcd_wr_reg_data(0xD50B,0x81);

        lcd_wr_reg_data(0xD50C,0x00);
        lcd_wr_reg_data(0xD50D,0xA6);
        lcd_wr_reg_data(0xD50E,0x00);
        lcd_wr_reg_data(0xD50F,0xE5);
        lcd_wr_reg_data(0xD510,0x01);
        lcd_wr_reg_data(0xD511,0x13);
        lcd_wr_reg_data(0xD512,0x01);
        lcd_wr_reg_data(0xD513,0x54);
        lcd_wr_reg_data(0xD514,0x01);
        lcd_wr_reg_data(0xD515,0x82);
        lcd_wr_reg_data(0xD516,0x01);
        lcd_wr_reg_data(0xD517,0xCA);
        lcd_wr_reg_data(0xD518,0x02);
        lcd_wr_reg_data(0xD519,0x00);
        lcd_wr_reg_data(0xD51A,0x02);
        lcd_wr_reg_data(0xD51B,0x01);
        lcd_wr_reg_data(0xD51C,0x02);
        lcd_wr_reg_data(0xD51D,0x34);
        lcd_wr_reg_data(0xD51E,0x02);
        lcd_wr_reg_data(0xD51F,0x67);
        lcd_wr_reg_data(0xD520,0x02);
        lcd_wr_reg_data(0xD521,0x84);
        lcd_wr_reg_data(0xD522,0x02);
        lcd_wr_reg_data(0xD523,0xA4);
        lcd_wr_reg_data(0xD524,0x02);
        lcd_wr_reg_data(0xD525,0xB7);
        lcd_wr_reg_data(0xD526,0x02);
        lcd_wr_reg_data(0xD527,0xCF);
        lcd_wr_reg_data(0xD528,0x02);
        lcd_wr_reg_data(0xD529,0xDE);
        lcd_wr_reg_data(0xD52A,0x02);
        lcd_wr_reg_data(0xD52B,0xF2);
        lcd_wr_reg_data(0xD52C,0x02);
        lcd_wr_reg_data(0xD52D,0xFE);
        lcd_wr_reg_data(0xD52E,0x03);
        lcd_wr_reg_data(0xD52F,0x10);
        lcd_wr_reg_data(0xD530,0x03);
        lcd_wr_reg_data(0xD531,0x33);
        lcd_wr_reg_data(0xD532,0x03);
        lcd_wr_reg_data(0xD533,0x6D);
        lcd_wr_reg_data(0xD600,0x00);
        lcd_wr_reg_data(0xD601,0x33);
        lcd_wr_reg_data(0xD602,0x00);
        lcd_wr_reg_data(0xD603,0x34);
        lcd_wr_reg_data(0xD604,0x00);
        lcd_wr_reg_data(0xD605,0x3A);
        lcd_wr_reg_data(0xD606,0x00);
        lcd_wr_reg_data(0xD607,0x4A);
        lcd_wr_reg_data(0xD608,0x00);
        lcd_wr_reg_data(0xD609,0x5C);
        lcd_wr_reg_data(0xD60A,0x00);
        lcd_wr_reg_data(0xD60B,0x81);

        lcd_wr_reg_data(0xD60C,0x00);
        lcd_wr_reg_data(0xD60D,0xA6);
        lcd_wr_reg_data(0xD60E,0x00);
        lcd_wr_reg_data(0xD60F,0xE5);
        lcd_wr_reg_data(0xD610,0x01);
        lcd_wr_reg_data(0xD611,0x13);
        lcd_wr_reg_data(0xD612,0x01);
        lcd_wr_reg_data(0xD613,0x54);
        lcd_wr_reg_data(0xD614,0x01);
        lcd_wr_reg_data(0xD615,0x82);
        lcd_wr_reg_data(0xD616,0x01);
        lcd_wr_reg_data(0xD617,0xCA);
        lcd_wr_reg_data(0xD618,0x02);
        lcd_wr_reg_data(0xD619,0x00);
        lcd_wr_reg_data(0xD61A,0x02);
        lcd_wr_reg_data(0xD61B,0x01);
        lcd_wr_reg_data(0xD61C,0x02);
        lcd_wr_reg_data(0xD61D,0x34);
        lcd_wr_reg_data(0xD61E,0x02);
        lcd_wr_reg_data(0xD61F,0x67);
        lcd_wr_reg_data(0xD620,0x02);
        lcd_wr_reg_data(0xD621,0x84);
        lcd_wr_reg_data(0xD622,0x02);
        lcd_wr_reg_data(0xD623,0xA4);
        lcd_wr_reg_data(0xD624,0x02);
        lcd_wr_reg_data(0xD625,0xB7);
        lcd_wr_reg_data(0xD626,0x02);
        lcd_wr_reg_data(0xD627,0xCF);
        lcd_wr_reg_data(0xD628,0x02);
        lcd_wr_reg_data(0xD629,0xDE);
        lcd_wr_reg_data(0xD62A,0x02);
        lcd_wr_reg_data(0xD62B,0xF2);
        lcd_wr_reg_data(0xD62C,0x02);
        lcd_wr_reg_data(0xD62D,0xFE);
        lcd_wr_reg_data(0xD62E,0x03);
        lcd_wr_reg_data(0xD62F,0x10);
        lcd_wr_reg_data(0xD630,0x03);
        lcd_wr_reg_data(0xD631,0x33);
        lcd_wr_reg_data(0xD632,0x03);
        lcd_wr_reg_data(0xD633,0x6D);
        //LV2 Page 0 enable
        lcd_wr_reg_data(0xF000,0x55);
        lcd_wr_reg_data(0xF001,0xAA);
        lcd_wr_reg_data(0xF002,0x52);
        lcd_wr_reg_data(0xF003,0x08);
        lcd_wr_reg_data(0xF004,0x00);
        //Display control
        lcd_wr_reg_data(0xB100, 0xCC);
        lcd_wr_reg_data(0xB101, 0x00);
        //Source hold time
        lcd_wr_reg_data(0xB600,0x05);
        //Gate EQ control
        lcd_wr_reg_data(0xB700,0x70);
        lcd_wr_reg_data(0xB701,0x70);
        //Source EQ control (Mode 2)
        lcd_wr_reg_data(0xB800,0x01);
        lcd_wr_reg_data(0xB801,0x03);
        lcd_wr_reg_data(0xB802,0x03);
        lcd_wr_reg_data(0xB803,0x03);
        //Inversion mode (2-dot)
        lcd_wr_reg_data(0xBC00,0x02);
        lcd_wr_reg_data(0xBC01,0x00);
        lcd_wr_reg_data(0xBC02,0x00);
        //Timing control 4H w/ 4-delay
        lcd_wr_reg_data(0xC900,0xD0);
        lcd_wr_reg_data(0xC901,0x02);
        lcd_wr_reg_data(0xC902,0x50);
        lcd_wr_reg_data(0xC903,0x50);
        lcd_wr_reg_data(0xC904,0x50);
        lcd_wr_reg_data(0x3500,0x00);
        lcd_wr_reg_data(0x3A00,0x55);  //16-bit/pixel
        lcd_wr_reg(0x1100);
        delay_us(120);
        lcd_wr_reg(0x2900);  
    }
                                    
    FMC_Bank1E->BWTR[0]&=~(0XF<<0);	 
    FMC_Bank1E->BWTR[0]&=~(0XF<<8);	
    FMC_Bank1E->BWTR[0]|=4<<0;	 		 
    FMC_Bank1E->BWTR[0]|=4<<8; 		

    display_dir(0);

    lcd_clear(WHITE);

    LED_BACKLIGHT_ON;
}

void lcd_driver::lcd_scan_dir(uint8_t dir)
{
    uint16_t regval=0;
    uint16_t dirreg=0;
    uint16_t temp;  

    //if(id==0x9341||id==0X5310||id==0X5510||id==0X1963)//9341/5310/5510/1963
    {
        switch(dir)
        {
            case L2R_U2D:
                regval|=(0<<7)|(0<<6)|(0<<5); 
                break;
            case L2R_D2U:
                regval|=(1<<7)|(0<<6)|(0<<5); 
                break;
            case R2L_U2D:
                regval|=(0<<7)|(1<<6)|(0<<5); 
                break;
            case R2L_D2U:
                regval|=(1<<7)|(1<<6)|(0<<5); 
                break;	 
            case U2D_L2R:
                regval|=(0<<7)|(0<<6)|(1<<5); 
                break;
            case U2D_R2L:
                regval|=(0<<7)|(1<<6)|(1<<5); 
                break;
            case D2U_L2R:
                regval|=(1<<7)|(0<<6)|(1<<5); 
                break;
            case D2U_R2L:
                regval|=(1<<7)|(1<<6)|(1<<5); 
                break;	 
        }
        dirreg = 0X3600;

        lcd_wr_reg_data(dirreg, regval);
        //if(id!=0X1963)
        {
            if(regval&0X20)
            {
                if(width_< height_)
                {
                    temp = width_;
                    width_= height_;
                    height_=temp;
                }
            }
            else  
            {
                if(width_ > height_)
                {
                    temp = width_;
                    width_= height_;
                    height_ =temp;
                }
            }  
        }

        lcd_wr_reg(setxcmd_);
        lcd_wr_data(0); 
        lcd_wr_reg(setxcmd_+1);
        lcd_wr_data(0); 
        lcd_wr_reg(setxcmd_+2);
        lcd_wr_data((width_-1)>>8); 
        lcd_wr_reg(setxcmd_+3);
        lcd_wr_data((width_-1)&0XFF); 
        lcd_wr_reg(setycmd_);
        lcd_wr_data(0); 
        lcd_wr_reg(setycmd_+1);
        lcd_wr_data(0); 
        lcd_wr_reg(setycmd_+2);
        lcd_wr_data((height_-1)>>8); 
        lcd_wr_reg(setycmd_+3);
        lcd_wr_data((height_-1)&0XFF);
    } 
}  

void lcd_driver::display_dir(uint8_t dir)
{
    dir_ = dir;
    if(dir == 0)
    {
        wramcmd_= 0X2c00;
        setxcmd_= 0X2A00;
        setycmd_ = 0X2B00; 
        width_ = 480;
        height_ = 800;
    }
    else
    {
        wramcmd_ = 0X2C00;
        setxcmd_ = 0X2A00;
        setycmd_ = 0X2B00; 
        width_ = 800;
        height_ = 480;
    }
    lcd_scan_dir(DFT_SCAN_DIR);
}

void lcd_driver::lcd_setcursor(uint16_t Xpos, uint16_t Ypos)
{	 
    lcd_wr_reg(setxcmd_);lcd_wr_data(Xpos>>8); 		
    lcd_wr_reg(setxcmd_+1);lcd_wr_data(Xpos&0XFF);			 
    lcd_wr_reg(setycmd_);lcd_wr_data(Ypos>>8);  		
    lcd_wr_reg(setycmd_+1);lcd_wr_data(Ypos&0XFF);			
} 

void lcd_driver::write_ram_prepare(void)
{
    LCD->LCD_REG = wramcmd_;
    __NOP();
}	 

void lcd_driver::lcd_clear(uint32_t color)
{
    uint32_t index=0;      
    uint32_t totalpoint= width_; 
    totalpoint *= height_; 						
    lcd_setcursor(0x00, 0x0000);		
    write_ram_prepare();     		  
    for(index=0; index<totalpoint; index++)
    {
        LCD->LCD_RAM=color;	
    } 
}

BaseType_t lcd_driver::hardware_init(void)
{
    FMC_NORSRAM_TimingTypeDef Timing = {0};
    FMC_NORSRAM_TimingTypeDef ExtTiming = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* USER CODE BEGIN FMC_Init 1 */

    /* USER CODE END FMC_Init 1 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

    /*Configure GPIO pin : PB5 */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /** Perform the SRAM1 memory initialization sequence
     */
    hsram1.Instance = FMC_NORSRAM_DEVICE;
    hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram1.Init */
    hsram1.Init.NSBank = FMC_NORSRAM_BANK1;
    hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
    hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
    hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
    hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram1.Init.WrapMode = FMC_WRAP_MODE_DISABLE;
    hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
    hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
    hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
    hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
    hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
    hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
    hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
    
    /* Timing */
    Timing.AddressSetupTime = 15;
    Timing.AddressHoldTime = 15;
    Timing.DataSetupTime = 70;
    Timing.BusTurnAroundDuration = 1;
    Timing.CLKDivision = 16;
    Timing.DataLatency = 17;
    Timing.AccessMode = FMC_ACCESS_MODE_A;
    
    /* ExtTiming */
    ExtTiming.AddressSetupTime = 15;
    ExtTiming.AddressHoldTime = 15;
    ExtTiming.DataSetupTime = 15;
    ExtTiming.BusTurnAroundDuration = 1;
    ExtTiming.CLKDivision = 16;
    ExtTiming.DataLatency = 17;
    ExtTiming.AccessMode = FMC_ACCESS_MODE_A;

    if (HAL_SRAM_Init(&hsram1, &Timing, &ExtTiming) != HAL_OK)
        return pdFAIL;
    
    return pdPASS;
    
}

void lcd_driver::lcd_wr_reg(uint16_t regval)
{
    __NOP();
    LCD->LCD_REG = regval;
}

void lcd_driver::lcd_wr_data(uint16_t data)
{
    __NOP();
    LCD->LCD_RAM = data;
}

uint16_t lcd_driver::lcd_rd_data(void)
{
    volatile uint16_t ram;
    __NOP(); 
    ram = LCD->LCD_RAM;
    return ram;
}

void lcd_driver::lcd_wr_reg_data(uint16_t reg, uint16_t data)
{
    __NOP(); 
    LCD->LCD_REG = reg;
    LCD->LCD_RAM = data;
}

uint16_t lcd_driver::lcd_read_reg(uint16_t reg)
{
    lcd_wr_reg(reg);
    delay_us(5);
    return lcd_rd_data();
}

void lcd_driver::fast_drawpoint(uint16_t x, uint16_t y, uint32_t color)
{	   
    lcd_wr_reg(setxcmd_); lcd_wr_data(x>>8);  
    lcd_wr_reg(setxcmd_+1); lcd_wr_data(x&0XFF);	  
    lcd_wr_reg(setycmd_); lcd_wr_data(y>>8);  
    lcd_wr_reg(setycmd_+1); lcd_wr_data(y&0XFF); 

    LCD->LCD_REG = wramcmd_; 
    LCD->LCD_RAM = color; 
}	

void lcd_driver::lcd_showchar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		
    num=num-' ';
    for(t=0;t<csize;t++)
    { 
        switch(size)
        {
            #if SUPPORT_ASCII_1206 == 1
            case 12:
                temp = asc2_1206[num][t];
                break;
            #endif
            
            #if SUPPORT_ASCII_1608 == 1
            case 16:
                temp = asc2_1608[num][t];	
                break;
            #endif
            
            #if SUPPORT_ASCII_2412 == 1
            case 24:
                temp = asc2_2412[num][t];	
                break; 
            #endif
            
            #if SUPPORT_ASCII_3216 == 1
            case 32:
                temp = asc2_3216[num][t];	
                break;
            #endif
            
            default:
                return;
        }
                            
        for(t1=0;t1<8;t1++)
        {			    
            if(temp&0x80)	
                fast_drawpoint(x,y,POINT_COLOR);
            else if(mode==0)
                fast_drawpoint(x,y,BACK_COLOR);
            temp<<=1;
            y++;
            if(y>=height_)
                return;		
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=width_)return;	
                break;
            }
        }
    }
}
	
void lcd_driver::lcd_showstring(uint16_t x,uint16_t y,uint16_t width,uint16_t height, uint8_t size, char *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while((*p<='~')&&(*p>=' '))
    {       
        if(x>=width)
        {
            x=x0;
            y+=size;
        }
        if(y >= height)
            break;

        lcd_showchar(x, y, *p, size, 0);
        x+=size/2;
        p++;
    }
}

uint32_t lcd_driver::lcd_pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}		

void lcd_driver::lcd_show_num(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{  
    uint8_t t,temp;
    uint8_t enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/lcd_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                if(mode&0X80)lcd_showchar(x+(size/2)*t,y,'0',size,mode&0X01);  
                else lcd_showchar(x+(size/2)*t,y,' ',size,mode&0X01);  
                continue;
            }
            else 
                enshow=1;
        }
        lcd_showchar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
    }
} 

void lcd_driver::lcd_show_extra_num(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{  
    uint8_t t,temp;
    uint8_t enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/lcd_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                if(mode&0X80)lcd_showchar(x+(size/2)*t,y,'0',size,mode&0X01);  
                else lcd_showchar(x+(size/2)*t,y,' ',size,mode&0X01);  
                continue;
            }
            else 
                enshow=1;
        }
        lcd_showchar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
    }
} 

void lcd_driver::test()
{
#if LCD_TEST == 1
    lcd_clear(WHITE);
    HAL_Delay(100);
    lcd_clear(BLACK);
    HAL_Delay(100);
    lcd_clear(BLUE);
    HAL_Delay(100);
    lcd_clear(RED);
    HAL_Delay(100);
    lcd_clear(MAGENTA);
    HAL_Delay(100);
    lcd_clear(GREEN);
    HAL_Delay(100);
    lcd_clear(WHITE);
    
    lcd_showstring(10, 40, 320, 32, 32, (char *)"Apollo STM32F4/F7");
    lcd_showstring(10, 80, 240, 24, 24, (char *)"TFTLCD TEST");
    lcd_showstring(10, 110, 240, 16, 16, (char *)"ATOM@ALIENTEK");
    lcd_showstring(10, 140, 320, 16, 16, (char *)"TEMPERATE: 00.00C, Vol:00.00V");
    lcd_showstring(10, 160, 200, 16, 16, (char *)"Timer: 00-00-00 00:00:00");
#endif
}