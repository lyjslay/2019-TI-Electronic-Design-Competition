/**
  ******************************************************************************
  * @file           : control.c
  * @brief          : control function file.
	*										pid, pr, ...
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

#include "control.h"
#include "main.h"
#include "stm32f4xx_hal.h"

#include "stdio.h"
#include "arm_const_structs.h"

PID_TypeDef hpid1, hpid2;
PR_TypeDef  hpr1, hpr3x1;
PLL_TypeDef hpll;

uint16_t adc1_values[adc1_data_num];
int16_t adc1_filter[adc1_channel];
float adc1_average[adc1_channel];

uint16_t adc2_values[adc2_data_num];
int16_t adc2_filter[adc2_channel];
float adc2_average[adc2_channel];

void ADC_Init(void)
{
	static uint16_t adc_i = 0;
	for(adc_i = 0; adc_i < adc1_channel; adc_i ++)
	{
		adc1_average[adc_i] = 2048;
	}
	for(adc_i = 0; adc_i < adc2_channel; adc_i ++)
	{
		adc2_average[adc_i] = 2048;
	}
}

void ADC1_Average(uint16_t *ad_values)
{
	static uint16_t adc1_i = 0;
	static uint16_t adc1_j = 0;
	static uint16_t adc1_k = 0;
	uint32_t adc1_sum[adc1_channel] = {0};
	
	for(adc1_i = 0,adc1_k = 0; adc1_i < adc1_per_num; adc1_i ++)						//保存DMA中数据到数组内
	{
		for(adc1_j = 0; adc1_j < adc1_channel; adc1_j ++)
		{
			adc1_sum[adc1_j] += ad_values[adc1_k ++];
		}
	}	
	for(adc1_k  = 0; adc1_k < adc1_channel; adc1_k ++)
	{
		adc1_filter[adc1_k] = adc1_sum[adc1_k] / adc1_per_num; 
	}	
}

void ADC2_Average(uint16_t *ad2_values)
{
	static uint16_t adc2_i = 0;
	static uint16_t adc2_j = 0;
	static uint16_t adc2_k = 0;

	int16_t adc2_sum[adc2_channel] = {0};

	
	for(adc2_i = 0,adc2_k = 0; adc2_i < adc2_per_num; adc2_i ++)						//保存DMA中数据到数组内
	{
		for(adc2_j = 0; adc2_j < adc2_channel; adc2_j ++)
		{
			adc2_sum[adc2_j] += ad2_values[adc2_k ++];
		}
	}

	for(adc2_k  = 0; adc2_k < adc2_channel; adc2_k ++)
	{
		adc2_filter[adc2_k] = adc2_sum[adc2_k] / adc2_per_num;
	}
}
/* PID FUNCTION START --------------------------------------------------------------*/
static float PID_Co_Cal(PID_TypeDef* hpid, uint8_t CO)
{
	switch (CO)
	{
		case 1:
			return hpid->Ki*hpid->Ts/(2+hpid->Mi*hpid->Ts);
		case 2:
			return (2-hpid->Mi*hpid->Ts)/(2+hpid->Mi*hpid->Ts);
		default:
			return 0;
	}
}
void PID_Init(void)
{
	// PID 1
	hpid1.out = 0;
	hpid1.u1 = 0;
	hpid1.yi = 0;
	hpid1.yi1 = 0;
	
	hpid1.Kp = 0.1;
	hpid1.Ki = 50;
	hpid1.Mi = 10;
	hpid1.Kd = 0.0;
	hpid1.Ts = 0.0005;
	
	hpid1.co1 = PID_Co_Cal(&hpid1, 1);
	hpid1.co2 = PID_Co_Cal(&hpid1, 2);

	//PID 2
	hpid2.out = 0;
	hpid2.u1 = 0;
	hpid2.yi = 0;
	hpid2.yi1 = 0;
	
	hpid2.Kp = 0.1;
	hpid2.Ki = 50;
	hpid2.Mi = 10;
	hpid2.Kd = 0.0;
	hpid2.Ts = 0.0005;
	
	hpid2.co1 = PID_Co_Cal(&hpid2, 1);
	hpid2.co2 = PID_Co_Cal(&hpid2, 2);
}
float PID_Calculate(float PID_Input, PID_TypeDef* hpid)
{
	hpid->u = PID_Input;
	hpid->yk = hpid->Kp * hpid->u;
	hpid->yi = hpid->co1 *(hpid->u+hpid->u1) + hpid->co2*hpid->yi1;
	if(hpid->yi > 1500)
		hpid->yi = 1500;
	if(hpid->yi < -1500)
		hpid->yi = -1500;
	hpid->yi1 = hpid->yi;
	hpid->u1 = hpid->u;
	
	hpid->out = hpid->yk + hpid->yi;
	return hpid->out;
}

/* PID FUNCTION END --------------------------------------------------------------*/

