/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "control.h"
#include "oled.h"
#include "w25q16.h"
#include "spwm.h"
#include "arm_const_structs.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RELAY_ON()	HAL_GPIO_WritePin(ENABLE1_GPIO_Port, ENABLE1_Pin, GPIO_PIN_SET)
#define RELAY_OFF()	HAL_GPIO_WritePin(ENABLE1_GPIO_Port, ENABLE1_Pin, GPIO_PIN_RESET)
#define LOAD_ON()		HAL_GPIO_WritePin(ENABLE2_GPIO_Port, ENABLE2_Pin, GPIO_PIN_SET)
#define LOAD_OFF()	HAL_GPIO_WritePin(ENABLE2_GPIO_Port, ENABLE2_Pin, GPIO_PIN_RESET)

#define GAIN_ALL_200kHz (54.28*12.6/7.93)
#define GAIN_ALL_150kHz (54.28*15.8/11.01)
#define GAIN_ALL_100kHz (54.28*20/14.26)
                              
#define GAIN_ALL_60kHz	(54.28*23/19.65)
#define GAIN_ALL_40kHz	(54.28*24.2/22.95)
#define GAIN_ALL_20kHz	(54.28*25.2/25.86)
                              
#define GAIN_ALL_10kHz	(54.28*33/27.03)
#define GAIN_ALL_1kHz		(54.28)
#define Schottky_drop		200

#define Filter   0.5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//Global Flag
uint8_t FLAG_GLOBAL_MODE = 0;
FlagStatus FLAG_MODE_SET0 = RESET;
FlagStatus FLAG_MODE_SET1 = RESET;

//ADC1&2 Datas
extern uint16_t adc1_values[adc1_data_num];
extern int16_t adc1_filter[adc1_channel];
extern float adc1_average[adc1_channel];
extern uint16_t adc2_values[adc2_data_num];
extern int16_t adc2_filter[adc2_channel];
extern float adc2_average[adc2_channel];

int16_t ad1_vrm_p[adc1_channel] = {0};
int16_t ad1_vrm_n[adc1_channel] = {0};
int16_t flag_ad_vrm = 0;

int16_t adc1_vpp = 0;
int16_t adc2_vpp = 0;
int16_t vo_vpp_no_load = 0;
int16_t vo_vpp_with_load = 0;
int16_t vo_avr_no_load = 0;
int16_t vo_avr_with_load = 0;
int16_t pulse_vpp_adc = 0;

double regsitor_input = 0;
double regsitor_input_temp = 0;
double regsitor_input_adv = 1;
double regsitor_output = 1000;
double regsitor_output_adv = 0;
double gain_in = 0.0;
double gain_trans[20] = {0.0};
double gain_trans_adv[20] = {0.0};
double gain_max = 0;
double gain_max_adv = 0;
double gain_limit = 0;
double gain_limit_adv = 0;
int16_t vo_avr_1kHz = 0;
int16_t vo_vpp[20] = {0};

uint8_t flag_vavr = 0;
uint8_t flag_input = 0;
uint8_t flag_input1 = 0;
uint8_t flag_output = 0;
uint8_t flag_gain_1kHz = 0;
uint8_t flag_gain_200kHz = 0;
uint8_t flag_pulse = 0;
uint8_t flag_50Hz_gain = 0;

int16_t flag_vavr_adc = 0;

double gain_freq[11] = {0.05,0.1,0.5,1,10,20,40,60,100,150,200,};

