#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f4xx_hal.h"

typedef struct PID_TypeDef
{
	double expectedvalue;
	double actualvalue;
	double incrementvalue;
	double err;
	double err_last;
	double err_prev;
	double Kp, Ki, Kd;
}PID_TypeDef;

typedef struct PR_TypeDef
{
	double expectedvalue;
	double actualvalue;
	double incrementvalue;
	double err;
	double err_last;
	double err_prev;
	double Kp, Kr;
}PR_TypeDef;

void PID_Init(void);
void PID_Calculate(uint32_t present_value);

void PR_Init(void);
void PR_Calculate(uint32_t present_value);

#endif

