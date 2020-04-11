/**
  ******************************************************************************
  * File Name          : control.h
  * Description        : This file provides code for the configuration
  *                      of the control header file.
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

#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f4xx_hal.h"

#define HPR_1_is_OPEN
#define HPID_1_is_OPEN

#define adc1_channel		4
#define adc1_per_num		8
#define adc1_data_num		(adc1_channel * adc1_per_num)

#define adc2_channel		2
#define adc2_per_num		5
#define adc2_data_num		(adc2_channel * adc2_per_num)

typedef struct PowerFactor_TypeDef
{
	float PF;
	float U, I, Urms, Irms;
	float ActiveP, ReactiveP, ApparentP;
	float Ums_SUM, Ims_SUM;
	float ActiveP_SUM;
}PowerFactor_TypeDef;

typedef struct PID_TypeDef
{
	float Ts;
	float co1, co2;
	float Kp, Ki, Kd;
	float Mi, Md;
	
	float u, u1;
	float yk, yi, yi1;

	float out;
}PID_TypeDef;

typedef struct PR_TypeDef
{
	//Init Para:
	float Wo, Wc;
	float Ts;
	float co1, co2, co3;
	float Kr;
	
	float Kp;
	float u, u1;
	float yk, yr, yr1, yr2, yr_next;
	float out, out1;
	float slope;
}PR_TypeDef;

typedef struct PLL_TypeDef
{
	PID_TypeDef hpid;
	float Ts;
	float u, ya;
	float COMP;
}PLL_TypeDef;

static float PID_Co_Cal(PID_TypeDef* hpid, uint8_t CO);
static float PR_Co_Cal(PR_TypeDef* hpr, uint8_t CO);

void PID_Init(void);
void PR_Init(void);
void PLL_Init(void);
void ADC_Init(void);

float PID_Calculate(float PID_Input, PID_TypeDef* hpid);
int16_t PR_Calculate(int16_t PR_Input, PR_TypeDef* hpr);
float PLL_Omega_Cal(int16_t PLL_input, PLL_TypeDef* hpll);
float PLL_Phi_Cal(PLL_TypeDef* hpll);

void ADC1_Average(uint16_t *ad1_values);
void ADC2_Average(uint16_t *ad2_values);

#endif

