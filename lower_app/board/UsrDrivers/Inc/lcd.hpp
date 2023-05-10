
#pragma once
#include "driver.hpp"

//LCD register and data 
typedef struct
{
	volatile uint16_t LCD_REG;
	volatile uint16_t LCD_RAM;
} LCD_TypeDef;			    
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0007FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

//scan director
#define L2R_U2D  0 		//从左到右,从上到下
#define L2R_D2U  1 		//从左到右,从下到上
#define R2L_U2D  2 		//从右到左,从上到下
#define R2L_D2U  3 		//从右到左,从下到上

#define U2D_L2R  4 		//从上到下,从左到右
#define U2D_R2L  5 		//从上到下,从右到左
#define D2U_L2R  6 		//从下到上,从左到右
#define D2U_R2L  7		//从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色


#define LED_BACKLIGHT_ON	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
#define LED_BACKLIGHT_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

class lcd_driver
{
public:
		bool init(void);
		static lcd_driver *get_instance()
		{
				static lcd_driver driver;
				return &driver;
		}

public:
		void lcd_clear(uint32_t color);
		void lcd_setcursor(uint16_t Xpos, uint16_t Ypos);
		void lcd_showstring(uint16_t x,uint16_t y,uint16_t width,uint16_t height, uint8_t size, char *p);
		void lcd_showchar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode);
		void fast_drawpoint(uint16_t x, uint16_t y, uint32_t color);		
		void test();

private:
		//config interface
		void hardware_init(void);
		void config_init(void);
		void display_dir(uint8_t dir);
		void lcd_scan_dir(uint8_t dir);
		
		//reg data interface
		void lcd_wr_reg(uint16_t regval);
		void lcd_wr_data(uint16_t data);
		uint16_t lcd_rd_data(void);
		void lcd_wr_reg_data(uint16_t reg, uint16_t data);
		uint16_t lcd_read_reg(uint16_t reg);

		void write_ram_prepare(void);

private:
		SRAM_HandleTypeDef hsram1;
		uint16_t id_{0};
		uint32_t width_{480};
		uint32_t height_{800};
		uint16_t dir_{0};						//横屏还是竖屏控制：0，竖屏；1，横屏。	
		uint16_t wramcmd_{0x2c00};		//开始写gram指令
		uint16_t setxcmd_{0x2a00};		//设置x坐标指令
		uint16_t setycmd_{0x2b00};		//设置y坐标指令 
};