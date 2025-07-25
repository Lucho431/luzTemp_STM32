/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_i2c_lfs.h"
#include "IOport_lfs.h"
#include "menuPantallas.h"
#include "funciones_domotica.h"
#include "24lc256_lfs.h"
#include "hora_tablero.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define IWD_OFF
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t flag_tim2 = 0; //cada 10 ms
uint8_t periodo_IOport = 1; //cada 20 ms
uint8_t periodo_temp = 99; //cada 1 segundo
uint8_t resets = 0;
//variables de prueba
uint8_t varTest;
uint8_t periodo_test = 24; //cada 250 ms
uint8_t max_periodoTest = 24; //49;
uint8_t dirI2C;
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_IWDG_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(150);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_ADC_Start(&hadc1);

//  uint8_t fff = sizeof (RTC_TimeTypeDef); //3 bytes
//  __NOP();
//  fff = sizeof (RTC_DateTypeDef); //4 bytes
//  __NOP();

//  HAL_StatusTypeDef i2c_status;
//
//  for (dirI2C = 0; dirI2C < 128; dirI2C++){
//	  i2c_status = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(dirI2C<<1), 1, 10);
//	  if(i2c_status == HAL_OK){
//		  __NOP();
//	  }
//  }

//  while (1){
//	  varTest = !varTest;
//	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, varTest);
//	  HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, varTest);
////	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, varTest);
////	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
////	  HAL_GPIO_TogglePin(OUT_rele_GPIO_Port, OUT_rele_Pin);
//	  HAL_Delay(500);
//	  __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
//  }

  HAL_I2C_Mem_Read(&hi2c1, 0x50<<1, OFFSET_RESETS, I2C_MEMADD_SIZE_16BIT, &resets, 1, 100);
  resets++;
  HAL_I2C_Mem_Write(&hi2c1, 0x50<<1, OFFSET_RESETS, I2C_MEMADD_SIZE_16BIT, &resets, 1, 100);
#ifndef IWD_OFF
  MX_IWDG_Init();
  __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
#endif
  lcd_init(&hi2c1, 0x27);
//  lcd_send_string("holis");
  init_botonera(&hi2c1, 0x20);
  init_sensores(&hadc1);
  start_menu(0);
#ifndef IWD_OFF
  __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (flag_tim2 != 0){

		  if (periodo_IOport != 0){
			  periodo_IOport--;
		  }else{
			  lecturaTeclas(); //cada 20 ms.
			  update_outputs(); //cada 20 ms.
			  periodo_IOport = 1;
		  } //fin if periodo_IOport

		  if (periodo_temp != 0){
			  periodo_temp--;
		  }else{
			  update_DHT();
			  update_ldr();
			  refresh_infoDHT();
			  periodo_temp = 99;
		  }

//		  if (periodo_test != 0){
//			  periodo_test--;
//		  }else{
//			  varTest = !varTest;
//			  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, varTest);
////			  HAL_GPIO_WritePin(OUT_rele_GPIO_Port, OUT_rele_Pin, varTest);
//			  periodo_test = max_periodoTest; //cada 250 ms o 500 ms
//		  }

		  timeoutMenu();
		  check_duracionPulsadores();
//		  timeoutTestAuto();
		  timeoutGrabaSinc();


		  flag_tim2 = 0;
	  } //fin if flag_tim2

	  check_menu();
	  check_luzAuto();
	  update_teclas();
#ifndef IWD_OFF
	  __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM2){
		flag_tim2 = 1;
	}
} //fin HAL_TIM_PeriodElapsedCallback()

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