//const uint16_t Sinebit1[100]={
//	0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,
//	0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,
//	0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,
//	0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,
//	0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,0x7FF,
//	0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,
//	0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,
//	0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,
//	0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,
//	0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,
//};
//Sine 
const uint16_t Sinebit1[100]={
	0x7ff,0x84c,0x89a,0x8e7,0x933,0x97e,0x9c7,0xa0f,0xa54,0xa98,
	0xad8,0xb16,0xb50,0xb87,0xbbb,0xbeb,0xc16,0xc3e,0xc62,0xc81,
	0xc9b,0xcb1,0xcc2,0xcce,0xcd5,0xcd8,0xcd5,0xcce,0xcc2,0xcb1,
	0xc9b,0xc81,0xc62,0xc3e,0xc16,0xbeb,0xbbb,0xb87,0xb50,0xb16,
	0xad8,0xa98,0xa54,0xa0f,0x9c7,0x97e,0x933,0x8e7,0x89a,0x84c,
	0x7ff,0x7b1,0x763,0x716,0x6ca,0x67f,0x636,0x5ee,0x5a9,0x565,
	0x525,0x4e7,0x4ad,0x476,0x442,0x412,0x3e7,0x3bf,0x39b,0x37c,
	0x362,0x34c,0x33b,0x32f,0x328,0x325,0x328,0x32f,0x33b,0x34c,
	0x362,0x37c,0x39b,0x3bf,0x3e7,0x412,0x442,0x476,0x4ad,0x4e7,
	0x525,0x565,0x5a9,0x5ee,0x636,0x67f,0x6ca,0x716,0x763,0x7b1,
};
const uint16_t Sinebit2[100]={
	0x7ff,0x806,0x80e,0x816,0x81d,0x825,0x82c,0x833,0x83a,0x841,
	0x847,0x84e,0x853,0x859,0x85e,0x863,0x867,0x86b,0x86f,0x872,
	0x875,0x877,0x878,0x87a,0x87a,0x87b,0x87a,0x87a,0x878,0x877,
	0x875,0x872,0x86f,0x86b,0x867,0x863,0x85e,0x859,0x853,0x84e,
	0x847,0x841,0x83a,0x833,0x82c,0x825,0x81d,0x816,0x80e,0x806,
	0x7ff,0x7f7,0x7ef,0x7e7,0x7e0,0x7d8,0x7d1,0x7ca,0x7c3,0x7bc,
	0x7b6,0x7af,0x7aa,0x7a4,0x79f,0x79a,0x796,0x792,0x78e,0x78b,
	0x788,0x786,0x785,0x783,0x783,0x782,0x783,0x783,0x785,0x786,
	0x788,0x78b,0x78e,0x792,0x796,0x79a,0x79f,0x7a4,0x7aa,0x7af,
	0x7b6,0x7bc,0x7c3,0x7ca,0x7d1,0x7d8,0x7e0,0x7e7,0x7ef,0x7f7,
};
uint16_t DualSinebit_Mian[2] = {0x7FF};
uint16_t DualSinebit[100];
uint32_t Idx = 0;

uint16_t flag_100Hz = 0;
uint16_t ii = 0;

double A1,A2;
double F1,F2;
double F0,A0;
double A1_adv,A2_adv;
double F1_adv,F2_adv;
double F0_adv,A0_adv;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void ADC_VRMS(void);
void BSP_Init(void);
void DAC_GAIN_Set(FlagStatus GAIN);
void PWM_SetFreqkHz(uint16_t freq);
void DAC_SetFreqHz(uint16_t freq);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_ADC2_Init();
  MX_TIM12_Init();
  MX_DAC_Init();
  MX_TIM9_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	ADC_Init();
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_values, adc1_data_num);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc2_values, adc2_data_num);
	HAL_Delay(100);
	
	OLED_Init();
	OLED_Clear(); 
	OLED_ShowString(6,0,(uint8_t*)"OLED TEST OK.",0,8);
	
	//ADC1_Average(adc1_values);
	//ADC2_Average(adc2_values);
	OLED_ShowString(6,1,(uint8_t*)"ADC OK.",0,8);
	
//	if(W25Q16 == W25QXX_ReadID())
//		OLED_ShowString(6,2,(uint8_t*)"W25Q16 OK.",0,8);

