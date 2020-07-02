/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM14_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */


  TIM3->DIER |= TIM_DIER_UIE;
  TIM3->CR1  |= TIM_CR1_CEN;

  TIM14->DIER |= TIM_DIER_UIE;
  TIM14->CR1  |= TIM_CR1_CEN;

  BufInit(&uartBuf,512);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	  for(uint16_t i = 0; i < uartBuf.size; i++){
//		switch(i){
//		case 10: BufPut(&uartBuf,'P'); break;
//		case 11: BufPut(&uartBuf,'S'); break;
//		case 12: BufPut(&uartBuf,'D'); break;
//		case 13: BufPut(&uartBuf,'H'); break;
//		case 14: BufPut(&uartBuf,'E'); break;
//		case 15: BufPut(&uartBuf,'L'); break;
//		case 16: BufPut(&uartBuf,'L'); break;
//		case 17: BufPut(&uartBuf,'O'); break;
//
//		case 36: BufPut(&uartBuf,'P'); break;
//		case 37: BufPut(&uartBuf,'S'); break;
//		case 38: BufPut(&uartBuf,'D'); break;
//		case 39: BufPut(&uartBuf,'W'); break;
//		case 40: BufPut(&uartBuf,'O'); break;
//		case 41: BufPut(&uartBuf,'R'); break;
//		case 42: BufPut(&uartBuf,'L'); break;
//		case 43: BufPut(&uartBuf,'D'); break;
//		default: BufPut(&uartBuf,'h'); break;
//		}
//	  }

  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



void BufInit(Buffer *buf, uint16_t size){
	buf->size = size;
	buf->buffer = (uint8_t*) malloc(size);
	buf->tail = 0;
	buf->head = 0;
	buf->count = 0;
}

void BufPut(Buffer *buf, uint8_t data){
	buf->buffer[buf->tail] = data;
	if(buf->count < buf->size){
		buf->count++;
	}
	else {
		buf->head = buf->tail + 1;
	}
	buf->tail = (buf->tail < buf->size) ? buf->tail + 1 : 0;
}
uint8_t BufPop(Buffer *buf){
	uint8_t data = buf->buffer[buf->head++];
	buf->count--;
	buf->head = (buf->head < buf->size) ? buf->head : 0;
	return data;
}

void ClearBuf(Buffer *buf){
	buf->count = 0;
	buf->tail = 0;
	buf->head = 0;
}

uint8_t BufFindStart(Buffer *buf){
	uint16_t count = buf->head;

	while(count != buf->size){

		if (buf->buffer[count] == 'P' && buf->buffer[count + 1] == 'S' &&  buf->buffer[count + 2] == 'D'){
			buf->PSDpointer = count + 3;
			return 1;
		}
		count = (count < buf->size) ? count + 1 : 0;
	}
	return 0;
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
