/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "u8g2.h"
#include "usart.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/********************************* DEFINES ************************************/

// ВЕРСИЯ ПО - ИЮЛЬ 1.0
#define VERS "ИЮЛЬ 1.0"

/*------------------------- НАСТРА�?ВАЕМЫЕ ЗНАЧЕН�?Я ---------------------------*/
#define string_size 		60					// макс_количество_символов_в_строке
#define string_amount 		163					// макс_количество_строк
#define string_pack_amount 	36					// макс_количество_строк_в_1-ом_пакете

#define start_serv_string	155					// начало_служебных_строк
#define start_ctrl_string	149					// начало_строк_контроля

#define degree_sym_amount	4					// максимальное_количество_знаков_градуса_в_строке
#define UartBufSize 		4096				// размер_буфера_UART

#define UP_TIME 			86400*2				// время_засыпания_дисплея
#define UP_TIME_small		240					// время_засыпания_дисплея_в_режме_сна
#define transmit_time		500					// период_отправок_на_материнку

#define scroll_speed		15					// скорость_пролистывания_строк_при_долгом_нажатии (строк_в_секунду)
#define scroll_delay		600					// время_удержания_кнопки_до_начала_пролистывания (мс)
#define OFF_delay			3000				// время_удержания_кнопки_выключения_до_запуска_отключения (мс)
#define PowerOFF_delay		5000				// задержка_после_начала_выключения
#define start_pause			600					// задержка_перед_стартом_дисплея (длительность_стартового_мигания)
#define Button_Zummer		10					// задержка_на_писк_зумера_при_нажатии_кнопки


#define pack_DELAY			150					// задержка_между_получением_пакета_и_началом_парсинга
#define restart_time		550					// время_после_которого_перезапускается_UART_при_отсутствии_приема_данных

/*-------------------------- РАСЧ�?ТНЫЕ ЗНАЧЕН�?Я ------------------------------*/
#define serv_string_amnt	(string_amount - start_serv_string)		// Количество_служебных_строк
#define ctrl_string_amnt	(start_serv_string - start_ctrl_string)	// Количество_строк_контроля

			/*- - - - - - - - - Номера_Строк - - - - - - - - - */
#define serv_string_1		start_serv_string						// 1-я_служебная_строка
#define serv_string_2		start_serv_string + 1					// 2-я_служебная_строка

#define ctrl_string_1		start_ctrl_string						// 1-я_строка_контроля
#define ctrl_string_2		start_ctrl_string + 1					// 2-я_строка_контроля
#define ctrl_string_3		start_ctrl_string + 2					// 3-я_строка_контроля
#define ctrl_string_4		start_ctrl_string + 3					// 4-я_строка_контроля
#define ctrl_string_5		start_ctrl_string + 4					// 5-я_строка_контроля
#define ctrl_string_6		start_ctrl_string + 5					// 5-я_строка_контроля

/*----------------------- ДАТЧ�?К ТЕМПЕРАТУРЫ STM32 ---------------------------*/
#define TEMP110_CAL_ADDR 	((uint16_t*) ((uint32_t) 0x1FFF7A2E))	// адрес_калиброваного_значения_при_T = 110
#define TEMP30_CAL_ADDR 	((uint16_t*) ((uint32_t) 0x1FFF7A2C))	// адрес_калиброваного_значения_при_T = 30
#define VDD_CALIB 			((uint16_t) (330))
#define VDD_APPLI 			((uint16_t) (300))

/******************************************************************************/




/******************************** VARIABLES ***********************************/

/*-------------------------------- EXTERNs -----------------------------------*/

			/*- - - - - - - - - - - _Logo_  - - - - - - - - - - */
extern const uint8_t logo_width;
extern const uint8_t logo_height;
extern const uint8_t logo_bits[];

			/*- - - - - - - - Periferal_TypeDefs  - - - - - - - */
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim7;
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
			/*- - - - - - - - - -  UP_time - - - - - - - - - - */
extern uint32_t uptime_tick;
extern uint32_t uptime;

extern uint8_t display_stat;

/*------------------------------- STRUCTUREs ---------------------------------*/

/*
 * СТРУКТУРА СТРОК.
 * В НЕЕ КЛАДУТСЯ ПР�?НЯТЫЕ �? ОТПРАВЛЯЕМЫЕ СТРОК�?.
 */
typedef struct{
	uint8_t buf[string_size];						// буфер_строки
	uint8_t number;									// номер_строки
	uint8_t status;									// статус_строки (можно_ли_ее_выводить)
	uint8_t degreeSym[degree_sym_amount];			// массив_хранящий_положение_в_строке_знаков_градуса (до degree_sym_amount)
}string_t;

/*
 * СТРУКТУРА ДЛЯ БУФЕРОВ.
 * ПЛАН�?РОВАЛАСЬ УН�?ВЕРСАЛЬНОЙ, НО �?СПОЛЬЗУЕТСЯ
 * ТОЛЬКО ДЛЯ РАСЧЕТА CRC32
 */
