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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f429xx.h"
#include "tm_stm32_lcd.h"
#include "Helper/l3gd20.h"
#include "Helper/led.h"
#include "Helper/spi.h"
#include "Helper/usart.h"
#include "Image.h"


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
LTDC_HandleTypeDef hltdc;

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
  TM_LCD_Init();
  LTDC->GCR &= ~LTDC_GCR_LTDCEN;

  LTDC->BCCR = 0xFF << LTDC_BCCR_BCRED_Pos;

  LTDC_Layer1->WHPCR = 110 << LTDC_LxWHPCR_WHSPPOS_Pos | 100 << LTDC_LxWHPCR_WHSTPOS_Pos;          // window horizontal start/stop positions
  LTDC_Layer1->WVPCR = 110 << LTDC_LxWVPCR_WVSPPOS_Pos | 100 << LTDC_LxWVPCR_WVSTPOS_Pos;           // window vertical start/stop positions
  LTDC_Layer1->PFCR = 0x01;   // RGB888 pixel format
  LTDC_Layer1->CACR = 255;                                                                        // constant alpha
  LTDC_Layer1->CR |= LTDC_LxCR_LEN;                                                               // enable layer1

  LTDC_Layer2->WHPCR = 150 << LTDC_LxWHPCR_WHSPPOS_Pos | 140 << LTDC_LxWHPCR_WHSTPOS_Pos;          // window horizontal start/stop positions
  LTDC_Layer2->WVPCR = 150 << LTDC_LxWVPCR_WVSPPOS_Pos | 140 << LTDC_LxWVPCR_WVSTPOS_Pos;           // window vertical start/stop positions
  LTDC_Layer2->PFCR = 0x01;   // RGB888 pixel format
  LTDC_Layer2->CACR = 255;                                                                        // constant alpha
  LTDC_Layer2->CR |= LTDC_LxCR_LEN;                                                               // enable layer1


  LTDC->SRCR = LTDC_SRCR_IMR;                                                                     // immediate shadow registers reload 

  LTDC->GCR |= LTDC_GCR_LTDCEN;

  TM_LCD_SetLayer1();
  TM_LCD_DrawFilledCircle(100, 100, 10, LCD_COLOR_GREEN);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  init_LEDs();

  // Initialize USART1 (ST-Link)
  init_USART1();

  // Initialize SPI5 (gyroscope)
  // init_SPI5();

  // if (L3GD20_ID_match())
  // {
  //   set_green_LED(1);
  // }
  // set_red_LED(1);
  update_red_LED_timer(500);
  update_green_LED_timer(500);

  // init_L3GD20();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int16_t X_data;
  int16_t Y_data;
  int64_t X_pos = 0;
  int64_t Y_pos = 0;
  char message[10];
  println("Hi there");
  while (1)
  {
    
    HAL_Delay(1000);
    LTDC_Layer1->WHPCR = 110 << LTDC_LxWHPCR_WHSPPOS_Pos | 100 << LTDC_LxWHPCR_WHSTPOS_Pos;          // window horizontal start/stop positions
    LTDC_Layer1->WVPCR = 210 << LTDC_LxWVPCR_WVSPPOS_Pos | 200 << LTDC_LxWVPCR_WVSTPOS_Pos;           // window vertical start/stop positions
    LTDC->SRCR = LTDC_SRCR_IMR;                                                                     // immediate shadow registers reload 
    /* USER CODE END WHILE */
  //  HAL_Delay(100);
  //  get_XY_data(&X_data, &Y_data);
  //  X_pos += X_data;
  //  Y_pos += Y_data;

  //  print("X_data: ");
  //  itoa(X_data/1000, message, 10);
  //  print(message);
  //  print(", Y_data: ");
  //  itoa(Y_data/1000, message, 10);
  //  print(message);
  //  print(", X_pos: ");
  //  itoa(X_pos/1000, message, 10);
  //  print(message);
  //  print(", Y_pos: ");
  //  itoa(Y_pos/1000, message, 10);
  //  println(message);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 60;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 7;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_16;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
