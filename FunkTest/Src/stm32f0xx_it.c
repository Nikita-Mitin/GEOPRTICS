/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
/* USER CODE BEGIN EV */
//uint8_t buf[] = "stroka 1: kanal 1 pitanie 12.32v TOK 2 mA "
//				"stroka 2: 12.1234° 12.3445 MPa            "
//				"stroka 3: 22.1234° 22.3445 MPa            "
//				"stroka 1: kanal 1 pitanie 12.32v TOK 2 mA "
//				"stroka 2: 12.1234° 12.3445 MPa            "
//				"stroka 3: 22.1234° 22.3445 MPa            ";
uint8_t u = 0;
uint8_t buf[] = "PSD;"
				"01:MODEBUS RS232 15200;"
				"03:MODBUS RS485 9600;"
				"02:UTC 22:28:30 05.04.2020;"
				"04:MODEBUS RS232 15200;"
				"07:MODBUS RS485 9600;"
				"06:UTC 22:28:30 05.04.2020;"
				"05:MODEBUS RS232 15200;"
				"08:MODBUS RS485 9600;"
				"09:UTC 22:28:30 05.04.2020;"
				"CRC:3F;";

uint8_t packet[] = "PDS;"
		"01:КВСМ-1 ГИТДТ № Версия ПО 1.6;"
		"02:UTC 09:22:22 2020-04-15;"
		"03:Канал 1 I:3mA U:4.12B Er:111;"
		"04:1Д1 -14.8887 -0.88876 MПа;"
		"05:1Д2 9.121234*C 11.12123 MПа;"
		"06:1Д3 33.13123*C 31.13123 MПа;"
		"07:1Д4 57.14123*C 59.14123 MПа;"
		"08:1Д5 81.15123*C 95.15123 MПа;"
		"09:1Д6 105.1612*C 139.1612 MПа;"
		"0A:Канал 2 I:6mA U:8.12B Er:222;"
		"0B:2Д1 -14.7887*C -0.78876 MПа;"
		"0C:2Д2 9.221234*C 11.22123 MПа;"
		"0D:2Д3 33.23123*C 31.23123 MПа;"
		"0E:2Д4 57.24123*C 59.24123 MПа;"
		"0F:2Д5 81.25123*C 95.25123 MПа;"
		"10:2Д6 105.2612*C 139.2612 MПа;"
		"11:Канал 3 I:9mA U:12.12B Er:333;"
		"12:3Д1 -14.6887*C -0.68876 MПа;"
		"13:3Д2 9.321234*C 11.32123 MПа;"
		"14:3Д3 33.33123*C 31.33123 MПа;"
		"15:3Д4 57.34123*C 59.34123 MПа;"
		"16:3Д5 81.35123*C 95.35123 MПа;"
		"17:3Д6 105.3612*C 139.3612 MПа;"
		"18:Канал 4 I:12mA U:16.12B Er:444;"
		"19:4Д1 -14.5887*C -0.58876 MПа;"
		"1A;4Д2 9.421234*C 11.42123 MПа;"
		"1B:4Д3 33.43123*C 31.43123 MПа;"
		"1C:4Д4 57.44123*C 59.44123 MПа;"
		"1D:4Д5 81.45123*C 95.45123 MПа;"
		"1E:4Д6 105.4612*C 139.4612 MПа;"
		"1F:Канал 5 I:15mA U:20.12B Er:555;"
		"20:5Д1 -14.4887*C -0.48876 MПа;"
		"CRC;NAN;";
