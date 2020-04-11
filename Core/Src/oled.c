/**
  ******************************************************************************
  * @file           : oled.c
  * @brief          : oled display file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "oled.h"
#include "font.h"

extern I2C_HandleTypeDef hi2c1;

/**
	* @brief IIC Write Command
	* @retval None
	*/
void Write_IIC_Command(unsigned char IIC_Command)
{
	uint8_t *pData;
	pData = &IIC_Command;
	HAL_I2C_Mem_Write(&hi2c1,OLED_ADDRESS,0x00,I2C_MEMADD_SIZE_8BIT,pData,1,100);
}

/**
	* @brief IIC Write Data
	* @retval None
	*/
void Write_IIC_Data(unsigned char IIC_Data)
{
	uint8_t *pData;
	pData = &IIC_Data;
	HAL_I2C_Mem_Write(&hi2c1,OLED_ADDRESS,0x40,I2C_MEMADD_SIZE_8BIT,pData,1,100);
}

// OLED写一个字节
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
		Write_IIC_Data(dat);
	else
		Write_IIC_Command(dat);
}

// 初始化SSD1306
void OLED_Init(void)
{
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0x00,OLED_CMD);//--256   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}

// 清屏函数，黑色
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			OLED_WR_Byte(0,OLED_DATA); 
	}
}

// 开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

// 关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

// OLED设置位置
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
} 

// 在指定位置显示一个字符
// x:0~127
// y:0~7
// mode:0,正常显示;1,反白显示				 
// size:选择字体 16/8 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode,uint8_t char_size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值		

	if(char_size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
		{
			if(mode!=1)
				OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			else
				OLED_WR_Byte(~F8X16[c*16+i],OLED_DATA);
		}
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		{
			if(mode!=1)
				OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			else
				OLED_WR_Byte(~F8X16[c*16+i+8],OLED_DATA);
		}
	}
	else 
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		{
			if(mode!=1)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
			else
				OLED_WR_Byte(~F6x8[c][i],OLED_DATA);
		}
	}
}

// 显示一个字符号串
// x:0~127
// y:0~7
// mode:0,正常显示;1,反白显示				 
// size:选择字体 16/8 
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t mode,uint8_t char_size)
{
	unsigned char j=0;
	while(chr[j]!='\0')
	{
		OLED_ShowChar(x,y,chr[j],mode,char_size);
		if(char_size == 16)
			x+=8;
		else if(char_size == 8)
			x+=6;
		j++;
	}
}

// m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

// 显示多位数字
// x,y :起点坐标	 
// len :数字的位数
// size:字体大小
// mode:0,正常显示;1,反白显示
// num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t mode,uint8_t char_size)
{
	uint8_t t,temp;
	uint8_t enshow=0;		
	uint8_t size2=0;
	
	if(char_size == 8)
		size2 = 6;
	else
		size2 = char_size/2;
	
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				//OLED_ShowChar(x+size2*t,y,' ',mode,char_size);
				OLED_ShowChar(x+size2*t,y,'0',mode,char_size);
				continue;
			}
			else 
				enshow=1; 
		}
	 	OLED_ShowChar(x+size2*t,y,temp+'0',mode,char_size); 
	}
}

// 显示多位数字
// x,y :起点坐标	 
// len :数字的位数
// size:字体大小
// mode:0,正常显示;1,反白显示
// num :小数
void OLED_ShowFloat(uint8_t x,uint8_t y,double num,uint8_t integer,uint8_t decimal,uint8_t mode,uint8_t char_size)
{
	uint8_t t,temp;
	uint8_t enshow=0;		
	uint8_t size2=0;
	
	if(char_size == 8)
		size2 = 6;
	else
		size2 = char_size/2;
	
	uint16_t zheng;
	uint16_t xiao;
	
	zheng = (uint16_t)num;
	xiao =  (uint32_t)(num*oled_pow(10,decimal)) - zheng*oled_pow(10,decimal);
	
	for(t=0;t<integer;t++)
	{
		temp=(zheng/oled_pow(10,integer-t-1))%10;
		if(enshow==0&&t<(integer-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+size2*t,y,' ',mode,char_size);
				//OLED_ShowChar(x+size2*t,y,'0',mode,char_size);
				continue;
			}
			else 
				enshow=1; 
		}
		OLED_ShowChar(x+size2*t,y,temp+'0',mode,char_size); 
	}
	
	OLED_ShowChar(x+size2*integer,y,'.',mode,char_size); 
	
	x=x+size2*(integer+1);
	for(t=0;t<decimal;t++)
	{
		temp=(xiao/oled_pow(10,decimal-t-1))%10;
		if(enshow==0&&t<(decimal-1))
		{
			if(temp==0)
			{
				//OLED_ShowChar(x+size2*t,y,' ',mode,char_size);
				OLED_ShowChar(x+size2*t,y,'0',mode,char_size);
				continue;
			}
			else 
				enshow=1; 
		}
		OLED_ShowChar(x+size2*t,y,temp+'0',mode,char_size); 
	}
}

// 显示一个点
// x:0~127
// y:0~63
void OLED_ShowPoint(uint8_t x,uint8_t y)
{
	OLED_Set_Pos(x,y/8);
	OLED_WR_Byte(0x01<<(y%8),OLED_DATA);
}

// 显示一个波形
// x:0~127
// y:0~7
void OLED_ShowWave(uint8_t x,uint8_t y)
{
	unsigned char i=0;
	
	OLED_Set_Pos(x,y);	
	for(i=0;i<80;i++)
		OLED_WR_Byte(Wave80x24[i],OLED_DATA);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<80;i++)
		OLED_WR_Byte(Wave80x24[80+i],OLED_DATA);
	OLED_Set_Pos(x,y+2);
	for(i=0;i<80;i++)
		OLED_WR_Byte(Wave80x24[160+i],OLED_DATA);
}

// 清除一个波形
// x:0~127
// y:0~7
void OLED_ShowWaveNone(uint8_t x,uint8_t y)
{
	unsigned char i=0;
	
	OLED_Set_Pos(x,y);	
	for(i=0;i<80;i++)
		OLED_WR_Byte(Wave80x24[240],OLED_DATA);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<80;i++)
		OLED_WR_Byte(Wave80x24[240],OLED_DATA);
	OLED_Set_Pos(x,y+2);
	for(i=0;i<80;i++)
		OLED_WR_Byte(Wave80x24[240],OLED_DATA);
}
