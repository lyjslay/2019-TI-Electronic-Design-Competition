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

#ifndef _SPWM_H
#define _SPWM_H

#include "main.h"
//#define SINE_LENGTH	(50000/50)

//void SINE_50Hz_Init(void);

//uint16_t SINE_50Hz(uint16_t position);
uint16_t A_SINE_Hz(double A, double position, uint16_t frequence);
#endif