/* PR FUNCTION START --------------------------------------------------------------*/
static float PR_Co_Cal(PR_TypeDef* hpr, uint8_t CO)
{
	switch (CO)
	{
		case 1:
			return 4*hpr->Kr*hpr->Ts*hpr->Wc / (hpr->Ts*hpr->Ts*hpr->Wo*hpr->Wo + 4*hpr->Wc*hpr->Ts + 4);
		case 2:
			return (2*hpr->Ts*hpr->Ts*hpr->Wo*hpr->Wo-8) / (hpr->Ts*hpr->Ts*hpr->Wo*hpr->Wo + 4*hpr->Wc*hpr->Ts + 4);
		case 3:
			return (hpr->Ts*hpr->Ts*hpr->Wo*hpr->Wo - 4*hpr->Wc*hpr->Ts + 4) / (hpr->Ts*hpr->Ts*hpr->Wo*hpr->Wo + 4*hpr->Wc*hpr->Ts + 4);
		default:
			return 0;
	}
}

void PR_Init(void)
{
#ifdef HPR_1_is_OPEN
	hpr1.out = 0;
	hpr1.u1 = 0;
	hpr1.yr = 0;
	hpr1.yr1 = 0;
	hpr1.yr2 = 0;
		
	hpr1.Kp = 0.7;
	hpr1.Kr = 48.0;
	hpr1.Ts = 0.0005;
	hpr1.Wo = 50*2*PI;
	hpr1.Wc = (float)0.0105 * hpr1.Wo;
	
	
	hpr1.co1 = PR_Co_Cal(&hpr1, 1);
	hpr1.co2 = PR_Co_Cal(&hpr1, 2);
	hpr1.co3 = PR_Co_Cal(&hpr1, 3);
#endif
  hpr3x1.out = 0;
	hpr3x1.u1 = 0;
	hpr3x1.yr = 0;
	hpr3x1.yr1 = 0;
	hpr3x1.yr2 = 0;
		
	hpr3x1.Kp = 0.0;
	hpr3x1.Kr = 18.5;
	hpr3x1.Ts = 0.0005;
	hpr3x1.Wo = 150*2*PI;
	hpr3x1.Wc = (float)0.013 * hpr3x1.Wo;
	
	
	hpr3x1.co1 = PR_Co_Cal(&hpr3x1, 1);
	hpr3x1.co2 = PR_Co_Cal(&hpr3x1, 2);
	hpr3x1.co3 = PR_Co_Cal(&hpr3x1, 3);
}

int16_t PR_Calculate(int16_t PR_Input, PR_TypeDef* hpr)
{
	hpr->u = PR_Input;
	//hpr->yk = (hpr->Kp * hpr->u);
	hpr->yr = (hpr->co1*(hpr->u-hpr->u1) -  hpr->co2*hpr->yr1 - hpr->co3*hpr->yr2);
	if(hpr->yr > (PERIOD_MAX_TIM1))
		hpr->yr = (PERIOD_MAX_TIM1);
	if(hpr->yr < -(PERIOD_MAX_TIM1))
		hpr->yr = -(PERIOD_MAX_TIM1);
	hpr->yr2 = hpr->yr1;
	hpr->yr1 = hpr->yr;
	hpr->u1 = hpr->u;
	
	//hpr->out = hpr->yr;
	//hpr->out = hpr->yk + hpr->yr;
	
	hpr->yr_next = (hpr->co1*(hpr->u-hpr->u1) -  hpr->co2*hpr->yr1 - hpr->co3*hpr->yr2);
	hpr->slope = (hpr->yr_next - hpr->yr) / 5 ;
	//return hpr->out;
	
	return hpr->yr;
}
/* PR FUNCTION END --------------------------------------------------------------*/

/* PLL FUNCTION START --------------------------------------------------------------*/
void PLL_Init(void)
{
	hpll.hpid.out = 0;
	hpll.hpid.u1 = 0;
	hpll.hpid.yi = 0;
	hpll.hpid.yi1 = 314;
	hpll.hpid.Kp = 20;//20
	hpll.hpid.Ki = 17000;//18000
	// Ts of PLL_Omega_Cal()
	hpll.hpid.Ts = 0.0005;
	
	hpll.hpid.co1 = PID_Co_Cal(&hpll.hpid, 1);
	hpll.hpid.co2 = PID_Co_Cal(&hpll.hpid, 2);
	// Ts of PLL_Phi_Cal()
	hpll.Ts = 0.0001;
}
float PLL_Omega_Cal(int16_t PLL_input, PLL_TypeDef* hpll)
{
	hpll->u = PLL_input / 800.0;
	hpll->hpid.u = (float)(arm_cos_f32(hpll->ya) * hpll->u) - (float)0.5*arm_sin_f32(2*hpll->ya);
	//PID
	hpll->hpid.yk = hpll->hpid.Kp * hpll->hpid.u;
	hpll->hpid.yi = hpll->hpid.co1 *(hpll->hpid.u + hpll->hpid.u1) + hpll->hpid.co2 * hpll->hpid.yi1;
	if(hpll->hpid.yi > 500)
		hpll->hpid.yi = 500;
	if(hpll->hpid.yi < 220)
		hpll->hpid.yi = 220;
	hpll->hpid.yi1 = hpll->hpid.yi;
	hpll->hpid.out = hpll->hpid.yk + hpll->hpid.yi;
	hpll->COMP = hpll->hpid.Ts*hpll->hpid.out;
	return hpll->hpid.out;
}
float PLL_Phi_Cal(PLL_TypeDef* hpll)
{
	hpll->ya = hpll->Ts * hpll->hpid.out + hpll->ya; 
	if(hpll->ya>=2*PI)
		hpll->ya -= 2*PI;
	return hpll->ya;
}
/* PLL FUNCTION END --------------------------------------------------------------*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
