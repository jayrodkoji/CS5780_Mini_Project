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
#include "Helper/l3gd20.h"
#include "Helper/lcd.h"
#include "Helper/led.h"
#include "Helper/spi.h"
#include "Helper/usart.h"
#include "Helper/rng.h"

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
  init_SPI5();

  init_LCD();
  init_LTDC();
  init_RNG();

  uint32_t first_random_number = get_random_number(); // not used but for comparison.

  //startup_sequence();

  uint16_t current_ball = 0;
  uint32_t target_x = get_random_number();
  uint32_t target_y = get_random_number();
  uint32_t ball_x = get_random_number();
  uint32_t ball_y = get_random_number();
  display_target(target_x, target_y);
  display_ball(current_ball, ball_x, ball_y);
  HAL_Delay(1000);


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  init_LEDs();

  // Initialize USART1 (ST-Link)
  init_USART1();

  if (L3GD20_ID_match())
  {
    set_green_LED(1);
  }
  update_red_LED_timer(500);

  init_L3GD20();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int16_t X_data;
  int16_t Y_data;
  int64_t X_pos = 0;
  int64_t Y_pos = 0;
  int16_t X_speed = 0;
  int16_t Y_speed = 0;
  int16_t direction = 0;

  char message[10];
  uint32_t rand;
  while (1) {
      /* USER CODE END WHILE */
      HAL_Delay(100);

      get_XY_data(&X_data, &Y_data);

      if (X_data >= 30000) {
              direction = 1;
      } else if (X_data <= -30000) {
              direction = -1;
      }

     if((X_speed >= 0 && X_speed <= 5) || (X_speed <= 0 && X_speed >= -5))
        if(X_speed == 5){
            if(direction == -1)
                X_speed += direction;

        }
     else if (X_speed == -5) {
        if (direction == 1)
            X_speed += direction;
     }
     else
         X_speed += direction;

      if (Y_data >= 30000) {
          direction = 1;
      } else if (Y_data <= -30000) {
          direction = -1;
      }

      if((Y_speed >= 0 && X_speed <= 5) || (Y_speed <= 0 && X_speed >= -5))
          if(Y_speed == 5){
              if(direction == -1)
                  Y_speed += direction;

          }
          else if (Y_speed == -5) {
              if (direction == 1)
                  Y_speed += direction;
          }
          else
              Y_speed += direction;

      ball_x = ball_x + X_speed;
      ball_y = ball_y + Y_speed;
      display_ball(current_ball, ball_x, ball_y);
      X_pos += X_data / 10;
      Y_pos += Y_data / 10;

      if(X_data >= 30000 || X_data <= -30000) {
          print("Direction: ");
          itoa(direction, message, 10);
          print(message);
          print(", X_speed: ");
          itoa(X_speed, message, 10);
          print(message);
          print(", X_data: ");
          itoa(X_data, message, 10);
          print(message);
          print(", Y_data: ");
          itoa(Y_data, message, 10);
          print(message);
          print(", X_pos: ");
          itoa(X_pos, message, 10);
          print(message);
          print(", Y_pos: ");
          itoa(Y_pos, message, 10);
          println(message);
      }

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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 60;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
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
