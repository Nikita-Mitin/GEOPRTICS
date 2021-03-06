/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define UartBufSize 40*40*10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */





/**************************** GLOBAL VARIABLES ********************************/
uint8_t uart_buf[UartBufSize] = {'\0'};		// циклический_буфер_UART
uint8_t uart_hlf_flag = 0;					// флаг_половины_заполнения_буфера
uint8_t data_ready_flag = 0;				// флаг_готовности_данных_для_вывода (спарсен_хотяб_один_пакет)
string_t datastring[string_amount];			// массив_строк_данных - 											 											 										   общее_адрссное_пространство_для_получаемых_и_отправляемых_строк

uint16_t ADC_val[5];						// МАСС�?В ХРАН�?Т ЗНАЧЕН�?Е АЦП
												// 0 - СЕНСОР ТЕМПЕРАТУРЫ STM32
												// 1 - ТЕМП.СЕНСОР ПЛАТЫ 1
												// 2
												// 3
												// 4
uint8_t ADC_reset_count = 0;
uint8_t display_stat = 1;					// Переменная_статуса_дисплея
uint8_t LEDs_stat;							// Переменная_статуса_светодиодов

uint8_t WDG_TackArr[4] = {0};

uint8_t UTC[19] = {'\0'};
uint8_t UTC_show_flag = 0;

uint8_t service_mode = 0;
/******************************************************************************/


