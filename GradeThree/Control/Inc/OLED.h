#ifndef __OLED_H
#define __OLED_H
#include "sys.h"
    						  
//-----------------OLED端口定义----------------  		
#define OLED_RST    PCout(14)
#define OLED_DC     PCout(15)
 
//使用4线串行接口时使用 
#define OLED_SCLK   PAout(0)  //D0
#define OLED_SDIN   PCout(13)  //D1

#define OLED_CMD  	0		//写命令
#define OLED_DATA 	1		//写数据

//OLED控制用函数
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);
void myOLED_Decimals(uint8_t x,uint8_t y, float als);
void DisplayFont_16X16(u8 x,u8 y,u32 len,const unsigned char *dispdata);
#endif