//		"CRC;9F0F41DE;";
uint8_t packet2[] = "PDS;"
		"21;5Д2 9.521234*C 11.52123 MПа;"
		"22;5Д3 33.53123*C 31.53123 MПа;"
		"23;5Д4 57.54123*C 59.54123 MПа;"
		"24;5Д5 81.55123*C 95.55123 MПа;"
		"25;5Д6 105.5612*C 139.5612 MПа;"
		"26;Канал 6 I:18mA U:24.12B Er:666;"
		"27;6Д1 -14.3887*C -0.38876 MПа;"
		"28;6Д2 9.621234*C 11.62123 MПа;"
		"29;6Д3 33.63123*C 31.63123 MПа;"
		"2A;6Д4 57.64123*C 59.64123 MПа;"
		"2B;6Д5 81.65123*C 95.65123 MПа;"
		"2C;6Д6 105.6612*C 139.6612 MПа;"
		"2D;Сеть 1: 192.168.88.97;"
		"2E;Сеть 2: 255.255.255.255;"
		"2F;USB НЕ ПОДКЛЮЧЕН;"
		"30;USB :FLASH_123 7.7GB;"
		"31;USB :FLASH_123 7.7GB;"
		"32;MODBUS TCP порт: 1506;"
		"33;RTU RS-485 BR 115200 ID: 3;"
		"34;RTU RS-232 BR 115200 ID: 4;"
		"A0;R1G1B1;"
		"A1;R1R1R0R0DC1DISP1;"
		"CRC;NAN;";
//		"CRC;BEDCD061;";

//uint8_t packet2[] = "PDS;"
//		"01:KVSM-1 GITD № VERS PO 1.6;"
//		"02:UTC 09:22:22 2020-04-15;"
//		"03:KANAL 1 I:3mA U:4.12V Er:111;"
//		"04:1D1 -14.8887 -0.88876 MPa;"
//		"05:1D2 9.121234° 11.12123 MPa;"
//		"06:1D3 33.13123 31.13123 MPa;";
//		"07:1D4 57.14123 59.14123 MPa;"
//		"08:1D5 81.15123 95.15123 MPa;"
//		"09:1D6 105.1612 139.1612 MPa;"
//		"0A:KANAL 2 I:6mA U:8.12B Er:222;"
//		"0B:2D1 -14.7887 -0.78876 MPa;"
//		"0C:2D2 9.221234℃ 11.22123 MPa;"
//		"0D:2D3 33.23123℃ 31.23123 MPa;"
//		"0E:2D4 57.24123℃ 59.24123 MPa;"
//		"0F:2D5 81.25123℃ 95.25123 MPa;"
//		"10:2D6 105.2612℃ 139.2612 MPa;"
//		"11:KANAL 3 I:9mA U:12.12V Er:333;"
//		"12:3D1 -14.6887℃ -0.68876 MPa;"
//		"13:3D2 9.321234℃ 11.32123 MPa;"
//		"14:3D3 33.33123℃ 31.33123 MPa;"
//		"15:3D4 57.34123℃ 59.34123 MPa;"
//		"16:3D5 81.35123℃ 95.35123 MPa;"
//		"17:3D6 105.3612℃ 139.3612 MPa;"
//		"18:KANAL 4 I:12mA U:16.12V Er:444;"
//		"19:4D1 -14.5887℃ -0.58876 MPa;"
//		"1A;4D2 9.421234℃ 11.42123 MPa;"
//		"1B:4D3 33.43123℃ 31.43123 MPa;"
//		"1C:4D4 57.44123℃ 59.44123 MPa;"
//		"1D:4D5 81.45123℃ 95.45123 MPa;"
//		"1E:4D6 105.4612℃ 139.4612 MPa;"
//		"1F:KANAL 5 I:15mA U:20.12V Er:555;"
//		"20:5D1 -14.4887℃ -0.48876 MPa;";

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
	GPIOC->ODR ^= 1<<9;
  /* USER CODE END TIM3_IRQn 0 */
	HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles TIM14 global interrupt.
  */
void TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM14_IRQn 0 */
	GPIOC->ODR ^= 1<<8;
	if(!u) HAL_UART_Transmit(&huart1,&packet, strlen(&packet),0xFFFF);
	else HAL_UART_Transmit(&huart1,&packet2, strlen(&packet2),0xFFFF);
	u =! u;
//	HAL_UART_Transmit(&huart1,&packet2, strlen(&packet2),0xFFFF);
  /* USER CODE END TIM14_IRQn 0 */
	HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM14_IRQn 1 */

  /* USER CODE END TIM14_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