/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myDisplayTaskHandle;
osThreadId myDataTaskHandle;
osThreadId myControlTaskHandle;
osThreadId myLEDsTaskHandle;
osMessageQId myCursorQueueHandle;
osMessageQId myDispStatQueueHandle;
osMessageQId myDisplayStatHandle;
uint8_t myDisplayStatBuffer[ 4 * sizeof( uint8_t ) ];
osStaticMessageQDef_t myDisplayStatControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartDisplayTask(void const * argument);
void StartDataTask(void const * argument);
void StartControlTask(void const * argument);
void StartLEDsTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myCursorQueue */
  osMessageQDef(myCursorQueue, 5, uint8_t);
  myCursorQueueHandle = osMessageCreate(osMessageQ(myCursorQueue), NULL);

  /* definition and creation of myDispStatQueue */
  osMessageQDef(myDispStatQueue, 4, uint8_t);
  myDispStatQueueHandle = osMessageCreate(osMessageQ(myDispStatQueue), NULL);

  /* definition and creation of myDisplayStat */
  osMessageQStaticDef(myDisplayStat, 4, uint8_t, myDisplayStatBuffer, &myDisplayStatControlBlock);
  myDisplayStatHandle = osMessageCreate(osMessageQ(myDisplayStat), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myDisplayTask */
  osThreadDef(myDisplayTask, StartDisplayTask, osPriorityNormal, 0, 2800);
  myDisplayTaskHandle = osThreadCreate(osThread(myDisplayTask), NULL);

  /* definition and creation of myDataTask */
  osThreadDef(myDataTask, StartDataTask, osPriorityNormal, 0, 800);
  myDataTaskHandle = osThreadCreate(osThread(myDataTask), NULL);

  /* definition and creation of myControlTask */
  osThreadDef(myControlTask, StartControlTask, osPriorityNormal, 0, 400);
  myControlTaskHandle = osThreadCreate(osThread(myControlTask), NULL);

  /* definition and creation of myLEDsTask */
  osThreadDef(myLEDsTask, StartLEDsTask, osPriorityIdle, 0, 128);
  myLEDsTaskHandle = osThreadCreate(osThread(myLEDsTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/************************************************************
 * 															*
 *				 ПУСТАЯ СТАНДАРТНАЯ ЗАДАЧА					*
 *				 ВЫПОЛНЯЕТСЯ ТОГДА, КОГДА					*
 *				  Н�?ЧЕГО НЕ ВЫПОЛНЯЕТСЯ �?					*
 *				   ЗАПУСКАЕТ ПЕРЕФЕР�?Ю						*
 *				   											*
 ************************************************************/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */

/*------------------------- ЗАПУСКАЕМ ПЕРЕФЕР�?Ю ---------------------------*/
	HAL_UART_Receive_DMA(&huart3, (uint8_t*)uart_buf, UartBufSize);	// ЗАПУСК UART В РЕЖ�?МЕ DMA
	HAL_IWDG_Init(&hiwdg);											// ЗАПУСК WATHDOG
	HAL_TIM_Base_Start_IT(&htim7);									// ЗАПУСК ТАЙМЕРА UP_TIME
//	HAL_TIM_Base_Start_IT(&htim14);									// ЗАПУСК ТАЙМЕРА UP_TIME

	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_val, 5);				// ЗАПУСК АЦП В РЕЖ�?МЕ DMA

	datastring[ctrl_string_1].number = ctrl_string_1;
	datastring[ctrl_string_2].number = ctrl_string_2;
	datastring[ctrl_string_3].number = ctrl_string_3;
	datastring[ctrl_string_4].number = ctrl_string_4;
	datastring[ctrl_string_5].number = ctrl_string_5;
	datastring[ctrl_string_6].number = ctrl_string_6;
	datastring[ctrl_string_7].number = ctrl_string_7;
	datastring[ctrl_string_8].number = ctrl_string_8;
	datastring[ctrl_string_9].number = ctrl_string_9;
	datastring[ctrl_string_10].number = ctrl_string_10;

	PutString(&datastring[ctrl_string_9], VERS, 0);

	datastring[serv_string_1].number = serv_string_1;
	datastring[serv_string_2].number = serv_string_2;

	if(!HSE_status){
		HAL_GPIO_WritePin(SYM_LED_B_GPIO_Port, SYM_LED_B_Pin, GPIO_PIN_SET);
	}
	osDelay(500);
	HAL_TIM_Base_Start_IT(&htim13);									// ЗАПУСК ТАЙМЕРА WDG_TIME
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/*
 * ЗАДАЧА Р�?СОВАН�?Я НА Д�?СПЛЕЙ
 * ОТБ�?РАЕТ СТРОК�? ДЛЯ ВЫВОДА
 * ПР�?Н�?МАЕТ ЗНАЧЕН�?Е КУРСОРА
 * ВЫВОД�?Т СТРОК�? НА Д�?СПЛЕЙ
 *
 */
/************************************************************
 * 															*
 *				    ЗАДАЧА Р�?СОВАН�?Я НА Д�?СПЛЕЙ			*
 *				    ОТБ�?РАЕТ СТРОК�? ДЛЯ ВЫВОДА				*
 *				  	ПР�?Н�?МАЕТ ЗНАЧЕН�?Е КУРСОРА			*
 *				   	ВЫВОД�?Т СТРОК�? НА Д�?СПЛЕЙ				*
 *				   											*
 ************************************************************/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void const * argument)
{
  /* USER CODE BEGIN StartDisplayTask */

/************************** ПЕРЕМЕННЫЕ ЗАДАЧ�? ****************************/

/*-----------------------  ОСНОВНЫЕ ПЕРЕМЕННЫЕ ------------------------- */
	static u8g2_t u8g2;											// TypeDef_библиотеки_u8g2_для_рисования_на_дисплей
	uint16_t cursor = 0;										// Положение_окошка_дисплея
	string_t *rdy[string_amount];								// Массив_указателей_на_готовые_к_выводу_строки
//	uint8_t disp_stat = 1;										// Статус_дисплея. Определяется_приянтым_от_компа_значением
	uint8_t show_pointer = 0;
	uint8_t string_num[3] = {'\0'};
/*----------------------------- СЧЕТЧ�?К�? ------------------------------- */
	uint8_t rdy_count = 0;
	uint8_t show_count = 0;
	uint32_t displayOFF_del = 0;
	uint8_t chanel_num[2] = {'\0'};

/*----------------------- ФУНКЦ�?�? �?Н�?ЦАЛ�?ЗАЦ�?�? --------------------------*/
	DisplayInit(&u8g2);											// �?Н�?Ц�?АЛ�?ЗАЦ�?Я Д�?СПЛЕЯ
	PowerON(&u8g2);

/***************************** ТЕЛО ЗАДАЧ�? *******************************/
	for(;;){

		WDG_TackArr[0] = 1;
		// УСЛОВ�?Е ОПРЕДЕЛЯЮЩЕЕ, ГОТОВЫ Л�? ДАННЫЕ ДЛЯ ВЫВОДА
		// ЕСЛ�? НЕ ГОТОВЫ, ВЫВОД�?Т ОКНО ЗАГРУЗК�?,
		// �?НАЧЕ ВЫПОЛНЯЕТ ВЫВОД ДАННЫХ НА Д�?СПЛЕЙ
		if(!(data_ready_flag && uart_hlf_flag) && display_stat && !service_mode){
			display_stat = 3;
		}
		else if (display_stat){

			display_stat = (display_stat == 0) ? 0 : 1;

/*----------------------- ОТБ�?РАЕМ НУЖНЫЕ СТРОК�? -------------------------*/

			// ПРОСМАТР�?ЕВАЕМ ВЕСЬ МАСС�?В СТРОК В ПО�?СКАХ ГОТОВЫХ СТРОК
			// ЕСЛ�? СТРОКА ГОТОВА, В МАСС�?В УКАЗАТЕЛЕЙ ЗАП�?СЫВАЕМ АДРЕСС ГОТОВОЙ СТРОК�?
			rdy_count = 0;
			for(uint8_t i = 1; i < string_amount; i++){
				if(datastring[i].status){
					rdy[rdy_count] = &datastring[i];
					rdy_count++;
				}
			}

/*-------------------ОПРЕДЕЛЯЕМ ПОЛОЖЕН�?Е ОКНА(КУРСОРА) ------------------*/

			// ПР�?Н�?МАЕМ ЗНАЧЕН�?Е КУРСОРА �?З ЗАДАЧ�? StartControlTask
			xQueueReceive(myCursorQueueHandle, &cursor, 10);

			// ПРОВЕРЯЕМ, ЧТО ОКНО НЕ ВЫХОД�?Т ЗА КОНЕЦ ВЫВОД�?МЫХ СТРОК(т.к_количество_строк_может_меняться)
			// ЕСЛ�? ВЫХОД�?Т, ТО ПР�?Н�?МАЕМ МАКС�?МАЛЬНОЕ �? ОТПРАВЛЯЕМ ЗНАЧЕН�?Е ЗАДАЧЕ StartControlTask
			if(cursor > rdy_count - 1 && cursor < 160){
				cursor = 0;
				xQueueSendToBack(myCursorQueueHandle, &cursor, portMAX_DELAY);
			}
			else if(cursor > 160) {
				cursor = rdy_count-1;
				xQueueSendToBack(myCursorQueueHandle, &cursor, portMAX_DELAY);
			}

/*-------------------------- Р�?СУЕМ НА Д�?СПЛЕЙ --------------------------*/

			// ПР�?Н�?МАЕМ СТАТУС Д�?СПЛЕЯ �?З ЗАДАЧ�? StartControlTask
//			taskENTER_CRITICAL();
//			xQueueReceive(myDispStatQueueHandle, &display_stat, 100);
//			taskEXIT_CRITICAL();

			u8g2_ClearBuffer(&u8g2);										// ОЧ�?ЩАЕМ БУФЕР Д�?СПЛЕЯ

			u8g2_SetFont(&u8g2, u8g2_font_unifont_t_cyrillic);				// УСТАНАВЛ�?ВАЕМ К�?Р�?Л�?ЧЕСК�?Й ШР�?ФТ

			u8g2_SetDrawColor(&u8g2, 1);

			show_count = 4;
			show_pointer = cursor;

			while(show_count){

				if(service_mode){
					u8g2_DrawUTF8(&u8g2, 0, 15 + 15 * (4 - show_count), (char *) itoa(rdy[show_pointer]->number,(char*) &string_num, 10));
				}

				u8g2_DrawUTF8(&u8g2, service_mode * (9 * strlen((char*) &string_num) + 3), 15+ 15 * (4 - show_count),(char *) rdy[show_pointer]->buf);

				// ВВОД�?М В БУФЕР Д�?СПЛЕЯ ЗНАК�? С�?МВОЛОВ ГРАДУСА СТРОК�? В МЕСТА, НА КОТОРЫХ ОН�? РАСПОЛОЖЕНЫ
				for(uint8_t j = 0; j < degree_sym_amount; j++){
					if(rdy[show_pointer]->degreeSym[j] != '\0'){
						DrawSym(&u8g2, rdy[show_pointer]->degreeSym[j] * 7 + 7 + service_mode * (9 * strlen((char*) &string_num) + 3), 15 + 15 * (4 - show_count), 176);

						u8g2_SetFont(&u8g2, u8g2_font_unifont_t_cyrillic);
						u8g2_DrawUTF8(&u8g2, rdy[show_pointer]->degreeSym[j] * 7 + 17 + service_mode * (9 * strlen((char*) &string_num) + 3), 15 + 15 * (4 - show_count), "C");
					}
				}


				show_pointer = (show_pointer <= rdy_count - 2) ? show_pointer + 1 : 0;
				show_count--;
			}

			if(UTC_show_flag){

				itoa(rdy[cursor]->chanel_num,(char *) &chanel_num, 10);

				u8g2_SetFontMode(&u8g2, 1);
				u8g2_DrawBox(&u8g2, 0, 0, 254, 18);
				u8g2_SetDrawColor(&u8g2, 2);
				u8g2_DrawUTF8(&u8g2, 0, 15,"Канал");
				u8g2_DrawUTF8(&u8g2, 0 + 48, 15, (char *) &chanel_num);
				u8g2_DrawUTF8(&u8g2, 64, 15,"UTC");
				u8g2_DrawUTF8(&u8g2, 64 + 30, 15,(char *) &UTC);
			}
		}
//			// ОТПРАВЛЯЕМ БУФЕР БЕЗ РЕЖ�?МОВ (ДЛЯ ОТЛАДК�?)
//			u8g2_SendBuffer(&u8g2);

//			 ВЫБ�?РАЕМ, ЧТО ДЕЛАТЬ С ВВЕДЕННЫМ�? ДАННЫМ�?, ОСНОВЫВАЯСЬ НА СТАТУСЕ Д�?СПЛЕЯ
//			 0 - НА Д�?СПЛЕЙ Н�?ЧЕГО НЕ ВЫВОД�?ТСЯ
//			 1 - Д�?СПЛЕЙ РАБОТАЕТ В НОРМАЛЬНОМ РЕЖ�?МЕ, �? ЗАСЫПАЕТ ПОСЛЕ UP_TIME(РЕДКО)
//			 2 - Д�?СПЛЕЙ РАБОТАЕТ В РЕЖ�?МЕ СНА �? ЗАСЫПАЕТ ПОСЛЕ UP_TIME_small(ЧАСТО)

		switch (display_stat){
		case 0:
			displayOFF_del = HAL_GetTick();

			while(HAL_GetTick() - displayOFF_del < PowerOFF_delay - 10 || displayOFF_del > HAL_GetTick()){
				u8g2_ClearBuffer(&u8g2);
				OFF_Window(&u8g2);
			}
			u8g2_ClearDisplay(&u8g2);
			vTaskDelete(myDisplayTaskHandle);
			break;
		case 1:
			if(uptime_tick - uptime < UP_TIME){
				u8g2_SendBuffer(&u8g2);
			}
			else{
				u8g2_ClearDisplay(&u8g2);
			}
			break;
		case 2:
			if(uptime_tick - uptime < UP_TIME_small){
				u8g2_SendBuffer(&u8g2);
			}
			else{
				u8g2_ClearDisplay(&u8g2);
			}
			break;
		case 3:
			u8g2_ClearBuffer(&u8g2);
			LoadWindow(&u8g2, logo_width, logo_height,(uint8_t *) &logo_bits);
			u8g2_SendBuffer(&u8g2);
		}

		// ПЕРЕВОД�?М ЗАДАЧУ В РЕЖ�?М ОЖ�?ДАН�?Я (мс)
		osDelay(10);
	}
  /* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartDataTask */
/************************************************************
 * 															*
 * 				 ЗАДАЧА ПАРСЕРА ПАКЕТОВ UART				*
 *		ПАРС�?Т СТРОК�? В МАСС�?В string_t datastring[]		*
 *					�?НДЕКС - НОМЕР СТРОК�?					*
 *				ПРОВЕРЯЕТ ПАКЕТ НА ЦЕЛОСТНОСТЬ				*
 *															*
*************************************************************/
/* USER CODE END Header_StartDataTask */
void StartDataTask(void const * argument)
{
  /* USER CODE BEGIN StartDataTask */

/************************** ПЕРЕМЕННЫЕ ЗАДАЧ�? ****************************/

		/*- - - - - - - -  ОСНОВНЫЕ ПЕРЕМЕННЫЕ - - - - - - - - - */
	uint16_t stringnum = 0;										// ХРАН�?Т НОМЕР ТЕКУЩЕЙ СТРОК�?
	buffer_t crc32_buf;											// БУФЕР, В КОТОРЫЙ ЗАП�?СЫВАЮТСЯ ДАННЫЕ ДЛЯ РАСЧЕТА СRC32
	char strnumhex[2];											// С�?МВОЛЬНЫЙ БУФЕР, ХРАНЯЩ�?Й НОМЕР СТРОК�? в hex

		/*- - - - - - - - - - - СЧЕТЧ�?К�?  - - - - - - - - - - - -*/
	uint16_t counter = 0;										// СЧЕТЧ�?К Ц�?КЛ�?ЧЕСКОГО БУФЕРА (АТАВ�?ЗМ)
	uint16_t pointer = 0;										// УКАЗАТЕЛЬ НА С�?МВОЛ БУФЕРА uart_buf
	uint8_t celsium_count = 0;									// СЧЕТЧ�?К ДЛЯ ЗАПОЛНЕН�?Я ЗНАКОВ ГРАДУСА ЦЕЛЬС�?Я
	uint8_t string_count = 0;									// СЧЕТЧ�?К ДЛЯ ЗАПОЛНЕН�?Я СТРОК
	uint32_t restart_val = 0;
	uint8_t UTC_count = 0;
	uint16_t UTC_pointer = 0;
	uint16_t chanel_pointer = 0;
	uint8_t chanel_val = 0;

	uint8_t last_transmit_str_count = 0;
	uint32_t last_transmit = 0;
	uint8_t last_transmit_char[7] = {'\0'};
	uint32_t mother_restart_last = 0;

	osDelay(500);
/***************************** ТЕЛО ЗАДАЧ�? *******************************/
	for(;;){

		WDG_TackArr[1] = 1;
		// ЕСЛ�? RX НЕ В ПРОСТОЕ, ТО ЖДЕМ ОПРЕДЕЛЕННОЕ ВРЕМЯ
		// �? НАЧ�?НАЕМ ПО�?СК СТАРТОВОЙ КОМБ�?НАЦ�?�?
		// УСЛОВ�?Е ПОМАГАЕТ ПАРС�?ТЬ НОВЫЙ ПАКЕТ ОД�?Н РАЗ
		if(!(USART3->SR & USART_SR_IDLE)){
			restart_val = HAL_GetTick();

			HAL_GPIO_WritePin(STM32_BUTTON_LED_POWER_GPIO_Port, STM32_BUTTON_LED_POWER_Pin, GPIO_PIN_RESET);

			osDelay(pack_DELAY);

			counter = UartBufSize;

			// ТЕЛО ПАРСЕРА:
			// ПРОБЕГАЕТ ВЕСЬ БУФЕР В ПО�?СКАХ СТАРТОВОЙ КОМБ�?НАЦ�?�?
			// ПОСЛЕ ТОГО, КАК НАЙДЕТ, ОБРАБАТЫВАЕТ ПАКЕТ �? ВЫХОД�?Т �?З ТЕЛА Ц�?КЛА
			while(counter){

				// ЕСЛ�? НАЙДЕНА СТАРТОВАЯ КОМБ�?НАЦ�?Я, ТО НАЧ�?НАЕМ ПАРС�?ТЬ
				if(FindString((uint8_t *)&uart_buf, UartBufSize, &pointer,"PDS", 3)){

					last_transmit = uptime_tick;

					// ПРОПУСКАЕМ С�?МВОЛ ';'
					PassSym((uint8_t *)&uart_buf, UartBufSize, &pointer, 1);

					// РАСЧ�?ТЫВАЕМ CRC32, ЕСЛ�? CRC СОШЕЛСЯ, ПРОДОЛЖАЕМ ПАРС�?ТЬ
					// �?НАЧЕ ОСТАНАВЛ�?ВАЕМ ПАРС�?НГ: ФУНКЦ�?Я ЗАП�?СЫВАЕТ �? ОШ�?БК�? CRC
					if(CRC32_Status((uint8_t *)&uart_buf, UartBufSize, pointer, &crc32_buf)){}
					else break;

					// Ц�?КЛ ПАРС�?Т string_pack_amount СТРОК ПО ОТДЕЛЬНОСТ�?
					for(uint8_t i = 0; i < string_pack_amount; i++){

						// ЕСЛ�? ВМЕСТО НОМЕРА СТРОК�? НАЙДЕНА КОМ�?НАЦ�?Я CRC - ПАКЕТ ОКОНЧЕН, ВЫХОД�?М �?З ПАРС�?НГА
						if(FindString((uint8_t *)&uart_buf, UartBufSize, &pointer, "CRC", 3)){
							break;
						}

						if(uart_buf[pointer] == (uint8_t)';'){
							break;
						}

						string_count = 0;
						celsium_count = 0;

						// НОМЕР СТРОК�? ВСЕГДА СОСТО�?Т �?З ДВУХ С�?МВОЛОВ
						// ПАРС�?М ПЕРВЫЙ С�?МВОЛ �? ПЕРЕХОД�?М К СЛЕДУЮЩЕМУ
						strnumhex[0] = uart_buf[pointer];
						PassSym((uint8_t *)&uart_buf, UartBufSize, &pointer, 1);

						// ПАРС�?М ВТОРОЙ С�?МВОЛ �? ПРОПУСКАЕМ 2 С�?МВОЛА: ЭТОТ �? ';'
						strnumhex[1] = uart_buf[pointer];
						PassSym((uint8_t *)&uart_buf, UartBufSize, &pointer, 2);

						// ПЕРЕВОД�?М СТРОКУ НОМЕРА: (char, hex) -> (int, dec)
						stringnum = HexToDec((char *) &strnumhex,2);
						datastring[stringnum].number = stringnum;

						// ПАРС�?М СОДЕРЖ�?МОЕ СТРОК�?:
						// ПОКА НЕ НАЙДЕМ ЗНАК ';' �?Л�? СТРОКА НЕ ЗАКОНЧ�?ТСЯ
						// КЛАДЕМ КАЖДЫЙ С�?МВОЛ СОДЕРЖ�?МОГО БУФЕРА НАЧ�?НАЯ С pointer
						// В БУФЕР СТРОК�? НАЧ�?НАЯ С string_count (0)
						while((uart_buf[pointer] != (uint8_t)';') && (string_count < string_size)){

							// ЕСЛ�? НАЙДЕНА КОМБ�?НАЦ�?Я DEL, ЗНАЧ�?Т СТРОКУ НУЖНО УДАЛ�?ТЬ (СТАТУС СТРОК�? = 0)
							// �? ПАРС�?НГ СОДЕРЖ�?МОГО СТРОК�? ПРЕКРАЩАЕТСЯ
							// �?НАЧЕ СТАТУС СТРОК�? = 1
							if(FindString((uint8_t *)&uart_buf, UartBufSize, &pointer, "DEL", 3) && !string_count){
								datastring[stringnum].status = 0;
								break;
							}
							else if(!string_count) datastring[stringnum].status = 1;

							UTC_pointer = pointer;

							if(FindString((uint8_t *)&uart_buf, UartBufSize, &UTC_pointer, "UTC", 3)){

								PassSym((uint8_t *)&uart_buf, UartBufSize, &UTC_pointer, 1);
								UTC_count = 0;

								while(UTC_count < 19){
									UTC[UTC_count] = uart_buf[UTC_pointer];
									UTC_pointer++;
									UTC_count++;
								}
							}

							chanel_pointer = pointer;

							if(FindString((uint8_t *)&uart_buf, UartBufSize, &chanel_pointer,"Канал", 6)){
								PassSym((uint8_t *)&uart_buf, UartBufSize, &chanel_pointer, 5);
								chanel_val = uart_buf[chanel_pointer] - 48;
							}
							// ЕСЛ�? НАЙДЕНА КОМБ�?НАЦ�?Я "*C" ВМЕСТО НЕЕ КЛАДЕМ В СТРОКУ ' '
							// �? ЗАП�?СЫВАЕМ ПОЛОЖЕН�?Е ЗНАКА ГРАДУСА (string_count)
							if(FindString((uint8_t *)&uart_buf,UartBufSize,&pointer,"*C",2)){
								datastring[stringnum].buf[string_count] = (uint8_t)' ';
								datastring[stringnum].degreeSym[celsium_count] = string_count;
								string_count++;
								datastring[stringnum].buf[string_count] = (uint8_t)' ';
								string_count++;
								datastring[stringnum].buf[string_count] = (uint8_t)' ';
								string_count++;
								celsium_count++;
							}

							// ЕСЛ�? Н�? ОДНА �?З КОМБ�?НАЦ�?Й НЕ БЫЛА НАЙДЕНА
							// ПРОСТО КЛАДЕМ С�?МВОЛ БУФЕРА В СТРОКУ
							datastring[stringnum].buf[string_count] = uart_buf[pointer];
							PassSym((uint8_t *)&uart_buf, UartBufSize, &pointer, 1);
							string_count++;
						}

						datastring[stringnum].chanel_num = chanel_val;

						// КОГДА СТРОКА УЖЕ ЗАПОЛНЕНА ДАННЫМ�?,
						// ОЧ�?ЩАЕМ ОСТАВШ�?ЕСЯ ЭЛЕМЕНТЫ В БУФЕРЕ СТРОК�?
						while(string_count < string_size){
							datastring[stringnum].buf[string_count] = '\0';
							string_count++;
						}
						// ПРОПУСКАЕМ ';'
						PassSym((uint8_t *)&uart_buf, UartBufSize, &pointer, 1);
					}

					// СТАВ�?М ФЛАГ ГОТОВНОСТ�? ДАННЫХ, ДЛЯ ВЫХОДА �?З ЗАГРУЗОЧНОГО ОКНА
					// �? ВЫХОД�?М �?З ПАРСЕРА ПАКЕТА
					data_ready_flag = 1;

					for(uint16_t i = 0; i < UartBufSize; i++){
						uart_buf[i] = '\0';
					}
					break;
				}

				// ЕСЛ�? PSD НЕ НАЙДЕН, ПРОПУСКАЕМ С�?МВОЛ ЗА С�?МВОЛОМ ПОКА БУФЕР НЕ КОНЧ�?ТСЯ
				PassSym((uint8_t *)&uart_buf, UartBufSize, &pointer, 1);
				counter--;
			}

			for(uint8_t i = 0; i < 7; i++){
				last_transmit_char[i] = '\0';
			}

			itoa(uptime_tick - last_transmit, (char*)&last_transmit_char, 10);

			for(uint8_t i = 0; i < string_size; i++){
				datastring[ctrl_string_7].buf[i] = '\0';
			}

			last_transmit_str_count = 0;

			PutString(&datastring[ctrl_string_7], "Last connection - ", &last_transmit_str_count);

			last_transmit_str_count = 18;
			PutString(&datastring[ctrl_string_7], (const char*) last_transmit_char, &last_transmit_str_count);


			if(!counter){
				// ВЫВОД�?М ОШ�?БКУ, ЧТО СТАРТОВАЯ КОМБ�?НАЦ�?Я НЕ БЫЛА НАЙДЕНА
				PutERROR((string_t *)&datastring,"PACK_NOT_FOUND");
			}
		}

		// ЕСЛ�? ДАННЫЕ НЕ ОБНОВЛЯЮТСЯ КАКОЕ ТО ВРЕМЯ,
		// ПЕРЕЗАПУСКАЕМ UART, ДЛЯ �?ЗБЕЖАН�?Я ВОЗМОЖНЫХ ОШ�?БОК
		if(HAL_GetTick() - restart_val > restart_time || restart_val > HAL_GetTick()){
			for(uint16_t i = 0; i < UartBufSize; i++){
				uart_buf[i] = '\0';
			}
			HAL_UART_DMAStop(&huart3);
			HAL_UART_Receive_DMA(&huart3, (uint8_t*)uart_buf, UartBufSize);
		}

		if(uptime_tick - last_transmit > mother_wdgtime && uptime_tick - mother_restart_last > mother_wdgtime){
			HAL_GPIO_WritePin(STM32_Relay_mother_GPIO_Port, STM32_Relay_mother_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SYM_LED_G_GPIO_Port, SYM_LED_G_Pin, GPIO_PIN_SET);
			mother_restart_last = uptime_tick;
			while(uptime_tick - mother_restart_last >10000){
				osDelay(1000);
				WDG_TackArr[1] = 1;
			}

			HAL_GPIO_WritePin(SYM_LED_G_GPIO_Port, SYM_LED_G_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(STM32_Relay_mother_GPIO_Port, STM32_Relay_mother_Pin, GPIO_PIN_SET);
			mother_restart_last = uptime_tick;
		}

		// ОБНАВЛЯЕМ WATHDOG


//		if(!HAL_GPIO_ReadPin(STM32_BUTTON_POWER_GPIO_Port, STM32_BUTTON_POWER_Pin)){
			HAL_GPIO_WritePin(STM32_BUTTON_LED_POWER_GPIO_Port, STM32_BUTTON_LED_POWER_Pin, GPIO_PIN_SET);
//		}
		// ПЕРЕВОД�?М ЗАДАЧУ В РЕЖ�?М ОЖ�?ДАН�?Я. ВРЕМЯ МАЛО, ЧТОБ НЕ ПРОПУСТЬ ПАКЕТ
		osDelay(10);
	}
  /* USER CODE END StartDataTask */
}

/* USER CODE BEGIN Header_StartControlTask */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 														   *
 *				 ЗАДАЧА ОБРАБОТК�? СЛУЖЕБНЫХ СТРОК		   *
 *				  �? СОСТАВЛЕН�?Я СТРОК НА ОТПРАВКУ		   *
 *														   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* USER CODE END Header_StartControlTask */
void StartControlTask(void const * argument)
{
  /* USER CODE BEGIN StartControlTask */

/************************** ПЕРЕМЕННЫЕ ЗАДАЧ�? ****************************/

/*-------------------- ШАБЛОНЫ ДЛЯ ЗАПОЛНЕН�?Я СТРОК ---------------------*/
	const uint8_t ctrl_template[] = "R?R?R?R?DC?";				// ШАБЛОН ДЛЯ СТРОК�? С ДАННЫМ�? O РЕЛЕ �? DC
	const uint8_t but_template[] = 	"B?B?B?PB?";				// ШАБЛОН ДЛЯ СТРОК�? С ДАННЫМ�? СОСТОЯН�? КНОПОК

/*-------------------------- ПЕРЕМЕННЫЕ CRC32 ---------------------------*/
	uint16_t crc_count = 0;										// СЧЕТЧ�?К ДЛЯ ОПРЕДЕЛЕН�?Я РАЗМЕРА CRC32
	uint8_t crc_hex_buf[10];									// С�?МВОЛЬНЫЙ БУФЕР, ХРАНЯЩ�?Й ЗАП�?СЬ CRC32 В hex
	int32_t crc32;												// ХРАН�?Т РАСЧ�?ТАНОЕ ЗНАЯЕН�?Е CRC32

/*------------------------- ПЕРЕМЕННЫЕ RELE/DC --------------------------*/
	ctrl_status active;											// ПЕРЕМЕННАЯ СТРУКТУРЫ СТРОК РЕЛЕ/DC
	uint16_t RELE_PINS[] = {STM32_OUT_REL_1_Pin, 				// МАСС�?В, ХРАНЯЩ�?Й НОМЕРА П�?НОВ РЕЛЕ
							STM32_OUT_REL_2_Pin, 					//
							STM32_OUT_REL_3_Pin, 					//
							STM32_OUT_REL_4_Pin};					//
	GPIO_TypeDef *RELE_PORTS[] = {STM32_OUT_REL_1_GPIO_Port, 	// МАСС�?В, ХРАНЯЩ�?Й НОМЕРА ПОРТОВ РЕЛЕ
								STM32_OUT_REL_2_GPIO_Port, 			//
								STM32_OUT_REL_3_GPIO_Port, 			//
								STM32_OUT_REL_4_GPIO_Port};			//
	uint8_t RB_count = 0;										// РЕЛЕ/DC СЧЕТЧ�?К

/*-------------------------- ФЛАГ�? СОСТОЯН�?Я ---------------------------*/
	uint8_t switch_off = 0;										// ФЛАГ ВЫКЛЮЧЕН�?Я П�?ТАН�?Я
	uint8_t flash_on = 0;										// ФЛАГ ЗАП�?С�? В ФЛЭШ

/*--------------------------- ВСЕ ДЛЯ КНОПОК ----------------------------*/
	uint8_t but_buf[4];											// БУФЕР ХРАН�?Т СОСТОЯН�?Е 4-Ч КНОПОК
																	// (0, 1 - ПРОКРУТКА; 2 - ФЛЭШ; 3 - ВЫКЛЮЧЕН�?Е)
	uint8_t cursor = 0;											// ПОЛОЖЕН�?Е КУРСОРА
	uint8_t butmem = 0;											// ПЕРЕМЕННАЯ ЗАПОМ�?НАЕТ СОСТОЯН�?Е КНОПОК
	uint8_t power_butmem = 0;
	uint32_t butthold = 0;										// ПЕРЕМЕННАЯ ДЛЯ ОТСЧЕТА ВРЕМЕН�? НАЖАТ�?Я КНОПК�?
	uint32_t power_butthold = 0;

/*----------------------- БУФЕР НА ОТПРАВКУ UART ------------------------*/
	uint8_t sendBuf[ctrl_string_amnt*string_size] = {'\0'};		// ОТПРАВЛЯЕМЫЙ ПО UART БУФЕР
	uint16_t sB_pointer = 0;									// УКАЗАТЕЛЬ БУФЕРА
	uint8_t sB_counter = 0;										// СЧЕТЧ�?К БУФЕРА
	uint32_t transmit_timer = 0;								// СОХРАНЯЕТ ВРЕМЯ ПОСЛЕДНЕЙ ОТПРАВК�?
	uint32_t flash_transmit_timer = 0;								// СОХРАНЯЕТ ВРЕМЯ ПОСЛЕДНЕЙ ОТПРАВК�?

/*--------------------------- ПЕРЕМЕННЫЕ АЦП ---------------------------*/
	uint8_t ADC_char[4];										// БУФЕР ДЛЯ ПЕРЕВОДА ЗНАЧЕН�?Я АЦП В СТРОКУ
	uint16_t ADC_statistics[5][5] = {'\0'};						// Статистика для усреднения
	uint32_t ADC_stat_time = 0;
	uint8_t ADC_summ_count = 0;
	uint16_t ADC_mid = 0;
	uint16_t ADC_real = 0;

	char up_time_char[10];
	uint8_t adc_count = 0;			 							// СЧЕТЧ�?К АЦП
	uint8_t adc_str_count = 0;									// СЧЕТЧ�?К СТРОК АЦП
	uint8_t opt_buf[6] = {0};

	uint32_t service_hold = 0;
	uint8_t service_mem = 0;
	uint8_t service_mem2 = 0;

	uint8_t con1_mem = 0;
	uint8_t con2_mem = 0;

/*-------------------------- ФУНКЦ�?�? ЗАПУСКА ----------------------------*/
//	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_val,5);			// ЗАПУСК АЦП В РЕЖ�?МЕ DMA



	osDelay(500);
/***************************** ТЕЛО ЗАДАЧ�? *******************************/
	for(;;){

		WDG_TackArr[2] = 1;

/*--------------------- ОБРАБАТЫВАЕМ НАЖАТ�?Я КНОПОК ---------------------*/

		// ПР�?Н�?МАЕМ ЗНАЧЕН�?Е КУРСОР �?З ЗАДАЧ�? StartDisplayTask
		xQueueReceive(myCursorQueueHandle,&cursor, 10);

		// ОБРАБАТЫВАЕМ КНОПК�? ПРОКРУТК�? Д�?СПЛЕЯ
		ScrollingButtonHandler(&cursor, &butthold, &butmem);

		// ОТПРАВЛЯЕМ ОБНОВЛЕННОЕ ЗНАЧЕН�?Е КУРСОРА ОБРАТНО В ЗАДАЧУ StartDisplayTask
		xQueueSendToBack(myCursorQueueHandle, &cursor, 10);

		// ОБРАБАТЫВАЕМ КНОПКУ ОТКЛЮЧЕН�?Я П�?ТАН�?Я
		PowerButtonHandler(&power_butthold, &power_butmem, &switch_off, OFF_delay - 1000);

		ServiceModeButtonHandler(&service_mem, &service_mem2, &service_hold, service_but_delay);

		ConnHandler(&con1_mem,&con2_mem);

		if(ADC_summ_count < 5 && (HAL_GetTick() - ADC_stat_time > 50 || ADC_stat_time > HAL_GetTick())){
			for(uint8_t i = 0; i < 5; i++){
				ADC_statistics[i][ADC_summ_count] = ADC_val[i];
			}
			ADC_summ_count++;
			ADC_stat_time = HAL_GetTick();
		}
/*----------- ЗАП�?СЫВАЕМ СОСТОЯН�?Е КНОПОК В МАСС�?В СОСТОЯН�?Я -----------*/
		if(butmem & STM32_BUTTON_3_Pin){
			flash_on = 1;
			flash_transmit_timer = HAL_GetTick();
			UTC_show_flag = 1;
		}
		else UTC_show_flag = 0;

		but_buf[0] = (butmem & STM32_BUTTON_1_Pin) >> 1;			    // КНОПКА ПРОКУРТК�? ВН�?З
		but_buf[1] = butmem & STM32_BUTTON_2_Pin;						// КНОПКА ПРОКРУТК�? ВВЕРХ
		but_buf[2] = flash_on;											// КНОПКА ЗАП�?С�? ВО ФЛЭШ
		but_buf[3] = switch_off;										// КНОПКА ВЫКЛЮЧЕН�?Я

		opt_buf[0] = HAL_GPIO_ReadPin(STM32_R_24_NO_GPIO_Port,STM32_R_24_NO_Pin);
		opt_buf[1] = HAL_GPIO_ReadPin(STM32_R_24_NC_GPIO_Port,STM32_R_24_NC_Pin);
		opt_buf[2] = HAL_GPIO_ReadPin(INPUT_OUTSIDE_1_GPIO_Port,INPUT_OUTSIDE_1_Pin);
		opt_buf[3] = HAL_GPIO_ReadPin(INPUT_OUTSIDE_2_GPIO_Port,INPUT_OUTSIDE_2_Pin);
		opt_buf[4] = HAL_GPIO_ReadPin(STM32_Conn_1_GPIO_Port,STM32_Conn_1_Pin);
		opt_buf[5] = HAL_GPIO_ReadPin(STM32_Conn_2_GPIO_Port,STM32_Conn_2_Pin);


//		I2C_Write(&hi2c1, 0x50, 2, 1);
//		I2C_Read(&hi2c1, 0x50, &i2c_r, 1);

//		HAL_I2C_Mem_Write(&hi2c1, 0x50<<1,0x5, 1,(uint8_t*) &uptime_tick, 1, 300);


		osDelay(10);

/*--------------- ОБРАБОТАВАЕМ и ВЫВОД�?М С�?ГНАЛЫ НА П�?НЫ ---------------*/

		// ПАРС�?М СЛУЖЕБНЫЕ СТРОК�?
		ServiceStringPars((string_t *) &datastring, &active);

		// МАСС�?В ЗАДАЕТ СОСТОЯН�?Е РЕЛЕ, ВЗА�?В�?СОСТ�? ОТ ПОЛУЧЕННЫХ ДАННЫХ
		for(uint8_t i = 0; i < 4; i++){
			if(active.RELE[i])
				HAL_GPIO_WritePin(RELE_PORTS[i], RELE_PINS[i], GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(RELE_PORTS[i], RELE_PINS[i], GPIO_PIN_RESET);
		}

//		display_stat = active.DISP;
		// СОСТОЯН�?Е Д�?СПЛЕЯ ОТПРАВЛЯЕТСЯ В ЗАДАЧУ StartDisplayTask
//		xQueueSendToBack(myDispStatQueueHandle, &active.DISP, portMAX_DELAY);



		if(HAL_GetTick() - transmit_timer > transmit_time || transmit_timer > HAL_GetTick()){
/*------------------------- СОСТАВЛЯЕМ СТРОК�? --------------------------*/

			// ПРОБЕГАЕМ ВСЕ СТРОК�? КНОТРОЛЯ (Ц�?КЛ ТУТ Л�?ШН�?Й, НО ПУСТЬ ПОКА БУДЕТ)
			for(uint8_t i = start_ctrl_string; i < start_serv_string; i++){
				switch (i){

			/*- - - - - - - -  Строка_№2_(РЕЛЕ/DC) - - - - - - - -*/
				case ctrl_string_2:
					for(uint8_t j = 0; j < string_size; j++){
						datastring[i].buf[j] = '\0';
					}

					RB_count = 0;

					// ЗАПОЛНЯЕМ СТРОКУ, �?СПОЛЬЗУЯ ШАБЛОН
					for(uint8_t j = 0; j < strlen((const char *)&ctrl_template);j++){

						// ЕСЛ�? С�?МВОЛ ШАБЛОНА 'R', ЗАП�?СЫВАЕМ СОСТОЯН�?Е РЕЛЕ(RB_COUNT ПО СЧЕТУ)
						if(ctrl_template[j] == (uint8_t)'R'){
							datastring[i].buf[j] = ctrl_template[j];
							datastring[i].buf[j+1] = active.RELE[RB_count] + 48; 	// + 48 - ПЕРЕВОД�?Т С�?МВОЛ �?З INT В CHAR
							RB_count++;
						}

						// ЕСЛ�? НАХОД�?М КОМБ�?НАЦ�?Ю "DC", ЗАП�?СЫВАЕМ СОСТОЯН�?Е DC
						if(ctrl_template[j] == (uint8_t)'D' && ctrl_template[j+1] == (uint8_t)'C'){
							datastring[i].buf[j] = ctrl_template[j];
							datastring[i].buf[j+1] = ctrl_template[j+1];
							datastring[i].buf[j+2] = active.DC + 48;				// + 48 - ПЕРЕВОД�?Т С�?МВОЛ �?З INT В CHAR
						}
					}
					break;

			/*- - - - - - - - -  Строка_№3_(КНОПК�?) - - - - - - - -*/
				case ctrl_string_3:
					for(uint8_t j = 0; j < string_size; j++){
						datastring[i].buf[j] = '\0';
					}

					RB_count = 0;

					// ЗАПОЛНЯЕМ СТРОКУ, �?СПОЛЬЗУЯ ШАБЛОН
					for(uint8_t j = 0; j < strlen((const char *)&but_template);j++){

						// ЕСЛ�? С�?МВОЛ ШАБЛОНА 'B', ЗАП�?СЫВАЕМ СОСТОЯН�?Е КНОПКО ПРОКУРУТК�? �? ФЛЭШ
						if(but_template[j] == (uint8_t)'B'){
							datastring[i].buf[j] = but_template[j];
							datastring[i].buf[j+1] = but_buf[RB_count] + 48;		// + 48 - ПЕРЕВОД�?Т С�?МВОЛ �?З INT В CHAR
							RB_count++;
						}
						// ЕСЛ�? НАХОД�?М КОМБ�?НАЦ�?Ю "PB", ЗАП�?СЫВАЕМ СОСТОЯН�?Е КНОПК�? ВЫКЛЮЧЕН�?Я
						if(but_template[j] == (uint8_t)'P' && but_template[j+1] == (uint8_t)'B'){
							datastring[i].buf[j] = but_template[j];
							datastring[i].buf[j+1] = but_template[j+1];
							datastring[i].buf[j+2] = but_buf[3] + 48;		// + 48 - ПЕРЕВОД�?Т С�?МВОЛ �?З INT В CHAR
	//						RB_count++;
						}
					}
					break;

			/*- - - - - - - - Строка_№4_(ТЕМПЕРАТУРА) - - - - - - -*/
				case ctrl_string_4:
					for(uint8_t j = 0; j < string_size; j++){
						datastring[i].buf[j] = '\0';
					}
					adc_count = 0;
					adc_str_count = 0;

					// СЧ�?ТЫВАЕМ ПОКА ЧТО ДВА КАНАЛА АЦП
					while(adc_count < 5){

						// ЗАП�?СЫВАЕМ ЗНАК ТЕМПЕРАТУРЫ

						// ЕСЛ�? ОПРАШ�?ВАЕМ ДАТЧ�?К ТЕМПЕРАТУРЫ STM, СЧ�?ТАЕМ ЕГО ПО СПЕЦ�?АЛЬНОЙ
						// �? ПЕРЕВОД�?М В СТРОКУ
						// �?НАЧЕ ПРОСТО ВЫВОД�?М ЗНАЧЕН�?Е ТЕМПЕРАТУРЫ
						switch (adc_count){
						case 0:
							datastring[i].buf[adc_str_count] = (uint8_t)'T';
							adc_str_count++;

							ADC_mid = ADC_Mean(ADC_statistics[0], 5);
							itoa(TemperatureGetData(ADC_mid),(char *)&ADC_char, 10);
							break;

						case 1:
							datastring[i].buf[adc_str_count] = (uint8_t)'T';
							adc_str_count++;

							ADC_mid = ADC_Mean(ADC_statistics[adc_count], 5);

							itoa(NTC_getTemp(ADC_mid),(char *)&ADC_char, 10);
							break;

						case 2:
							datastring[i].buf[adc_str_count] = (uint8_t)'U';
							adc_str_count++;

							ADC_mid = ADC_Mean(ADC_statistics[adc_count], 5);
							ADC_real = ADC_to_Volt(ADC_mid) * 2;
							itoa(ADC_real,(char *)&ADC_char, 10);
							break;

						case 3:
							datastring[i].buf[adc_str_count] = (uint8_t)'U';
							adc_str_count++;

							ADC_mid = ADC_Mean(ADC_statistics[adc_count], 5);
							ADC_real = ADC_to_Volt(ADC_mid) * 2;
							itoa(ADC_real,(char *)&ADC_char, 10);
							break;

						case 4:
							datastring[i].buf[adc_str_count] = (uint8_t)'U';
							adc_str_count++;

							ADC_mid = ADC_Mean(ADC_statistics[adc_count], 5);
							ADC_real = ADC_to_Volt(ADC_mid) * 11;
							itoa(ADC_real,(char *)&ADC_char, 10);
							break;
						}


						// ЗАП�?СЫВАЕМ СТРОКУ ЗНАЧЕН�?Я АЦП/ТЕМПЕРАТУРЫ В СТРОКУ
						for(uint8_t j = 0; j < strlen((const char*)&ADC_char); j++){
							datastring[i].buf[adc_str_count] = ADC_char[j];
							adc_str_count++;
						}
						adc_count++;
					}
					break;

				/*- - - - - - - - - Строка_№5_(ОПТОПАРЫ) - - - - - - - -*/
					case ctrl_string_5:
						for(uint8_t j = 0; j < string_size; j++){
							datastring[i].buf[j] = '\0';
						}
						datastring[i].number = i;
						adc_count = 0;
						adc_str_count = 0;

						while(adc_str_count < 6){
							datastring[i].buf[adc_count] = (uint8_t)'O';
							adc_count++;
							datastring[i].buf[adc_count] = opt_buf[adc_str_count] + 48;
							adc_count++;
							adc_str_count++;
						}
						break;
				/*- - - - - - - - - Строка_№6_(UP_time) - - - - - - - -*/
					case ctrl_string_6:
						for(uint8_t j = 0; j < string_size; j++){
							datastring[i].buf[j] = '\0';
						}
						adc_count = 0;
						datastring[i].buf[adc_count] = (uint8_t)'U';
						adc_count++;
						datastring[i].buf[adc_count] = (uint8_t)'P';
						adc_count++;
						datastring[i].buf[adc_count] = (uint8_t)'T';
						adc_count++;

						itoa(uptime_tick,up_time_char,10);

						for(uint8_t j = 0; j < strlen(up_time_char); j++){
							datastring[i].buf[adc_count] = up_time_char[j];
							adc_count++;
						}
						break;
					/*- - - - - - - - - Строка_№8_(ADC) - - - - - - - -*/
					case ctrl_string_8:
						for(uint8_t j = 0; j < string_size; j++){
							datastring[i].buf[j] = '\0';
						}
						adc_count = 0;
						adc_str_count = 0;

						while(adc_count < 5){
							datastring[i].buf[adc_str_count] = 'A';
							adc_str_count++;

							ADC_mid = ADC_Mean(ADC_statistics[adc_count], 5);

							itoa(ADC_mid,(char *)&ADC_char, 10);

							// ЗАП�?СЫВАЕМ СТРОКУ ЗНАЧЕН�?Я АЦП/ТЕМПЕРАТУРЫ В СТРОКУ
							for(uint8_t j = 0; j < strlen((const char*)&ADC_char); j++){
								datastring[i].buf[adc_str_count] = ADC_char[j];
								adc_str_count++;
							}

							adc_count++;
						}
						break;
						// ЗАП�?СЫВАЕМ СТРОКУ ЗНАЧЕН�?Я АЦП В СТРОКУ
				}
			}

	/*-------------------- СОБ�?РАЕМ МАСС�?В ДЛЯ ОТПРАВК�? ----------------------*/

			for(uint32_t i = 0; i < ctrl_string_amnt*string_size; i++){
				sendBuf[i] = '\0';
			}

			sB_pointer = 0;

			// ВНАЧАЛЕ БЛОК КОДА ПЕРЕД Ц�?КЛОМ ЗАП�?СЫВАЕТ СТАРОТОВУЮ КОМБ�?НАЦ�?Ю В ОТПРАВЛЯЕМЫЙ БУФЕР
			sendBuf[sB_pointer] = 'P';
			sB_pointer++;
			sendBuf[sB_pointer] = 'S';
			sB_pointer++;
			sendBuf[sB_pointer] = 'D';
			sB_pointer++;
			sendBuf[sB_pointer] = ';';
			sB_pointer++;

			crc_count = 0;

			// Ц�?КЛЫ ЗАП�?СЫВАЕТ КАЖДУЮ СТРОКУ КОНТРОЛЯ В МАСС�?В �? ПР�?СВАЕВАЕТ СТРОКЕ НОВЫЙ НОМЕР
			for(uint8_t i = start_ctrl_string; i < start_serv_string; i++){
				sB_counter = 0;

				// ЗАП�?СЫВАЕМ НОМЕР, Т.К СОСТО�?Т �?З 2-Х Ц�?ФР, А В МОЕМ СЛУЧАЕ
				// �?СПОЛЬЗУЮТСЯ ОДНОЗНАЧНЫЕ Ч�?СЛА, ЗАП�?СЫВАЕМ ПЕРВЫМ 0
				sendBuf[sB_pointer] = '0';

				// ЭТОТ СЧЕТЧ�?К ОТСЧ�?ТЫВАЕТ КОЛ�?ЧЕСТВО С�?МВОЛОВ ДЛЯ РАСЧЕТА CRC
				crc_count++;

				sB_pointer++;

				// ЗАП�?СЫВАЕМ НОМЕР СТРОК�?, НАЧ�?НАЕТСЯ С 1 �? ДО 4
				sendBuf[sB_pointer] = (i - start_ctrl_string + 1) + 48; 		// + 48 - ПЕРЕВОД�?Т �?З INT В CHAR
				crc_count++;
				sB_pointer++;

				// ПОСЛЕ НОМЕРА СТРОК�? ЗАП�?ШЕМ ';'
				sendBuf[sB_pointer] = ';';
				crc_count++;
				sB_pointer++;

				// В ДАННОМ Ц�?КЛЕ ЗАП�?СЫВАЕТСЯ СОДЕРЖ�?МОЕ СТРОК�?, ПОКА СТРОКА НЕ ЗАКОНЧ�?ТСЯ
				while(datastring[i].buf[sB_counter] != '\0'){
					sendBuf[sB_pointer] = datastring[i].buf[sB_counter];
					crc_count++;
					sB_counter++;
					sB_pointer++;
				}

				// ОТДЕЛЯЕМ СТРКОУ ЗНАКОМ ';'
				sendBuf[sB_pointer] = ';';
				crc_count++;
				sB_pointer++;
			}

			// КОГДА ВСЕ СТРОК�? ПЕРЕП�?САНЫ, ПОСЛЕ Н�?Х ЗАП�?СЫВАЕМ СТРОКУ "CRC"
			sendBuf[sB_pointer] = 'C';
			sB_pointer++;
			sendBuf[sB_pointer] = 'R';
			sB_pointer++;
			sendBuf[sB_pointer] = 'C';
			sB_pointer++;
			sendBuf[sB_pointer] = ';';
			sB_pointer++;

			// РАСЧ�?ТЫВАЕМ CRC32
			// *примечание_ДЛЯ РАСЧЕТА CRC32 В ДАННОМ СЛУЧАЕ НЕ �?СПОЛЬЗУЕТСЯ ОТДЕЛЬНЫЙ БУФЕР
			// 	ДАННЫЕ БЕРУТСЯ �?З ОТПРАВЛЯЕМОГО БУФЕРА, НО НЕ СНАЧАЛА, А ПРОПУСКАЯ 4 С�?МВОЛА "PSD;"
			//	�? ЗАКАНЧ�?ВАЯ ДО "СRC", Т.К �?СПОЛЬЗУЕТСЯ СЧЕТЧ�?К С�?МВОЛОВ crc_count
			crc32 = Crc32((const unsigned char *) &sendBuf + 4, crc_count);

			// ПЕРЕВОД�?М ЗНАЧЕН�?Е СRC32 В СТРОКУ (В HEX)
			itoa(crc32, (char *) &crc_hex_buf, 16);

			// ЗАП�?СЫВАЕМ ПОС�?МВОЛЬНО СТРОКОВОЕ ЗНАЧЕН�?Е CRC32
			for(uint8_t i = 0; i < 8; i++){

				// Т.К itoa ПЕРЕВОД�?Т В HEX �?СПОЛЬЗУЯ МАЛЕНЬК�?Е БУКВЫ, МЕНЯЕМ �?Х НА БОЛЬШ�?Е. см_ФУНКУЮ BigLatter()
				if(8 - strlen((char*)&crc_hex_buf) > i){
					sendBuf[sB_pointer] = '0';
				}
				else{
					sendBuf[sB_pointer] = BigLatter(crc_hex_buf[i]);
				}
				sB_pointer++;
			}

			// ЗАВЕРШАЕМ ПАКЕТ ';'
			sendBuf[sB_pointer] = ';';
			sB_pointer++;

			// ОТПРАВЛЯЕМ ПАКЕТ

			transmit_timer = HAL_GetTick();
			HAL_UART_Transmit(&huart3,(uint8_t *) &sendBuf, sB_pointer, 0xFFFF); // ОТПРАВЛЯЕМ МАСС�?В
			ADC_summ_count = 0;
		}

		if(HAL_GetTick() - flash_transmit_timer > transmit_time * 3 ||  flash_transmit_timer > HAL_GetTick()){
			flash_on = 0;

		}

/*------------ ОБРАБАТЫВАЕМ КНОПК�? ВЫКЛЮЧЕН�?Я �? ЗАП�?С�? FLASH --------------*/
		if(but_buf[3] && (HAL_GetTick() - power_butthold > OFF_delay || power_butthold > power_butthold)){
			PowerOFF(&active.DISP);												// КНОПКА ВЫКЛЮЧЕН�?Я
		}

		osDelay(50);
	}
  /* USER CODE END StartControlTask */
}

/* USER CODE BEGIN Header_StartLEDsTask */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 														   *
 *				 	  ЗАДАЧА СВЕТОД�?ОДОВ		  		   *
 *														   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* USER CODE END Header_StartLEDsTask */
void StartLEDsTask(void const * argument)
{
  /* USER CODE BEGIN StartLEDsTask */
	RGB_status RGB[3];											// RGB НА Л�?ЦЕВОЙ ПАНЕЛ�?

	// УСТАНАВЛ�?ВАЕМ П�?НЫ �? ПОРТЫ СВЕТОД�?ОДОВ
	//    *�?СПОЛЬЗУЕТСЯ НЕ const, ЧТО НЕ ЕСТЬ ХОРОШО, НО ПРОБЛЕМ БЫТЬ НЕ ДОЛЖНО
	RGB[0].RED.LED_port = RED_232_GPIO_Port;
	RGB[0].RED.LED_pin = RED_232_Pin;
	RGB[0].GREEN.LED_port = GREEN_232_GPIO_Port;
	RGB[0].GREEN.LED_pin = GREEN_232_Pin;
	RGB[0].BLUE.LED_port = BLUE_232_GPIO_Port;
	RGB[0].BLUE.LED_pin = BLUE_232_Pin;

	RGB[1].RED.LED_port = RED_422_GPIO_Port;
	RGB[1].RED.LED_pin = RED_422_Pin;
	RGB[1].GREEN.LED_port = GREEN_422_GPIO_Port;
	RGB[1].GREEN.LED_pin = GREEN_422_Pin;
	RGB[1].BLUE.LED_port = BLUE_422_GPIO_Port;
	RGB[1].BLUE.LED_pin = BLUE_422_Pin;

	RGB[2].RED.LED_port = RED_485_GPIO_Port;
	RGB[2].RED.LED_pin = RED_485_Pin;
	RGB[2].GREEN.LED_port = GREEN_485_GPIO_Port;
	RGB[2].GREEN.LED_pin = GREEN_485_Pin;
	RGB[2].BLUE.LED_port = BLUE_485_GPIO_Port;
	RGB[2].BLUE.LED_pin = BLUE_485_Pin;

	// УКАЗАТЕЛЬ НА ОТДЕЛЬНЫЙ СВЕТОД�?ОД С ЕГО ДАННЫМ�? (РЕЖ�?МОМ М�?ГАН�?Я, П�?НОМ)
	led_status *oneLED;

	// ЗАДЕРЖКА ПР�? ВКЛЮЧЕН�?�? ПОКА �?Н�?Ц�?АЛ�?З�?РУЕТСЯ Д�?СПЛЕЙ
	osDelay(500);

	for(;;)
	{
		WDG_TackArr[3] = 1;

		// ПАРС�?М СТРОКУ СВЕТОД�?ОДОВ
		LEDStringPars((string_t *)&datastring,(RGB_status *) &RGB);



		// ОБРАБАТЫВАЕМ КАЖДЫЙ RGB (3)
		for(uint8_t i = 0; i < 3; i++){

			// ОБРАБАТЫВАЕМ КАЖДЫЙ СВЕТОД�?ОД В RGB
			for(uint8_t j = 0; j < 3; j++){

				// ВЫБ�?РАЕМ СВЕТОД�?ОД УКАЗЫВАЯ НА НУЖНЫЙ
				switch (j){
				case 0: oneLED = &RGB[i].RED;   break;
				case 1: oneLED = &RGB[i].BLUE;  break;
				case 2: oneLED = &RGB[i].GREEN; break;
				}

				// УСТАНАВЛ�?ВАЕМ РЕЖ�?М М�?ГАН�?Я СВЕТОД�?ОДОМ
				//    *�?СПОЛЬЗУЕТ uint32_t ДЛЯ ХРАНЕН�?Я ПЕРЕМЕННОЙ ВРЕМЕН�? ДЛЯ КАЖДОГО СВЕТОД�?ОДА
				//     МОЖЕТ БЫТЬ ОПТ�?М�?З�?РОВАНО ПО ПАМЯТ�? ДЛЯ КАЖДОГО РЕЖ�?МА, ЧТО С�?ЛЬНО УМЕНЬШ�?Т ПАМЯТЬ
				switch (oneLED->LED_status)
				{
				case (uint8_t)'0': LED_control(oneLED,0,1); 	break;
				case (uint8_t)'1': LED_control(oneLED,1,0); 	break;
				case (uint8_t)'2': LED_control(oneLED,50,100);  break;
				case (uint8_t)'3': LED_control(oneLED,200,200); break;
				case (uint8_t)'4': LED_control(oneLED,50,500);  break;
				}
			}
		}

		osDelay(10);
	}
  /* USER CODE END StartLEDsTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/*
 * ОБРАБОТЧ�?К ПРЕРЫВАН�?Й ТАЙМЕРА UPTIME
 */
void UPTIME_IRQHandler(){
	uptime_tick++;
}

void WDG_tim13_Handler(){


	if(WDG_TackArr[0] && WDG_TackArr[1] && WDG_TackArr[2] && WDG_TackArr[3]){
		HAL_GPIO_WritePin(SYM_LED_G_GPIO_Port, SYM_LED_G_Pin, GPIO_PIN_SET);
		WDG_TackArr[0] = 0;
		WDG_TackArr[1] = 0;
		WDG_TackArr[2] = 0;
		WDG_TackArr[3] = 0;
		HAL_IWDG_Refresh(&hiwdg);
	}
	else HAL_GPIO_WritePin(SYM_LED_B_GPIO_Port, SYM_LED_B_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SYM_LED_G_GPIO_Port, SYM_LED_G_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SYM_LED_B_GPIO_Port, SYM_LED_B_Pin, GPIO_PIN_RESET);
}

void Send32toMem(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, size_t MemAddSize, uint32_t pData, uint32_t Timeout){
	uint8_t bytes[4] = {'\0'};

	for(uint8_t i = 0; i < 4; i++){
		bytes[i] = (uint8_t)pData;
		pData >>= 8;
	}

	HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, (uint8_t*) &bytes, 4, Timeout);

}

uint32_t Read32toMem(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, size_t MemAddSize, uint32_t Timeout){
	uint8_t bytes[4] = {'\0'};
	uint32_t data = 0;
	HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, (uint8_t*) &bytes, 4, Timeout);

	for(uint8_t i = 0; i < 4; i++){
		data += (uint32_t) bytes[i] << (8 * i);
	}

	return data;
}

uint16_t ADC_Mean(uint16_t* ADC_arr, uint8_t len){
	uint16_t ADC_mean = 0;
	for(uint8_t i = 0; i < len; i++){
		ADC_mean += ADC_arr[i];
	}

	return ADC_mean/len;
}


uint16_t ADC_to_Volt(uint16_t adc_val){
	return (adc_val*330)/4095;
}

void ADC_Read12vHandler(){
	UBaseType_t uxSavedInterruptStatus;

	// ПРОВЕРЯЕМ УСЛОВ�?Е, ЧТО П�?ТАН�?Е 12 В
	// ЕСЛ�? ДА, ТО ПРОДОЛЖАЕМ
	// ЕСЛ�? НЕТ, ТО ВКЛЮЧАЕМ ЗУМЕРЫ В Ц�?КЛЕ

	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR ();

	if(ADC_val[4] < Low12vThreshold || ADC_val[4] > High12vThreshold){
		ADC_reset_count++;

	}

	if(ADC_reset_count > ADC_12v_reset_val){
		HAL_GPIO_TogglePin(SYM_LED_G_GPIO_Port, SYM_LED_G_Pin);

		HAL_GPIO_WritePin(STM32_Relay_mmn_GPIO_Port, STM32_Relay_mmn_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(STM32_Relay_mother_GPIO_Port, STM32_Relay_mother_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(STM32_RS_DC_DC_GPIO_Port, STM32_RS_DC_DC_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);

		ADC_reset_count = 0;

		while(ADC_reset_count < ADC_12v_reset_val){
			if(ADC_val[4] > Low12vThreshold || ADC_val[4] < High12vThreshold){
				ADC_reset_count++;
			}
		}
	}

	HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}



/*
 * ОБРАБОТЧ�?К ПРЕРЫВАН�?Й ANALOG WATHDOG
 * ПР�? ОТЛАДКЕ ПЛАТЫ БУДЕТ ПЕРЕП�?САНО, Т.К
 * В ДАННОМ ВАР�?АНТЕ ЗАТРУДНЕНО ОТКЛЮЧЕН�?Е Д�?СПЛЕЯ
 */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc){
	UBaseType_t uxSavedInterruptStatus;

	// ПРОВЕРЯЕМ УСЛОВ�?Е, ЧТО П�?ТАН�?Е 12 В
	// ЕСЛ�? ДА, ТО ПРОДОЛЖАЕМ
	// ЕСЛ�? НЕТ, ТО ВКЛЮЧАЕМ ЗУМЕРЫ В Ц�?КЛЕ
	// П�?ТАН�?Е РАСЧ�?ТАНО �?З СХЕМЫ : 				12V/(5.1К + 5.1К + 1К)*1К = 0,29464V
	// ПЕРЕВОД�?М В ЗНАЧЕН�?Е 12 РАЗРЯДНОГО АЦП		0,29464*4095/3.3 = 1329,545
	// БЕРЕМ +-90 (+-0,8V НА ВХОД)
	// Ц�?КЛ ЗАЩ�?ЩЕН ФУНКЦ�?ЯМ�? FREERTOS
	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR ();

	while(ADC_val[4] < Low12vThreshold || ADC_val[4] > High12vThreshold){
		HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(STM32_Relay_mmn_GPIO_Port, STM32_Relay_mmn_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(STM32_Relay_mother_GPIO_Port, STM32_Relay_mother_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(STM32_RS_DC_DC_GPIO_Port, STM32_RS_DC_DC_Pin, GPIO_PIN_RESET);

		// ОБНОВЛЯЕМ WATHDOG
		HAL_IWDG_Refresh(&hiwdg);
	}
	HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}


void I2C_Write(I2C_HandleTypeDef *hi, uint8_t ADDR, uint8_t *sent_i2c, size_t size){

	HAL_I2C_Master_Transmit(hi, (uint16_t)ADDR, sent_i2c, size, 1000);
//		if (HAL_I2C_GetError(hi) != HAL_I2C_ERROR_AF)
//
//			{
//				//место_для_ошибки
//			}
//	}
}

void I2C_Read(I2C_HandleTypeDef *hi, uint8_t ADDR, uint8_t *sent_i2c, size_t size){

	HAL_I2C_Master_Receive(hi, (uint16_t)ADDR, sent_i2c, size, 1000);
//
//		if (HAL_I2C_GetError(hi) != HAL_I2C_ERROR_AF){
//
//		}
//
//	}
}

/*
 * ФУНКЦ�?Я ОТКЛЮЧЕН�?Я С�?СТЕМЫ
 * ОТКЛЮЧАЕТ
 */

void PowerON(u8g2_t* u8g2){

//	uint32_t check12v = 0;
//	UBaseType_t uxSavedInterruptStatus;

	// ПРОВЕРЯЕМ УСЛОВ�?Е, ЧТО П�?ТАН�?Е 12 В
	// ЕСЛ�? ДА, ТО ПРОДОЛЖАЕМ
	// ЕСЛ�? НЕТ, ТО ВКЛЮЧАЕМ ЗУМЕРЫ В Ц�?КЛЕ
	// П�?ТАН�?Е РАСЧ�?ТАНО �?З СХЕМЫ : 				12V/(5.1К + 5.1К + 1К)*1К = 1.07142V
	// ПЕРЕВОД�?М В ЗНАЧЕН�?Е 12 РАЗРЯДНОГО АЦП		1.07142*4095/3.3 = 1329,545
	// БЕРЕМ +-90 (+-0,8V НА ВХОД)
	// Ц�?КЛ ЗАЩ�?ЩЕН ФУНКЦ�?ЯМ�? FREERTOS

	// НА БУДУЩЕЕ
//	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR ();
//
//	for(uint8_t i = 0; i < 10; i++){
//		HAL_Delay(10);
//
//		check12v += ADC_val[4];
//	}
//
//	if(check12v/10 > High12vThreshold || check12v/10 < Low12vThreshold){
//			while(1){
//				HAL_GPIO_TogglePin(SYM_LED_B_GPIO_Port, SYM_LED_B_Pin);
//				HAL_Delay(100);
//				HAL_IWDG_Refresh(&hiwdg);
//				HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
//			}
//		}
//
//	taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);


	// ЗАЖ�?ГАЕМ С�?Н�?Й ЦВЕТ RGB ПР�? СТАРТЕ
	HAL_GPIO_WritePin(BLUE_232_GPIO_Port, BLUE_232_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BLUE_422_GPIO_Port, BLUE_422_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BLUE_485_GPIO_Port, BLUE_485_Pin, GPIO_PIN_SET);

	// ВКЛЮЧАЕМ РЕЛЕ
	HAL_GPIO_WritePin(STM32_Relay_mmn_GPIO_Port, STM32_Relay_mmn_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(STM32_Relay_mother_GPIO_Port, STM32_Relay_mother_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(STM32_RS_DC_DC_GPIO_Port, STM32_RS_DC_DC_Pin, GPIO_PIN_SET);

	// ВКЛЮЧАЕМ ЗУМЕРЫ
	for(uint8_t i = 0; i < 4; i++){
		HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
		osDelay(30);
		HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
		osDelay(30);
	}

	// ЗАЖ�?ГАЕМ Д�?СПЛЕЙ
	u8g2_DrawBox(u8g2, 0, 0, 254, 64);
	u8g2_SendBuffer(u8g2);

	// ЖДЕМ
	HAL_Delay(start_pause);

	// ТУШ�?М Д�?СПЛЕЙ
	u8g2_ClearDisplay(u8g2);

	// ТУШ�?М СВЕТОД�?ОДЫ
	HAL_GPIO_WritePin(BLUE_232_GPIO_Port, BLUE_232_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BLUE_422_GPIO_Port, BLUE_422_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BLUE_485_GPIO_Port, BLUE_485_Pin, GPIO_PIN_RESET);

	// ВЫКЛЮЧАЕМ ЗУМЕРЫ
	HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
}

void PowerOFF(){

	uint32_t last_time = 0;

	for(uint8_t i = 0; i < 4; i++){
		HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
		osDelay(30);
		HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
		osDelay(30);
	}

	display_stat = 0;


	while(HAL_GetTick()-last_time < PowerOFF_delay || last_time > HAL_GetTick()){
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(500);
	}

//	vTaskDelete(myDisplayTaskHandle);

	vTaskDelete(myDataTaskHandle);
	vTaskDelete(myLEDsTaskHandle);

	HAL_GPIO_WritePin(STM32_RS_DC_DC_GPIO_Port, STM32_RS_DC_DC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STM32_Relay_mother_GPIO_Port, STM32_Relay_mother_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STM32_Relay_mmn_GPIO_Port, STM32_Relay_mmn_Pin, GPIO_PIN_RESET);

	LEDs_OFF();

	SleepMode();

}

void SleepMode(){
	uint8_t switch_off = 0;
	uint8_t power_butmem = 0;									// ПЕРЕМЕННАЯ ДЛЯ ОТСЧЕТА ВРЕМЕН�? НАЖАТ�?Я КНОПК�?
	uint32_t power_butthold = 0;
	uint16_t PWM_val = 0;
//	uint8_t direct = 1;
	double angl = 0;

	while(1){
		HAL_IWDG_Refresh(&hiwdg);

		for(uint16_t i = 0; i < 3000; i++){
			if(i < PWM_val){
				HAL_GPIO_WritePin(STM32_BUTTON_LED_1_GPIO_Port, STM32_BUTTON_LED_1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STM32_BUTTON_LED_2_GPIO_Port, STM32_BUTTON_LED_2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STM32_BUTTON_LED_1_GPIO_Port, STM32_BUTTON_LED_3_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(STM32_BUTTON_LED_POWER_GPIO_Port, STM32_BUTTON_LED_POWER_Pin, GPIO_PIN_SET);

				HAL_GPIO_WritePin(BLUE_422_GPIO_Port, BLUE_422_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(BLUE_485_GPIO_Port, BLUE_485_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(BLUE_232_GPIO_Port, BLUE_232_Pin, GPIO_PIN_SET);
			}
			else{
				HAL_GPIO_WritePin(STM32_BUTTON_LED_1_GPIO_Port, STM32_BUTTON_LED_1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STM32_BUTTON_LED_2_GPIO_Port, STM32_BUTTON_LED_2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STM32_BUTTON_LED_1_GPIO_Port, STM32_BUTTON_LED_3_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(STM32_BUTTON_LED_POWER_GPIO_Port, STM32_BUTTON_LED_POWER_Pin, GPIO_PIN_RESET);

				HAL_GPIO_WritePin(BLUE_422_GPIO_Port, BLUE_422_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(BLUE_485_GPIO_Port, BLUE_485_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(BLUE_232_GPIO_Port, BLUE_232_Pin, GPIO_PIN_RESET);
			}
		}


		angl = (angl > 3.141) ? 0 : angl + 0.004;
		PWM_val = (sin(angl)) * 1000;


		PowerButtonHandler(&power_butthold, &power_butmem, &switch_off, 500);

		while(switch_off){

		}
	}
}

void FlashWriteStart(){

}
/*
 * ФУНКЦ�?Я ПЕРЕВОД�?Т МАЛЕНЬК�?Е БУКВЫ ШЕСТНАДЦАТ�?Р�?ЧНОГО НАБОРА
 * В БОЛЬШ�?Е. ЕСЛ�? НА ВХОД ПР�?ШЛА Ц�?ФРА, ТО ВОЗВРАЩАЕТСЯ С�?МВОЛ Ц�?ФРЫ
 */
char BigLatter(char smalllatter){
	switch(smalllatter){
	case 'a': return 'A';
	case 'b': return 'B';
	case 'c': return 'C';
	case 'd': return 'D';
	case 'e': return 'E';
	case 'f': return 'F';
	default: return smalllatter;
	}
}

/*
 * ФУНКЦ�?Я РАСЧЕТА ТЕМПЕРАТУРЫ ВСТРОЕНОГО ДАТЧ�?КА STM.
 * ПР�?Н�?МАЕТ ДАННЫЕ АЦП С ТЕМПЕРЕАТУРНОГО КАНАЛА
 * ВОЗВРАЩАЕТ ТЕМПЕРАТУРУ В ГРАДУСАХ ЦЕЛЬС�?Я
 */
uint8_t TemperatureGetData(uint16_t ADCResult){
	int32_t temperature; /* will contain the temperature in degrees Celsius */

	temperature = (((int32_t) ADCResult * VDD_APPLI / VDD_CALIB) - (int32_t) *TEMP30_CAL_ADDR );
	temperature = temperature * (int32_t)(110 - 30);
	temperature = temperature / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);

    return (uint8_t) temperature + 25;
}

/*
 * КЛАДЕТ ТЕКСТ ОШ�?БК�? В 1-Ю СТРОКУ КОНТРОЛЬНЫХ СТРОК
 */
void PutERROR(string_t *error_string, const char *error_tekst){

	error_string[start_ctrl_string].number = start_ctrl_string;
	// Ч�?СТ�?М СТРОКУ
	for(uint8_t i = 0; i < string_size; i++){
		error_string[start_ctrl_string].buf[i] = '\0';
	}

	// ЗАПОЛНЯЕМ СТРОКУ
	for(uint8_t i = 0; i < strlen(error_tekst); i++){
		error_string[start_ctrl_string].buf[i] = error_tekst[i];
	}
}

void PowerButtonHandler(uint32_t *butthold,uint8_t *butmem, uint8_t *switch_off, uint16_t off_delay){

	// ЕСЛ�? КНОПКА НАЖАТА
	if(HAL_GPIO_ReadPin(STM32_BUTTON_POWER_GPIO_Port, STM32_BUTTON_POWER_Pin)){

		HAL_GPIO_WritePin(STM32_BUTTON_LED_POWER_GPIO_Port, STM32_BUTTON_LED_POWER_Pin, GPIO_PIN_RESET);

		// ЕСЛ�? КНОПКА ДО ЭТОГО НЕ БЫЛА НАЖАТА
		if(!(*butmem &  STM32_BUTTON_POWER_Pin)){

			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(Button_Zummer);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);

			// НАЧ�?НАЕТ ОТСЧЕТ ВРЕМЕН�?
			*butthold = HAL_GetTick();
		}
		// ЕСЛ�? ПРОШЛО ОПРЕДЕЛЕННОЕ ВРЕМЯ �? КНОПКА ВСЕ ЕЩЕ НАЖАТА, ТО ПОДН�?МАЕМ ФЛАГ
		if((HAL_GetTick() - *butthold) > off_delay){

			*switch_off = 1;

		}
	}

	*butmem = STM32_BUTTON_POWER_GPIO_Port->IDR;		// ПАМЯТЬ КНОПК�?
}



/*
 * ФУНКЦ�?Я ОБРАБОТК�? КНОПОК
 */
void ScrollingButtonHandler(uint8_t *cursor, uint32_t *butthold,uint8_t *butmem){

	// ЕСЛ�? КНОПКА НАЖАТА �? (НЕ БЫЛА ЕЩЕ НАЖАТА, �?Л�? ПРОШЛО МНОГО ВРЕМЕН�?)
	// УСЛОВ�?Е РАБОТАЕТ НА ОД�?НОЧНЫЕ НАЖАТ�?Я, НО ЕСЛ�? ПРОШЛО ОПРЕДЕЛЕННОЕ ВРЕМЯ,
	// ПАМЯТЬ НАЖАТ�?Я КНОПК�? НЕ УЧ�?ТЫВАЕТСЯ
	if(HAL_GPIO_ReadPin(STM32_BUTTON_1_GPIO_Port, STM32_BUTTON_1_Pin)){

		HAL_GPIO_WritePin(STM32_BUTTON_LED_1_GPIO_Port, STM32_BUTTON_LED_1_Pin, GPIO_PIN_RESET);

		if(!(*butmem & STM32_BUTTON_1_Pin) || (HAL_GetTick() - *butthold > scroll_delay)){

			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(Button_Zummer);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);

			*cursor = (*cursor < (160 - 4)) ? *cursor + 1 : *cursor;
			osDelay(1000/scroll_speed);
			uptime = uptime_tick;
			if(!(*butmem & STM32_BUTTON_1_Pin))
				*butthold = HAL_GetTick();
		}
	}
	else {
		HAL_GPIO_WritePin(STM32_BUTTON_LED_1_GPIO_Port, STM32_BUTTON_LED_1_Pin, GPIO_PIN_SET);
	}

	// 2 АНАЛОГ�?ЧНО
	if(HAL_GPIO_ReadPin(STM32_BUTTON_2_GPIO_Port, STM32_BUTTON_2_Pin)){

		HAL_GPIO_WritePin(STM32_BUTTON_LED_2_GPIO_Port, STM32_BUTTON_LED_2_Pin, GPIO_PIN_RESET);

		if(!(*butmem & STM32_BUTTON_2_Pin) || ((HAL_GetTick() - *butthold) > scroll_delay)){

			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(Button_Zummer);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);

			*cursor = (*cursor > 0) ? *cursor - 1 : 170;
			osDelay(1000/scroll_speed);
			uptime = uptime_tick;

			if(!(*butmem & STM32_BUTTON_2_Pin))
				*butthold = HAL_GetTick();
			}

	}
	else HAL_GPIO_WritePin(STM32_BUTTON_LED_2_GPIO_Port, STM32_BUTTON_LED_2_Pin, GPIO_PIN_SET);

	//3
	if(HAL_GPIO_ReadPin(STM32_BUTTON_3_GPIO_Port, STM32_BUTTON_3_Pin)){

			HAL_GPIO_WritePin(STM32_BUTTON_LED_3_GPIO_Port, STM32_BUTTON_LED_3_Pin, GPIO_PIN_RESET);

			if(!(*butmem & STM32_BUTTON_3_Pin) || ((HAL_GetTick() - *butthold) > scroll_delay)){

				HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
				osDelay(Button_Zummer);
				HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);

				uptime = uptime_tick;
				if(!(*butmem & STM32_BUTTON_3_Pin))
					*butthold = HAL_GetTick();
				}

		}
		else HAL_GPIO_WritePin(STM32_BUTTON_LED_3_GPIO_Port, STM32_BUTTON_LED_3_Pin, GPIO_PIN_SET);


	*butmem = STM32_BUTTON_LED_1_GPIO_Port->IDR;						 // т.к_порт_кнопок_один
}


void ServiceModeButtonHandler(uint8_t *mem, uint8_t *mem2, uint32_t *hold, uint16_t service_delay){

	if(HAL_GPIO_ReadPin(STM32_BUTTON_1_GPIO_Port, STM32_BUTTON_1_Pin)
			&& HAL_GPIO_ReadPin(STM32_BUTTON_2_GPIO_Port, STM32_BUTTON_2_Pin)){

		// ЕСЛ�? КНОПКА ДО ЭТОГО НЕ БЫЛА НАЖАТА
		if(!((*mem & STM32_BUTTON_1_Pin) && (*mem & STM32_BUTTON_2_Pin))){

			// НАЧ�?НАЕТ ОТСЧЕТ ВРЕМЕН�?
			*hold = HAL_GetTick();
		}
		// ЕСЛ�? ПРОШЛО ОПРЕДЕЛЕННОЕ ВРЕМЯ �? КНОПКА ВСЕ ЕЩЕ НАЖАТА, ТО ПОДН�?МАЕМ ФЛАГ
		if(((HAL_GetTick() - *hold) > service_delay) && *mem2 == 0){
			*mem2 = 1;
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(200);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);

			service_mode = (service_mode == 0) ? 1 : 0;

			datastring[0].status = service_mode;

			datastring[serv_string_1].status = service_mode;
			datastring[serv_string_2].status = service_mode;
			datastring[ctrl_string_1].status = service_mode;
			datastring[ctrl_string_2].status = service_mode;
			datastring[ctrl_string_3].status = service_mode;
			datastring[ctrl_string_4].status = service_mode;
			datastring[ctrl_string_5].status = service_mode;
			datastring[ctrl_string_6].status = service_mode;
			datastring[ctrl_string_7].status = service_mode;
			datastring[ctrl_string_8].status = service_mode;
			datastring[ctrl_string_9].status = service_mode;
			datastring[ctrl_string_10].status = service_mode;
		}
	}
	else{
		*mem2 = 0;
	}

	*mem = STM32_BUTTON_LED_1_GPIO_Port->IDR; //ошибка
}

void ConnHandler(uint8_t *mem1, uint8_t *mem2){

	if(HAL_GPIO_ReadPin(STM32_Conn_1_GPIO_Port, STM32_Conn_1_Pin)){

		if(!(*mem1)){
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(Button_Zummer);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
		}
	}

	if(!HAL_GPIO_ReadPin(STM32_Conn_1_GPIO_Port, STM32_Conn_1_Pin)){

		if(*mem1){
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(Button_Zummer);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
		}
	}



	if(HAL_GPIO_ReadPin(STM32_Conn_2_GPIO_Port, STM32_Conn_2_Pin)){

		if(!(*mem2)){
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(Button_Zummer);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
		}
	}

	if(!HAL_GPIO_ReadPin(STM32_Conn_2_GPIO_Port, STM32_Conn_2_Pin)){

		if(*mem2){
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_SET);
			osDelay(Button_Zummer);
			HAL_GPIO_WritePin(STM32_ZUMMER_GPIO_Port, STM32_ZUMMER_Pin, GPIO_PIN_RESET);
		}
	}

	*mem1 = HAL_GPIO_ReadPin(STM32_Conn_1_GPIO_Port, STM32_Conn_1_Pin);		// ПАМЯТЬ КНОПК�?
	*mem2 = HAL_GPIO_ReadPin(STM32_Conn_2_GPIO_Port, STM32_Conn_2_Pin);
}

/*
 * ФУНКЦ�?Я РАСЧЕТ CRC32. КЛАДЕТ ВСЕ С�?МВОЛЫ ПОСЛЕ "PSD;"
 * �? ДО "CRC" В МАСС�?В crc32_buf, ПОСЛЕ ЧЕГО СЧ�?ТАЕТ КОНТРОЛЬНУЮ СУММУ.
 * ВОЗВРАЩАЕТ 1 ЕСЛ�? КОНТРОЛЬНЫЕ СУММЫ СОШЛ�?СЬ
 * ВОЗВРАЩАЕТ 0 ЕСЛ�? СУММА НЕ СОШЛАСЬ �?Л�? ПАКЕТ ПОВРЕЖДЕН
 */
uint8_t CRC32_Status(uint8_t *buf, size_t buf_size, uint16_t pointer, buffer_t *crc32_buf){

	uint8_t crc_in_buf[8];												// С�?МВОЛЬНЫЙ БУФЕР ВХОДНОГО ЗНАЧЕН�?Я CRC32
	uint8_t crc_string_count = 0;										// СЧЕТЧ�?К
	uint32_t crc_in = 0;												// ВХОДНОЕ ЗНАЧЕН�?Е CRC32
	uint32_t crc_out = 0;												// РАСЧ�?ТАНОЕ ЗНАЧЕН�?Е СRC32
	uint16_t crc_pointer = pointer;										// УКАЗАТЕЛЬ ДЛЯ НАБОРА МАСС�?ВА CRC32

	uint16_t crc_count = string_pack_amount * string_size;				// МАКС�?МАЛЬНОЕ КОЛ�?ЧЕСТВО С�?МВОЛОВ В ПАКЕТЕ

	// ОБНУЛЯЕМ ХВОСТ БУФЕРА
	crc32_buf->tail = 0;

	// ПОКА БУФЕР НЕ ЗАКОНЧ�?ТСЯ
	while(crc_count){

		// ЕСЛ�? НАЙДЕНА СТАРТОВАЯ КОМБ�?НАЦ�?Я, ЗНАЧ�?Т ЧТО-ТО НЕ ТАК
		if(FindString((uint8_t *)buf, buf_size, &crc_pointer, "PDS", 3)){
			//место_для_вашей_ошибки
			return 0;
		}

		// ЕСЛ�? НАЙДЕН "CRC", ЗНАЧ�?Т МАСС�?В НАБРАН. ПР�?СТУПАЕМ К РАСЧЕТУ
		if(FindString((uint8_t *)buf, buf_size, &crc_pointer, "CRC", 3)){

			// ПРОПУСКАЕМ ';'
			PassSym((uint8_t *)&buf, buf_size, &crc_pointer, 1);

			// РАСЧ�?ТЫВАЕМ CRC32 �?З МАСС�?ВА
			crc_out = Crc32((const unsigned char*)crc32_buf, crc32_buf->tail);
			crc_in = 0;

			if(FindString(buf, buf_size, &crc_pointer, "NAN", 3)){
				PutERROR((string_t *)&datastring,"CRC_NAN");
				return 1;
			}

			// ПАРС�?М ЗНАЧЕН�?Е CRC32 �?З ПАКЕТА
			while(buf[crc_pointer] != (uint8_t)';' && (crc_string_count < 8)){
				crc_in_buf[crc_string_count] = buf[crc_pointer];

				// ПЕРЕВОД�?М ПОЛУЧЕНУЮ СТРОКУ В DEC
				crc_in = HexToDec((char *)&crc_in_buf,8);

				// ВЫВОЖУ СRC В 0-Ю СТРКОУ ДЛЯ ОТЛАДК�?
//				datastring[ctrl_string_8].buf[crc_string_count] = buf[crc_pointer];
//				datastring[ctrl_string_8].number = ctrl_string_8;

				PassSym((uint8_t *)&buf, buf_size, &crc_pointer, 1);
				crc_string_count++;
			}

			// СРАВН�?ВАЕМ ЗНАЧЕН�?Я, ВЫВОД�?М СООТВЕТСТВУЮЩЕЕ СОСТОЯН�?Е CRC
			if(crc_in == crc_out){
				PutERROR((string_t *)&datastring,"CRC_OK");

				return 1;
			}
			else{
				PutERROR((string_t *)&datastring,"CRC_ERROR");
				return 0;
			}
			break;
		}
		PassSymCRC((uint8_t *)buf, buf_size, &crc_pointer, 1, crc32_buf);
		crc_count--;
	}

	// ЕСЛ�? БУФЕР ЗАКНОЧ�?ЛСЯ �? CRC НЕ БЫЛ НАЙДЕН
	if(!crc_count){
		PutERROR((string_t *)&datastring,"CRC NOT FOUND");
		return 0;
	}
	return 0;
}

/* ФУНКЦ�?Я ПО�?СКА ПОДСТРОК�?(ПОСЛЕДОВАТЛЬНОСТ�? С�?МВОЛОВ) В �?СХОДНОМ МАСС�?ВЕ
 * ЕСЛ�? ПОСЛЕДОВАТЕЛЬНОСТЬ ОБНАРУЖЕНА ВОЗВРАЩАЕТ 1 �? ЗНАЧЕН�?Е �?НДЕКС pointer НА СЛЕДУЮЩ�?Й
 * С�?МВОЛ ПОСЛЕ �?СКОМОЙ СТРОК�?.
 * ЕСЛ�? ПОСЛЕДОВАТЕЛЬНОСТЬ НЕ ОБНАРУЖЕНА, ТО ВОЗРАЩАЕТ 0 �? НЕ МЕНЯЕТ pointer
 */
uint8_t FindString(uint8_t *buf,size_t buf_size,uint16_t *pointer,const char *str,size_t str_size){
	uint8_t count = 0;
	uint16_t p = *pointer;
	while(count < str_size){
		if(buf[p] == (uint8_t)str[count]){
			p = (p < buf_size - 1) ? p + 1 : 0;
			count++;
		}
		else return 0;
	}

	*pointer = p;
	return 1;
}

/*
 *  ФУНКЦ�?Я М�?ГАЕТ СВЕТОД�?ОДАМ ОП�?САНЫМ СТРУКТУРОЙ led_status
 *  ON_time ЗАДАЕТ ВРЕМЯ, КОТОРОЕ СВЕТОД�?ОД ВКЛЮЧЕН
 *  OFF_time ЗАДАЕТ ВРЕМЯ, КОТОРОЕ СВЕТОД�?Д ВЫКЛЮЧЕН
 */
void LED_control(led_status *oneLED, uint16_t ON_time, uint16_t OFF_time){

	if(OFF_time == 0 && ON_time != 0){
		HAL_GPIO_WritePin(oneLED->LED_port, oneLED->LED_pin, GPIO_PIN_SET);
		return;
	}

	if(ON_time == 0 && OFF_time != 0){
		HAL_GPIO_WritePin(oneLED->LED_port, oneLED->LED_pin, GPIO_PIN_RESET);
		return;
	}


	if(HAL_GetTick() - oneLED->timer < ON_time){
		HAL_GPIO_WritePin(oneLED->LED_port, oneLED->LED_pin, GPIO_PIN_SET);
	}
	else if(HAL_GetTick() - oneLED->timer < ON_time + OFF_time){

		HAL_GPIO_WritePin(oneLED->LED_port, oneLED->LED_pin, GPIO_PIN_RESET);
	}
	else oneLED->timer = HAL_GetTick();
}

void LEDs_OFF(){
	HAL_GPIO_WritePin(RED_232_GPIO_Port, RED_232_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GREEN_232_GPIO_Port, GREEN_232_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BLUE_232_GPIO_Port, BLUE_232_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(RED_485_GPIO_Port, RED_485_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GREEN_485_GPIO_Port, GREEN_485_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BLUE_485_GPIO_Port, BLUE_485_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(RED_422_GPIO_Port, RED_422_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GREEN_422_GPIO_Port, GREEN_422_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BLUE_422_GPIO_Port, BLUE_422_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(STM32_BUTTON_LED_1_GPIO_Port, STM32_BUTTON_LED_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STM32_BUTTON_LED_2_GPIO_Port, STM32_BUTTON_LED_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STM32_BUTTON_LED_3_GPIO_Port, STM32_BUTTON_LED_3_Pin, GPIO_PIN_RESET);
}

void PutString(string_t *dtstring, const char* str, uint8_t *pointer){
	for(uint8_t i = 0; i < strlen(str); i++){
		dtstring->buf[i + *pointer] = str[i];
	}

	*pointer += strlen(str);
}

/*
 * ФУНКЦ�?Я ПРОПУСКАЕТ УКАЗАНОЕ КОЛ�?ЧЕСТВО С�?МВОЛОВ sym_amount
 * C УЧЕТОМ Ц�?КЛ�?ЧНОСТ�? БУФЕРА. ПРОПУЩЕНЫЕ С�?МВОЛЫ НЕ УЧ�?ТЫВАЮТСЯ ПР�? РАСЧЕТЕ CRC32
 * pointer_ - УКАЗАТЕЛЬ НА ТЕКУЩ�?Й ЭЛЕМЕНТ
 * sym_amount - КОЛ�?ЧЕСТВО ПРОПУСКАЕМЫХ С�?МВОЛОВ
 */
void PassSym(uint8_t *buf, size_t buf_size, uint16_t *pointer, uint8_t sym_amount){
	for(uint8_t i = 0; i < sym_amount; i++){
		*pointer = (*pointer < buf_size-1) ? *pointer + 1 : 0;
	}
}

/* ФУНКЦ�?Я ПРОПУСКАЕТ УКАЗАНОЕ КОЛ�?ЧЕСТВО С�?МВОЛОВ sym_amount
 * C УЧЕТОМ Ц�?КЛ�?ЧНОСТ�? БУФЕРА. ПРОПУЩЕНЫЕ С�?МВОЛЫ УЧ�?ТЫВАЮТСЯ ПР�? РАСЧЕТЕ CRC32*/
void PassSymCRC(uint8_t *buf,size_t buf_size,uint16_t *pointer, uint8_t sym_amount,buffer_t *crc_buf){
	for(uint8_t i = 0; i < sym_amount; i++){
		CRC32_Put(crc_buf,buf[*pointer]);
		*pointer = (*pointer < buf_size-1) ? *pointer + 1 : 0;
	}
}

/*
 * ФУНКЦ�?Я ПЕРЕВОД�?Т 16-РАЗРЯДНОЕ Ч�?СЛО, ЗАП�?САННОЕ В МАСС�?ВЕ С�?МВОЛОВ
 * В 10-РАЗРЯДНОЕ �?НТОВОЕ ЗНАЧЕН�?Е.
 */
uint32_t HexToDec(char *hex,uint8_t size){
	uint32_t dec = 0;

	for(uint8_t i = 0; i < size;i++){
		switch (hex[i]){
		case 'A': dec += 10 * pow(16,size-1 - i); break;
		case 'B': dec += 11 * pow(16,size-1 - i); break;
		case 'C': dec += 12 * pow(16,size-1 - i); break;
		case 'D': dec += 13 * pow(16,size-1 - i); break;
		case 'E': dec += 14 * pow(16,size-1 - i); break;
		case 'F': dec += 15 * pow(16,size-1 - i); break;

		case '1': dec += 1 * pow(16,size-1 - i); break;
		case '2': dec += 2 * pow(16,size-1 - i); break;
		case '3': dec += 3 * pow(16,size-1 - i); break;
		case '4': dec += 4 * pow(16,size-1 - i); break;
		case '5': dec += 5 * pow(16,size-1 - i); break;
		case '6': dec += 6 * pow(16,size-1 - i); break;
		case '7': dec += 7 * pow(16,size-1 - i); break;
		case '8': dec += 8 * pow(16,size-1 - i); break;
		case '9': dec += 9 * pow(16,size-1 - i); break;
		case '0': dec += 0; break;
		}
	}
	return dec;
}

/*
 * ФУНКЦ�?Я ПАРС�?НГА СТРОК СВЕТОД�?ОДОВ
 *
 */
void LEDStringPars(string_t *datastring, RGB_status *leds){

	// Т.К. 3 RGB
	for(uint8_t i = 0; i < 3; i++){
		uint8_t pointer = 0;
		while(pointer < 6){
			switch(datastring[serv_string_1].buf[pointer]){
			case (uint8_t)'R': leds[i].RED.LED_status = datastring[serv_string_1].buf[pointer+1]; break;
			case (uint8_t)'G': leds[i].GREEN.LED_status = datastring[serv_string_1].buf[pointer+1]; break;
			case (uint8_t)'B': leds[i].BLUE.LED_status = datastring[serv_string_1].buf[pointer+1]; break;
			}
			pointer++;
		}
	}
}

/*
 * ФУНКЦ�?Я СТРОК�?, УПРАВЛЯЮЩ�?Е РЕЛЕ, DC �? ДС�?ПЛЕЕМ
 */
void ServiceStringPars(string_t *datastring, ctrl_status *active){
	active->RELE[0] = (datastring[serv_string_2].buf[1] == '1') ? 1 : 0;
	active->RELE[1] = (datastring[serv_string_2].buf[3] == '1') ? 1 : 0;
	active->RELE[2] = (datastring[serv_string_2].buf[5] == '1') ? 1 : 0;
	active->RELE[3] = (datastring[serv_string_2].buf[7] == '1') ? 1 : 0;
	active->DC = datastring[serv_string_2].buf[10] - 48;
	active->DISP = datastring[serv_string_2].buf[15] - 48;
}

/*
 * ОБРАБОТЧ�?К ПРЕРЫВАН�?Й ПОЛОВ�?НЫ ПЕРЕПОЛНЕН�?Я БУФЕРА uatrt_buf
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart){
	uart_hlf_flag = 1;
}

/*
 * фУНКЦ�?Я �?Н�?Ц�?АЛ�?ЗАЦ�?�? Д�?СПЛЕЯ
 */
void DisplayInit(u8g2_t* u8g2){
	u8g2_Setup_ssd1322_nhd_256x64_f(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
	u8g2_InitDisplay(u8g2);
	u8g2_ClearDisplay(u8g2);
	u8g2_SetPowerSave(u8g2, 0);
}
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	GPIOD->ODR ^= 1<<13;
//	HAL_UART_Receive_IT(&huart3, (uint8_t*)uart_buf,UartBufSize);
//}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