typedef struct{
	uint8_t buf[string_pack_amount*string_size];	// основной_буфер
	uint16_t tail;									// хвост_буфера (буфер_не_цикличный, поэтому_и_количество_символов_в_буфере)
	uint16_t head;									// голова_буфера (начало_,_оставлен_при_необходимости_сделать_цикличный_буфер)
}buffer_t;

typedef struct{
	uint8_t LED_status;
	GPIO_TypeDef *LED_port;
	uint16_t LED_pin;
	uint32_t timer;
}led_status;
/*
 * СТРУКТУРА СОСТОЯН�?Й СВЕТОД�?ОДОВ RGB
 * ХРАН�?Т РЕЖ�?МЫ РАБОТЫ КАЖДОГО �?З 3-х СВЕТОД�?ОДОВ
 */
typedef struct{
	led_status RED;									// красный_светодиод
	led_status GREEN;								// зеленый_-//-
	led_status BLUE;								// синий_  -//-
}RGB_status;

/*
 * СТРУКТУРА СОСТОЯН�?Я УПРАВЛЯЕМЫХ С�?СТЕМ
 * ХРАН�?Т СОСТОЯН�?Е РЕЛЕ, DC �? Д�?СПЛЕЯ
 */
typedef struct{
	uint8_t RELE[4];
	uint8_t DC;
	uint8_t DISP;
}ctrl_status;

/******************************************************************************/


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

/******************************** PROTOTYPES **********************************/

/*------------------------------- U8G2 USAGE ---------------------------------*/
void DisplayInit(u8g2_t*);	//!
uint8_t LoadWindow(u8g2_t*, uint16_t, uint16_t, uint8_t*);
void DrawSym(u8g2_t*, uint8_t,uint8_t, uint8_t);
void OFF_Window(u8g2_t *u8g2);

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *, U8X8_UNUSED uint8_t, U8X8_UNUSED uint8_t, U8X8_UNUSED void *);
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *, uint8_t, uint8_t, void *);

/*--------------------------- ФУНКЦ�?�? УПРАВЛЕН�?Я -----------------------------*/
void PowerOFF();
void FlashWriteStart(); //!
void PowerON(u8g2_t *);
void SleepMode();

/*----------------------------- ФУНКЦ�?�? РАСЧЕТА ------------------------------*/
uint8_t TemperatureGetData(uint16_t);

/*--------------------------- ОБРАБОТЧ�?К�? НАЖАТ�?Й ----------------------------*/
void PowerButtonHandler(uint32_t *,uint8_t *,uint8_t *, uint16_t); //!
void ScrollingButtonHandler(uint8_t *, uint32_t*, uint8_t*);

/*------------------------------ CRC32 ФУНКЦ�?�? -------------------------------*/
void CRC32_Put(buffer_t*, uint8_t);
uint_least32_t Crc32(const unsigned char*, size_t);
uint8_t CRC32_Status(uint8_t *, size_t, uint16_t, buffer_t *);

/*----------------------- ФУНКЦ�?�? РАБОТЫ С С�?МВОЛАМ�? -------------------------*/
uint8_t FindString(uint8_t*,size_t,uint16_t*,const char*,size_t);
uint32_t HexToDec(char*,uint8_t);
void PassSym(uint8_t*, size_t, uint16_t*, uint8_t);
void PassSymCRC(uint8_t*,size_t,uint16_t*, uint8_t,buffer_t*);
char BigLatter(char);

/*-------------------- ФУНКЦ�?�? РАБОТЫ С СТРОКАМ�? string_t --------------------*/
void PutERROR(string_t*,const char*);
void LEDStringPars(string_t *datastring,RGB_status *leds);
void ServiceStringPars(string_t *, ctrl_status *);\

/*--------------------------------СВЕТОД�?ОДЫ-----------------------------------*/
void LED_control(led_status *, uint16_t, uint16_t);
void LEDs_OFF();

/*------------------------- ОБРАБОТЧ�?К�? ПРЕРЫВАН�?Й ---------------------------*/
void UPTIME_IRQHandler();

