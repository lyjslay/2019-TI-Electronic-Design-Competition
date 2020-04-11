/**
  ******************************************************************************
  * File Name          : OLED.h
  * Description        : This file provides code for the configuration
  *                      of the OLED header file.
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

#ifndef __OLED_H
#define __OLED_H	

#include "stm32f4xx_hal.h"

#define X_WIDTH 	128
#define Y_WIDTH 	64	 

#define OLED_ADDRESS	0x78		//通过调整0R电阻,屏可以选择0x78  0x7A两个地址   默认为0x78
#define OLED_CMD  0						//写命令
#define OLED_DATA 1						//写数据

void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void OLED_WR_Byte(unsigned dat,unsigned cmd);

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(unsigned char x, unsigned char y) ;
void OLED_ShowPoint(uint8_t x,uint8_t y);
void OLED_ShowWave(uint8_t x,uint8_t y);
void OLED_ShowWaveNone(uint8_t x,uint8_t y);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode,uint8_t char_size);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t mode,uint8_t char_size);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t mode,uint8_t char_size);
void OLED_ShowFloat(uint8_t x,uint8_t y,double num,uint8_t integer,uint8_t decimal,uint8_t mode,uint8_t char_size);
uint32_t oled_pow(uint8_t m,uint8_t n);

#endif

