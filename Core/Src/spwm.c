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
	
#include "spwm.h"

#include "arm_const_structs.h"

//uint16_t sine_table[SINE_LENGTH] = {0};

//void SINE_50Hz_Init(void)
//{
//	uint16_t position = 0;
//	for(position = 0; position < SINE_LENGTH; position ++)
//	{
//		sine_table[position] = 840*arm_sin_f32(2*PI*position/SINE_LENGTH) + 840;
//	}
//}

//uint16_t SINE_50Hz(uint16_t position)
//{
//	return sine_table[position];
//}

uint16_t A_SINE_Hz(double A, double omega, uint16_t frequence)
{
	//position*frequence/50000
	return 0.9*(PERIOD_MAX_TIM1/2)*arm_sin_f32(omega) + (PERIOD_MAX_TIM1/2);
}