/******************************************************************************/

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define High12vThreshold 4000
#define Low12vThreshold 0
#define STM32_OUT_DOG_Pin GPIO_PIN_13
#define STM32_OUT_DOG_GPIO_Port GPIOC
#define STM32_BUTTON_POWER_Pin GPIO_PIN_0
#define STM32_BUTTON_POWER_GPIO_Port GPIOC
#define STM32_RS_DC_DC_Pin GPIO_PIN_1
#define STM32_RS_DC_DC_GPIO_Port GPIOC
#define STM32_relay_swith_pin_Pin GPIO_PIN_2
#define STM32_relay_swith_pin_GPIO_Port GPIOC
#define STM32_relay_outside_Pin GPIO_PIN_3
#define STM32_relay_outside_GPIO_Port GPIOC
#define STM32_ADC_NTC_1_Pin GPIO_PIN_0
#define STM32_ADC_NTC_1_GPIO_Port GPIOA
#define STM32_ADC_5V_Pin GPIO_PIN_1
#define STM32_ADC_5V_GPIO_Port GPIOA
#define STM32_ADC_3_3V_Pin GPIO_PIN_2
#define STM32_ADC_3_3V_GPIO_Port GPIOA
#define STM32_ADC_12V_Pin GPIO_PIN_3
#define STM32_ADC_12V_GPIO_Port GPIOA
#define OLED_RES_Pin GPIO_PIN_4
#define OLED_RES_GPIO_Port GPIOA
#define OLED_SCK_Pin GPIO_PIN_5
#define OLED_SCK_GPIO_Port GPIOA
#define OLED_D_C_Pin GPIO_PIN_6
#define OLED_D_C_GPIO_Port GPIOA
#define OLED_MOSI_Pin GPIO_PIN_7
#define OLED_MOSI_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_4
#define OLED_CS_GPIO_Port GPIOC
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define STM32_Conn_1_Pin GPIO_PIN_14
#define STM32_Conn_1_GPIO_Port GPIOE
#define STM32_Conn_2_Pin GPIO_PIN_15
#define STM32_Conn_2_GPIO_Port GPIOE
#define SYM_LED_G_Pin GPIO_PIN_12
#define SYM_LED_G_GPIO_Port GPIOB
#define BLUE_485_Pin GPIO_PIN_13
#define BLUE_485_GPIO_Port GPIOB
#define SYM_LED_B_Pin GPIO_PIN_14
#define SYM_LED_B_GPIO_Port GPIOB
#define GREEN_485_Pin GPIO_PIN_15
#define GREEN_485_GPIO_Port GPIOB
#define STM32_BUTTON_LED_POWER_Pin GPIO_PIN_8
#define STM32_BUTTON_LED_POWER_GPIO_Port GPIOD
#define RED_485_Pin GPIO_PIN_9
#define RED_485_GPIO_Port GPIOD
#define STM32_ZUMMER_Pin GPIO_PIN_10
#define STM32_ZUMMER_GPIO_Port GPIOD
#define BLUE_232_Pin GPIO_PIN_11
#define BLUE_232_GPIO_Port GPIOD
#define STM32_Relay_mmn_Pin GPIO_PIN_12
#define STM32_Relay_mmn_GPIO_Port GPIOD
#define GREEN_232_Pin GPIO_PIN_13
#define GREEN_232_GPIO_Port GPIOD
#define STM32_Relay_mother_Pin GPIO_PIN_14
#define STM32_Relay_mother_GPIO_Port GPIOD
#define RED_232_Pin GPIO_PIN_15
#define RED_232_GPIO_Port GPIOD
#define STM32_R_24_NC_Pin GPIO_PIN_6
#define STM32_R_24_NC_GPIO_Port GPIOC
#define RED_422_Pin GPIO_PIN_7
#define RED_422_GPIO_Port GPIOC
#define SYM_LED_R_Pin GPIO_PIN_8
#define SYM_LED_R_GPIO_Port GPIOC
#define GREEN_422_Pin GPIO_PIN_9
#define GREEN_422_GPIO_Port GPIOC
#define BLUE_422_Pin GPIO_PIN_8
#define BLUE_422_GPIO_Port GPIOA
#define STM32_R_24_NO_Pin GPIO_PIN_11
#define STM32_R_24_NO_GPIO_Port GPIOA
#define STM32_BUTTON_1_Pin GPIO_PIN_0
#define STM32_BUTTON_1_GPIO_Port GPIOD
#define STM32_BUTTON_2_Pin GPIO_PIN_1
#define STM32_BUTTON_2_GPIO_Port GPIOD
#define STM32_BUTTON_3_Pin GPIO_PIN_2
#define STM32_BUTTON_3_GPIO_Port GPIOD
#define STM32_BUTTON_LED_1_Pin GPIO_PIN_4
#define STM32_BUTTON_LED_1_GPIO_Port GPIOD
#define STM32_BUTTON_LED_2_Pin GPIO_PIN_5
#define STM32_BUTTON_LED_2_GPIO_Port GPIOD
#define STM32_BUTTON_LED_3_Pin GPIO_PIN_6
#define STM32_BUTTON_LED_3_GPIO_Port GPIOD
#define STM32_OUT_REL_4_Pin GPIO_PIN_7
#define STM32_OUT_REL_4_GPIO_Port GPIOD
#define STM32_OUT_REL_3_Pin GPIO_PIN_4
#define STM32_OUT_REL_3_GPIO_Port GPIOB
#define STM32_I2C_WP_Pin GPIO_PIN_5
#define STM32_I2C_WP_GPIO_Port GPIOB
#define STM32_OUT_REL_2_Pin GPIO_PIN_8
#define STM32_OUT_REL_2_GPIO_Port GPIOB
#define STM32_OUT_REL_1_Pin GPIO_PIN_9
#define STM32_OUT_REL_1_GPIO_Port GPIOB
#define INPUT_OUTSIDE_2_Pin GPIO_PIN_0
#define INPUT_OUTSIDE_2_GPIO_Port GPIOE
#define INPUT_OUTSIDE_1_Pin GPIO_PIN_1
#define INPUT_OUTSIDE_1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
