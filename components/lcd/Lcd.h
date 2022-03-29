#ifndef __Lcd_H
#define __Lcd_H

#include "driver/gpio.h"



typedef unsigned char   u8;
typedef unsigned short  u16;


//颜色
#define RED  	0xf800   //红色
#define GREEN	0x07e0   //绿色
#define BLUE 	0x001f   //蓝色
#define WHITE	0xffff   //白色
#define BLACK	0x0000   //黑色
#define YELLOW  0xFFE0   //黄色
#define GRAY0   0xEF7D  //灰色
#define GRAY1   0x8410  //灰色1
#define GRAY2   0x4208  //灰色2  
#define VIOLET  0xf81f  //紫色


//定义显示屏大小
#define X_MAX_PIXEL 128
#define Y_MAX_PIXEL  128

//LCD此脚定义
#define LCD_SCL GPIO_NUM_33       //SCLK  时钟 D1（SCLK）
#define LCD_SDA GPIO_NUM_14       //SDA   D0（MOSI） 数据
#define LCD_RST GPIO_NUM_12       //_RES  hardware reset   复位 
#define LCD_RS  GPIO_NUM_13       //A0  H/L 命令数据选通端，H：数据，L:命令


//SPI口置1
#define	LCD_RS_SET  	(gpio_set_level(LCD_RS, 1)) 
#define	LCD_SDA_SET  	(gpio_set_level(LCD_SDA, 1))
#define	LCD_SCL_SET  	(gpio_set_level(LCD_SCL, 1))    
#define	LCD_RST_SET  	(gpio_set_level(LCD_RST, 1))

//SPI口清0
#define	LCD_RS_CLR  	(gpio_set_level(LCD_RS, 0))    
#define	LCD_SDA_CLR  	(gpio_set_level(LCD_SDA, 0))    
#define	LCD_SCL_CLR  	(gpio_set_level(LCD_SCL, 0))    
#define	LCD_RST_CLR  	(gpio_set_level(LCD_RST, 0))    


void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num) ;
void LCD_P6x8Str(unsigned char x, unsigned char y, u16 fc, u16 bc,unsigned char ch[]);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void Lcd_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(u16 Color);
void Lcd_ClearRect(u16 x1,u16 y1, u16 x2,u16 y2, u16 Color);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
void LCD_WriteData_16Bit(u16 Data);


#endif