//	W25QXX_Read(read_value,W25Q16_SIZE-1000,6);	
//	temp_value = strtod((char*)read_value,NULL);
//	OLED_ShowFloat(6,3,temp_value,2,3,0,8);
	
	RELAY_ON();
	LOAD_OFF();

	HAL_Delay(100);
	OLED_Clear();
	OLED_ShowNum(0,0,0,5,0,8);	OLED_ShowNum(64,0,0,5,0,8);
	OLED_ShowNum(0,1,0,5,0,8);	OLED_ShowNum(64,1,0,5,0,8);
	
	OLED_ShowString(0,0,(uint8_t *)"Rin(kOhm)",0,8);
	OLED_ShowString(0,1,(uint8_t *)"Rout(Ohm)",0,8);
	OLED_ShowString(0,2,(uint8_t *)"Gain(db)",0,8);
	
	HAL_TIM_Base_Start(&htim6);				//DAC Sine_1kHz
	HAL_TIM_Base_Start_IT(&htim9);		//ADC VRMS
	HAL_TIM_Base_Start_IT(&htim12);		//1Hz
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//PWM
	
	DAC_GAIN_Set(SET);
	HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *)DualSinebit,100,DAC_ALIGN_12B_R);			
	HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_2,(uint32_t *)DualSinebit,100,DAC_ALIGN_12B_R);
	
	//PWM_SetFreqkHz(10);
	//DAC_SetFreqHz(100);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		switch(FLAG_GLOBAL_MODE)
		{
			case 0:  //000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
			{
				OLED_ShowFloat(64,0,regsitor_input,4,4,0,8);
				OLED_ShowFloat(64,1,regsitor_output,4,4,0,8);
				OLED_ShowFloat(64,2,gain_trans[3],4,4,0,8);
				
				OLED_ShowString(0,3,(uint8_t *)"                    ",0,8);
				OLED_ShowString(0,4,(uint8_t *)"                    ",0,8);
				OLED_ShowString(0,5,(uint8_t *)"                    ",0,8);
				OLED_ShowString(0,6,(uint8_t *)"                    ",0,8);
				OLED_ShowString(0,7,(uint8_t *)"                    ",0,8);
				break;
			}
			case 1:  //111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
			{
				//OLED_ShowFloat(64,4,gain_trans[1],4,4,0,8);
				OLED_ShowString(0,4,(uint8_t *)"         ",0,8);
				OLED_ShowString(64,4,(uint8_t *)"         ",0,8);
				OLED_ShowFloat(0,0,gain_trans[0],2,1,0,8);				OLED_ShowFloat(63,0,gain_trans[1],2,1,0,8);
				OLED_ShowFloat(0,1,gain_trans[2],2,1,0,8);				OLED_ShowFloat(63,1,gain_trans[3],2,1,0,8);
				OLED_ShowFloat(0,2,gain_trans[4],2,1,0,8);				OLED_ShowFloat(63,2,gain_trans[8],2,1,0,8);
				OLED_ShowFloat(0,3,gain_trans[10],2,1,0,8);				//OLED_ShowFloat(63,3,gain_trans[7],2,1,0,8);

				OLED_ShowString(24,0,(uint8_t *)":50   ",0,8);		OLED_ShowString(87,0,(uint8_t *)":100  ",0,8);	
				OLED_ShowString(24,1,(uint8_t *)":500  ",0,8);		OLED_ShowString(87,1,(uint8_t *)":1k   ",0,8);
				OLED_ShowString(24,2,(uint8_t *)":10k  ",0,8);		OLED_ShowString(87,2,(uint8_t *)":100k ",0,8);	
				OLED_ShowString(24,3,(uint8_t *)":200k ",0,8);		OLED_ShowString(63,3,(uint8_t *)"G(db)/F",0,8);
				
				OLED_ShowWave(11,5);	
				
				OLED_ShowFloat(87,5,F0,3,1,0,8);
				OLED_ShowString(87+30,5,(uint8_t *)"k",0,8);
				
				break;
			}
			case 2:    //222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
			{
				OLED_ShowString(0,4,(uint8_t *)"         ",0,8);
				OLED_ShowString(64,4,(uint8_t *)"         ",0,8);
				OLED_ShowString(0,3,(uint8_t *)"                    ",0,8);
				OLED_ShowString(0,0,(uint8_t *)"Rin(kOhm)",0,8);
				OLED_ShowString(0,1,(uint8_t *)"Rout(Ohm)",0,8);
				OLED_ShowString(0,2,(uint8_t *)"Gain(db)",0,8);
				//OLED_ShowString(0,3,(uint8_t *)"                    ",0,8);
				
				OLED_ShowFloat(64,0,regsitor_input_adv,4,4,0,8);
				//OLED_ShowFloat(64,1,regsitor_input_adv,4,4,0,8);
				OLED_ShowFloat(64,1,regsitor_output_adv,4,4,0,8);
				OLED_ShowFloat(64,2,gain_trans_adv[3],4,4,0,8);
				
				//OLED_ShowFloat(0,4,gain_trans_adv[3],2,1,0,8);						OLED_ShowFloat(63,4,gain_trans_adv[4],2,1,0,8);
				//OLED_ShowFloat(0,5,gain_trans_adv[5],2,1,0,8);						OLED_ShowFloat(63,5,gain_trans_adv[6],2,1,0,8);
				//OLED_ShowFloat(0,6,gain_trans_adv[7],2,1,0,8);						OLED_ShowFloat(63,6,gain_trans_adv[8],2,1,0,8);
				//OLED_ShowFloat(0,7,gain_trans_adv[9],2,1,0,8);						OLED_ShowFloat(63,7,gain_trans_adv[10],2,1,0,8);
				{ //INPUT R FLAG CALCULATION
					
				if(regsitor_input_adv <= 0.2)
					flag_input = 1;
				else if(regsitor_input_adv>0.2 && regsitor_input_adv<=0.7*regsitor_input)
					flag_input = 2;
				else if(regsitor_input_adv>0.7*regsitor_input && regsitor_input_adv<=2*regsitor_input)
					flag_input = 0;
				else if(regsitor_input_adv>2*regsitor_input && regsitor_input_adv<=100)
					flag_input = 3;
				else if(regsitor_input_adv>100)
					flag_input = 4;
				
				if(regsitor_input_adv <= 9.6)
					flag_input1 = 0;
				if(regsitor_input_adv > 10.5)
					flag_input1 = 1;
				}
				{ //OUTPUT R FLAG CALCULATION
				if(regsitor_output_adv <= 200)
					flag_output = 1;
				else
					flag_output = 0;
			  }
				
				
				
				
				// Vout AVR FLAG CALCULATION
				if(HAL_GPIO_ReadPin(ENABLE2_GPIO_Port,ENABLE2_Pin) == RESET)
				{
					if(flag_vavr_adc >= 3420)
						flag_vavr = 1;
					if(flag_vavr_adc>=2800 && flag_vavr_adc<3410 )
						flag_vavr = 2;
					if(flag_vavr_adc>=1500 && flag_vavr_adc<2800)
						flag_vavr = 3;
					if(flag_vavr_adc>=100 && flag_vavr_adc<1500)
						flag_vavr = 0;
					if(flag_vavr_adc>=20 && flag_vavr_adc<100)
						flag_vavr = 4;
					if(flag_vavr_adc<20)
						flag_vavr = 5;						
				}
					
				//OLED_ShowNum(0,3,ad1_vrm_p[2],5,0,8);
				//OLED_ShowNum(0,4,adc1_filter[3],5,0,8);		
				//OLED_ShowNum(64,3,gain_trans_adv[3],5,0,8);
				
				//OLED_ShowFloat(87,3,F0_adv,3,1,0,8);
				OLED_ShowNum(0,3,flag_input,1,0,8);
				OLED_ShowNum(20,3,flag_output,1,0,8);
				OLED_ShowNum(40,3,flag_vavr,1,0,8);
				//OLED_ShowNum(0,4,flag_vavr,1,0,8);
				//OLED_ShowNum(20,4,flag_gain_1kHz,1,0,8);
				//OLED_ShowNum(40,4,flag_gain_200kHz,1,0,8);
				//OLED_ShowNum(60,4,flag_pulse,1,0,8);

				
				
				OLED_ShowString(0,4,(uint8_t *)"         ",0,8);
				
				if((flag_input ==3) && ((flag_vavr ==1) ||(flag_vavr ==2) )&& (flag_input1 == 1))
					OLED_ShowString(0,4,(uint8_t *)"R1 open  ",0,8);
				if((flag_input == 2) || (flag_input == 1))
					if((flag_output == 1) && (flag_vavr == 2))
					OLED_ShowString(0,4,(uint8_t *)"R1 short ",0,8);
				if((flag_vavr == 0))
					OLED_ShowString(0,4,(uint8_t *)"R2 open  ",0,8);
				if((flag_input == 2) || (flag_input == 1))
					if((flag_output == 0) && ((flag_vavr ==1)||(flag_vavr ==2)))
					OLED_ShowString(0,4,(uint8_t *)"R2 short ",0,8);
				if((flag_vavr ==4))
					OLED_ShowString(0,4,(uint8_t *)"R3 open  ",0,8);
				if((flag_input ==0) && (flag_output == 1) && ((flag_vavr ==1) ||(flag_vavr ==2) ))
					OLED_ShowString(0,4,(uint8_t *)"R3 short ",0,8);
				if((flag_input ==3) && ((flag_vavr ==1) ||(flag_vavr ==2) ) && (flag_input1 == 0))
					OLED_ShowString(0,4,(uint8_t *)"R4 open  ",0,8);
				if((flag_vavr ==5))
					OLED_ShowString(0,4,(uint8_t *)"R4 short ",0,8);
				
				OLED_ShowString(64,4,(uint8_t *)"         ",0,8);
				OLED_ShowString(64,5,(uint8_t *)"          ",0,8);
				if((flag_vavr == 3))
				{
					if((flag_pulse == 1) && (flag_gain_1kHz == 1))
						OLED_ShowString(64,5,(uint8_t *)"C1 Double",0,8);
					if((flag_50Hz_gain == 1) && (flag_gain_1kHz == 1))
						OLED_ShowString(64,5,(uint8_t *)"C2 Double",0,8);
					if((flag_gain_200kHz == 1) && (flag_gain_1kHz == 1))
						OLED_ShowString(64,5,(uint8_t *)"C3 Double",0,8);
					
					if((flag_gain_200kHz == 3) && (flag_gain_1kHz == 1))
						OLED_ShowString(64,4,(uint8_t *)"C3 Open  ",0,8);
					if((flag_input == 4) && (flag_gain_1kHz == 0))
						OLED_ShowString(64,4,(uint8_t *)"C1 Open  ",0,8);
					
					if((flag_input == 3) && (flag_gain_1kHz == 0))
						OLED_ShowString(64,4,(uint8_t *)"C2 Open  ",0,8);
				}
				
				
				
				
				//OLED_ShowFloat(0,6,gain_trans_adv[10],3,1,0,8);
				
				break;
			}
			default:
				break;
		}
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		HAL_Delay(5);
	}
	//eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == KUP_Pin)							// KUP接下拉，弹起后电位产生下降沿，检测下降沿
  {
		HAL_Delay(20);
    if(HAL_GPIO_ReadPin(KUP_GPIO_Port,KUP_Pin)==GPIO_PIN_RESET)
		{
			FLAG_GLOBAL_MODE = 0;
		}
		__HAL_GPIO_EXTI_CLEAR_IT(KUP_Pin);
  }
	
  if(GPIO_Pin == K0_Pin)
  {
		HAL_Delay(20);
    if(HAL_GPIO_ReadPin(K0_GPIO_Port,K0_Pin)==GPIO_PIN_RESET)
		{
			FLAG_GLOBAL_MODE = 1;
		}
		__HAL_GPIO_EXTI_CLEAR_IT(K0_Pin);
  }
	
	if(GPIO_Pin == K1_Pin)
	{
		HAL_Delay(20);
    if(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin)==GPIO_PIN_RESET)
		{
			FLAG_GLOBAL_MODE = 2;
		}
		__HAL_GPIO_EXTI_CLEAR_IT(K1_Pin);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* TIM1 1MHz PWM -------------------------------------------------------------- -------------------------------------------------------------- TIM1 1MHz PWM*/
	if(htim->Instance == htim1.Instance)
	{
		
	}
	
	/* TIM9 3.89kHz -------------------------------------------------------------- ------------------------------------------------------------------ TIM9 2kHz */
	if(htim->Instance == htim9.Instance)
	{
		ADC_VRMS();
		switch(flag_100Hz)
		{
			case 1:
			{
				adc1_vpp = ad1_vrm_p[0]-ad1_vrm_n[0];
				adc2_vpp = ad1_vrm_p[1]-ad1_vrm_n[1];
				regsitor_input_temp = 0.7*regsitor_input_temp + 0.3*(double)adc2_vpp*10.0/(double)(adc1_vpp-adc2_vpp);
				regsitor_input_temp = regsitor_input_temp*10.0/11.2;
				break;
			}
			default:
			{
				adc1_vpp = ad1_vrm_p[0]-ad1_vrm_n[0];
				adc2_vpp = ad1_vrm_p[1]-ad1_vrm_n[1];
				regsitor_input_temp = 0.7*regsitor_input_temp + 0.3*(double)adc2_vpp*10.0/(double)(adc1_vpp-adc2_vpp);
				regsitor_input_temp = regsitor_input_temp*10.0/11.2;
				break;
			}
		}
		//DualSinebit_Mian[0] = (uint32_t)((ad1_vrm_p[0] << 16) + ad1_vrm_p[0]);
		//DualSinebit_Mian[1] = (uint32_t)((ad1_vrm_p[0] << 16) + ad1_vrm_p[0]);
	}
	
	/* TIM12 100Hz -------------------------------------------------------------- ----------------------------------------------------------------- TIM12 10Hz */
	if(htim->Instance == htim12.Instance)
	{
		switch(FLAG_GLOBAL_MODE)
		{
			case 0:			//基础部分1-3  0000000000000000000000000000000000000000000000000000000000000000000000000000
			{
				FLAG_MODE_SET0 = SET;
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
				RELAY_OFF();
				DAC_SetFreqHz(1000);
				switch(flag_100Hz)
				{
					case 0:
					{
						DAC_GAIN_Set(SET);
						break;
					}
					case 1:		//输入电阻 1V
					{
						//adc1_vpp = ad1_vrm_p[0]-ad1_vrm_n[0];
						//adc2_vpp = ad1_vrm_p[1]-ad1_vrm_n[1];
						//regsitor_input_temp = (double)adc2_vpp*10.0/(double)(adc1_vpp-adc2_vpp);
						//if(fabs(regsitor_input - (double)adc2_vpp*10.0/(double)(adc1_vpp-adc2_vpp)) > 30)
						//	regsitor_input = 0.5*(double)regsitor_input + 0.5*regsitor_input_temp;
						//else
						//	regsitor_input = 0.9*(double)regsitor_input + 0.1*regsitor_input_temp;
						//regsitor_input = (double)regsitor_input*100/103;
						
						regsitor_input = Filter*regsitor_input+ (1-Filter)*regsitor_input_temp;
						vo_vpp_no_load = (ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1;	
						vo_avr_no_load = adc1_filter[3];
						
						LOAD_ON();
						//DAC_GAIN_Set(RESET); //0.1V 为case2准备
						break;
					}
					case 2:		//输出电阻 0.1V
					{
						vo_vpp_with_load = (ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1;
						vo_avr_with_load = adc1_filter[3];
						regsitor_output = (double)(vo_avr_no_load-vo_avr_with_load) * 2000 / vo_avr_with_load;
						
						LOAD_OFF();
						DAC_GAIN_Set(SET); 
						break;
					}
					case 3:		//增益 1V 1kHz
					{
						vo_vpp[3] = Filter*vo_vpp[3] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_in = regsitor_input/(regsitor_input+10);
						gain_trans[3] = 20*log10f(vo_vpp[3]/GAIN_ALL_1kHz/gain_in); //    /gain_in !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						break;
					}
					default:
						break;
				}
				break;
			}
			case 1:			//基础4 伯德图  1111111111111111111111111111111111111111111111111111111111111111111111111111111111111
			{
				FLAG_MODE_SET1 = SET;
				LOAD_OFF();
				DAC_GAIN_Set(SET); 
				
//				RELAY_ON();	
//				PWM_SetFreqkHz(10);
//				switch(flag_10Hz)
//				{
//					case 0:
//					{
//						vo_vpp[0] = 0.5*vo_vpp[0] + 0.5*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
//						gain_trans[0] = 10.0*vo_vpp[0]/GAIN_ALL_1kHz;//gain_in;
//						break;
//					}
//					default:
//						break;
//				}
				switch(flag_100Hz)
				{
					case 0:
					{
						__HAL_TIM_SET_COUNTER(&htim1,0);
						__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
						pulse_vpp_adc = adc1_filter[1];
						
						RELAY_OFF();				//DAC使用
						DAC_SetFreqHz(50);
						break;
					}
					case 3:
					{
						//vo_vpp[0] = Filter*vo_vpp[0] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + 0)<<1);
						vo_vpp[0] = 0.75*vo_vpp[0] + (0.25)*((ad1_vrm_p[2] - vo_avr_1kHz + 0)<<1);
						gain_trans[0] = 20*log10f(vo_vpp[0]/GAIN_ALL_1kHz/gain_in);//gain_in;
						DAC_SetFreqHz(100);
						break;
					}
					case 4:
					{
						vo_vpp[1] = Filter*vo_vpp[1] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[1] = 20*log10f(vo_vpp[1]/GAIN_ALL_1kHz/gain_in);//gain_in;
						DAC_SetFreqHz(500);
						break;
					}
					case 5:
					{
						vo_vpp[2] = Filter*vo_vpp[2] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[2] = 20*log10f(vo_vpp[2]/GAIN_ALL_1kHz/gain_in);//gain_in;
						DAC_SetFreqHz(1000);
						break;
					}
					case 6:
					{
						vo_vpp[3] = Filter*vo_vpp[3] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[3] = 20*log10f(vo_vpp[3]/GAIN_ALL_1kHz/gain_in);//gain_in;
						vo_avr_1kHz = adc1_filter[3];

						//PWM open
						RELAY_ON();
						PWM_SetFreqkHz(10);
						break;			
					}
					case 7:
					{
						vo_vpp[4] = Filter*vo_vpp[4] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[4] = 20*log10f(vo_vpp[4]/GAIN_ALL_10kHz/gain_in);//gain_in;
						PWM_SetFreqkHz(20);
						break;
					}
					case 8:
					{
						vo_vpp[5] = Filter*vo_vpp[5] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[5] = 20*log10f(vo_vpp[5]/GAIN_ALL_20kHz/gain_in);//gain_in;
						PWM_SetFreqkHz(40);
						break;
					}
					case 9:
					{
						vo_vpp[6] = Filter*vo_vpp[6] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[6] = 20*log10f(vo_vpp[6]/GAIN_ALL_40kHz/gain_in);//gain_in;
						PWM_SetFreqkHz(60);
						break;
					}
					case 10:
					{
						vo_vpp[7] = Filter*vo_vpp[7] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[7] = 20*log10f(vo_vpp[7]/GAIN_ALL_60kHz/gain_in);//gain_in;
						PWM_SetFreqkHz(100);
						break;
					}
					case 11:
					{
						vo_vpp[8] = Filter*vo_vpp[8] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[8] = 20*log10f(vo_vpp[8]/GAIN_ALL_100kHz/gain_in);//gain_in;
						PWM_SetFreqkHz(150);
						break;
					}
					case 12:
					{
						vo_vpp[9] = Filter*vo_vpp[9] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[9] = 20*log10f(vo_vpp[9]/GAIN_ALL_150kHz/gain_in);//gain_in;
						PWM_SetFreqkHz(200);
						break;
					}
					case 13:
					{
						vo_vpp[10] = 0.7*vo_vpp[10] + 0.3*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
						gain_trans[10] = 20*log10f(vo_vpp[10]/GAIN_ALL_150kHz/gain_in);//gain_in;
						
						//PWM off
						__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
						RELAY_OFF();
						break;
					}
					case 14:
					{
						break;
					}
					default:
						break;
				}
				gain_max = 0.5*(gain_trans[3]+gain_trans[4]);
				gain_limit = gain_max - 3;
				
				for(ii=3;ii<=11;ii++)
				{
					if(gain_trans[ii]>= gain_limit)
					{
						A1 = gain_trans[ii];
						F1 = gain_freq[ii];
					}
					else
					{
						A2 = gain_trans[ii];
						F2 = gain_freq[ii];
						break;
					}
				}	
				A0 = gain_limit;
				if(F0 <= 10)
					F0 = 50;
				F0 = 0.7* F0 + 0.3*(F2 - (A2-A0)*(F2-F1)/(A2-A1));
				
				break;
			}
			case 2:     //提高部分   2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
			{
				if((FLAG_MODE_SET1==SET) && (FLAG_MODE_SET0==SET))
				{
					DAC_GAIN_Set(SET);
					LOAD_OFF();
					//DAC_SetFreqHz(1000);
					switch(flag_100Hz)
					{
						case 0:
						{
							RELAY_OFF();
							DAC_GAIN_Set(SET);
							break;
						}
						case 1:		//输入电阻 1V
						{
							regsitor_input_adv = Filter*regsitor_input_adv + (1-Filter)*regsitor_input_temp;
							
							vo_vpp_no_load = (ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1;	
							vo_avr_no_load = adc1_filter[3];
							
							LOAD_ON();
							//DAC_GAIN_Set(RESET); //0.1V 为case2准备
							break;
						}
						case 2:		//输出电阻 0.1V
						{
							vo_vpp_with_load = (ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1;
							vo_avr_with_load = adc1_filter[3];
							regsitor_output_adv = (double)(vo_avr_no_load-vo_avr_with_load) * 2000 / vo_avr_with_load;
							
							LOAD_OFF();
							DAC_GAIN_Set(SET); 
							DAC_SetFreqHz(50);
							break;
						}
						case 3:    //50Hz
						{
							//vo_vpp[0] = Filter*vo_vpp[0] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + 0)<<1);
							vo_vpp[0] = 0.65*vo_vpp[0] + (0.35)*((ad1_vrm_p[2] - vo_avr_1kHz + 0)<<1);
							gain_trans_adv[0] = 20*log10f(vo_vpp[0]/GAIN_ALL_1kHz/gain_in);//gain_in;
							
							if(gain_trans_adv[0] >= gain_trans[0]+3.3)
								flag_50Hz_gain = 1;
							if(gain_trans_adv[0] <= gain_trans[0]+2.4) // C2 double
								flag_50Hz_gain = 0;
							
							DAC_SetFreqHz(1000);
							break;
						}
						case 4:		//增益 1V 1kHz
						{
							vo_vpp[3] = Filter*vo_vpp[3] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_in = regsitor_input_adv/(regsitor_input_adv+10);
							//gain_in = 1;
							gain_trans_adv[3] = 20*log10f(vo_vpp[3]/GAIN_ALL_1kHz/gain_in); //    /gain_in !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							
							flag_vavr_adc = adc1_filter[3];
							if(gain_trans_adv[3] <= 0.72*gain_trans[3])
							{
								flag_gain_1kHz = 0;
							}
							else
							{
								flag_gain_1kHz = 1;
							}
							break;
						}
						case 5:
						{
							RELAY_ON();
							__HAL_TIM_SET_COUNTER(&htim1,0);
							__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
							break;
						}
						case 6:
						{
							__HAL_TIM_SET_AUTORELOAD(&htim1,800);
							__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,800);
							__HAL_TIM_SET_COUNTER(&htim1,0);
							break;
						}
						case 7:
						{
							__HAL_TIM_SET_COUNTER(&htim1,0);
							__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
							
							pulse_vpp_adc = adc1_filter[1];
							if(pulse_vpp_adc > 170)
								flag_pulse = 1;
							if(pulse_vpp_adc < 168)
								flag_pulse = 0;
							
							PWM_SetFreqkHz(10);
							break;
						}
						case 8:
						{
							vo_vpp[4] = Filter*vo_vpp[4] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_trans_adv[4] = 20*log10f(vo_vpp[4]/GAIN_ALL_10kHz/gain_in);
							PWM_SetFreqkHz(20);
							break;
						}
						case 9:
						{
							vo_vpp[5] = Filter*vo_vpp[5] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_trans_adv[5] = 20*log10f(vo_vpp[5]/GAIN_ALL_20kHz/gain_in);
							PWM_SetFreqkHz(40);
							break;
						}
						case 10:
						{
							vo_vpp[6] = Filter*vo_vpp[6] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_trans_adv[6] = 20*log10f(vo_vpp[6]/GAIN_ALL_40kHz/gain_in);
							PWM_SetFreqkHz(60);
							break;
						}
						case 11:
						{
							vo_vpp[7] = Filter*vo_vpp[7] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_trans_adv[7] = 20*log10f(vo_vpp[7]/GAIN_ALL_60kHz/gain_in);
							PWM_SetFreqkHz(100);
							break;
						}
						case 12:
						{
							vo_vpp[8] = Filter*vo_vpp[8] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_trans_adv[8] = 20*log10f(vo_vpp[8]/GAIN_ALL_100kHz/gain_in);
							PWM_SetFreqkHz(150);
							break;
						}
						case 13:
						{
							vo_vpp[9] = Filter*vo_vpp[9] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_trans_adv[9] = 20*log10f(vo_vpp[9]/GAIN_ALL_150kHz/gain_in);
							PWM_SetFreqkHz(200);
							break;
						}
						case 14:
						{
							vo_vpp[10] = Filter*vo_vpp[10] + (1-Filter)*((ad1_vrm_p[2] - adc1_filter[3] + Schottky_drop)<<1);
							gain_trans_adv[10] = 20*log10f(vo_vpp[10]/GAIN_ALL_200kHz/gain_in);
							
							
							if(gain_trans_adv[10] <= gain_trans[10] + 0.8)					
							{
								flag_gain_200kHz = 2;
							}
							if(gain_trans_adv[10] <= gain_trans[10] - 1.22)					
							{
								flag_gain_200kHz = 1;
							}
							if(gain_trans_adv[10] >= gain_trans[10] - 1.12)				
							{
								flag_gain_200kHz = 2;
							}
							if(gain_trans_adv[10] >= gain_trans[10] + 1.08)					
							{
								flag_gain_200kHz = 3; //C3 open
							}
							
							//PWM off
							__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
							RELAY_OFF();
							break;
						}
						default:
							break;
					}
					gain_max_adv = 0.5*(gain_trans_adv[3]+gain_trans_adv[4]);
					gain_limit_adv = gain_max_adv - 3;
					
					for(ii=3;ii<=14;ii++)
					{
						if(gain_trans_adv[ii]>= gain_limit_adv)
						{
							A1_adv = gain_trans_adv[ii];
							F1_adv = gain_freq[ii];
						}
						else
						{   
							A2_adv = gain_trans_adv[ii];
							F2_adv = gain_freq[ii];
							break;
						}
					}	
					A0_adv = gain_limit_adv;
					if(F0_adv <= 10)
						F0_adv = 50;
					F0_adv = 0.7* F0_adv + 0.3*(F2_adv - (A2_adv-A0_adv)*(F2_adv-F1_adv)/(A2_adv-A1_adv));
					break;
				}
			}
			default:
				break;
		}	
		
		flag_100Hz ++;
		if(flag_100Hz >= 15)
			flag_100Hz = 0;
	}
}

void ADC_VRMS(void)
{
	static int16_t adc_C = 0;
	static int16_t ad1_vrm_p1[adc1_channel] = {0};
	static int16_t ad1_vrm_n1[adc1_channel] = {4095};
	
	ADC1_Average(adc1_values);	
	
	for(adc_C = 0; adc_C < adc1_channel; adc_C ++)
	{
		if(adc1_filter[adc_C] >= ad1_vrm_p1[adc_C])
		{
			ad1_vrm_p1[adc_C] = adc1_filter[adc_C];
		}
		if(adc1_filter[adc_C] <= ad1_vrm_n1[adc_C])
		{
			ad1_vrm_n1[adc_C] = adc1_filter[adc_C];
		}
	}
	
	flag_ad_vrm ++;
	if(flag_ad_vrm >= 600)
	{
		flag_ad_vrm = 0;
		for(adc_C = 0; adc_C < adc1_channel; adc_C ++)
		{
			ad1_vrm_p[adc_C] = ad1_vrm_p1[adc_C];		//0.5*ad1_vrm_p[adc_C] + 0.5*ad1_vrm_p1[adc_C];
			ad1_vrm_n[adc_C] = ad1_vrm_n1[adc_C];		//0.5*ad1_vrm_n[adc_C] + 0.5*ad1_vrm_n1[adc_C];
			ad1_vrm_p1[adc_C] = 0;
			ad1_vrm_n1[adc_C] = 4095;
		}
	}

}

void DAC_GAIN_Set(FlagStatus GAIN)
{
	if(GAIN == SET)
	{
		for(Idx = 0; Idx <100; Idx++)  
		{  
			DualSinebit[Idx] = (Sinebit1[Idx] << 16)+ (Sinebit1[Idx]);  
		}
	}
	else
	{
		for(Idx = 0; Idx <100; Idx++)  
		{  
			DualSinebit[Idx] = (Sinebit2[Idx] << 16)+ (Sinebit2[Idx]);  
		}
	}
}

void PWM_SetFreqkHz(uint16_t freq) /*freq - kHz, 10kHz~4MHz*/
{
	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,168000/freq/2-1);
	__HAL_TIM_SET_AUTORELOAD(&htim1,168000/freq-1);
	__HAL_TIM_SET_COUNTER(&htim1,0);
}

void DAC_SetFreqHz(uint16_t freq)  /*freq - Hz, 10Hz~10kHz*/
{
	__HAL_TIM_SET_AUTORELOAD(&htim6,120000/freq-1);
	__HAL_TIM_SET_COUNTER(&htim6,0);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
