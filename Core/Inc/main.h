/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define K1_Pin GPIO_PIN_3
#define K1_GPIO_Port GPIOE
#define K1_EXTI_IRQn EXTI3_IRQn
#define K0_Pin GPIO_PIN_4
#define K0_GPIO_Port GPIOE
#define K0_EXTI_IRQn EXTI4_IRQn
#define DRV_Pin GPIO_PIN_5
#define DRV_GPIO_Port GPIOE
#define KUP_Pin GPIO_PIN_0
#define KUP_GPIO_Port GPIOA
#define KUP_EXTI_IRQn EXTI0_IRQn
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOA
#define FCS_Pin GPIO_PIN_0
#define FCS_GPIO_Port GPIOB
#define ENABLE1_Pin GPIO_PIN_7
#define ENABLE1_GPIO_Port GPIOE
#define ENABLE2_Pin GPIO_PIN_15
#define ENABLE2_GPIO_Port GPIOE
#define ENABLE3_Pin GPIO_PIN_10
#define ENABLE3_GPIO_Port GPIOB
#define ENABLE4_Pin GPIO_PIN_13
#define ENABLE4_GPIO_Port GPIOB
#define PB14_GND_Pin GPIO_PIN_14
#define PB14_GND_GPIO_Port GPIOB
#define S4_Pin GPIO_PIN_15
#define S4_GPIO_Port GPIOB
#define S4_EXTI_IRQn EXTI15_10_IRQn
#define S3_Pin GPIO_PIN_8
#define S3_GPIO_Port GPIOD
#define S3_EXTI_IRQn EXTI9_5_IRQn
#define S2_Pin GPIO_PIN_9
#define S2_GPIO_Port GPIOD
#define S2_EXTI_IRQn EXTI9_5_IRQn
#define S1_Pin GPIO_PIN_10
#define S1_GPIO_Port GPIOD
#define S1_EXTI_IRQn EXTI15_10_IRQn
#define ENABLE2D11_Pin GPIO_PIN_11
#define ENABLE2D11_GPIO_Port GPIOD
#define DISABLE2_Pin GPIO_PIN_12
#define DISABLE2_GPIO_Port GPIOD
#define PD14_GND_Pin GPIO_PIN_14
#define PD14_GND_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */
#define PERIOD_MAX_TIM1 (2200-1)
#define PERIOD_MAX_TIM8 PERIOD_MAX_TIM1
#define AC_VOLTAGE_1Vrms (830.0/24)

#define AC_CURRENT_CH1 				adc2_filter[0]
#define AC_CURRENT_CH1_AVG 		adc2_average[0]

#define AC_VOLTAGE_CH1 				adc1_filter[0]
#define AC_VOLTAGE_CH1_AVG 		adc1_average[0]

#define DC_CURRENT_CH1				adc2_filter[1]
#define DC_CURRENT_CH1_AVG		adc2_average[1]

#define DC_VOLTAGE_CH1 				adc1_filter[1]
#define DC_VOLTAGE_CH1_AVG 		adc1_average[1]
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
