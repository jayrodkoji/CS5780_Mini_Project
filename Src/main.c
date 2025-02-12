/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body contains initialization, logic, and state change.
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
  ******************************************************************************
  * Creators        : Jacob Sorenson & Jared Knight
  * Course          : CS5780
  *                   University of Utah
  */

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f429xx.h"
#include "stdlib.h"
#include "Helper/l3gd20.h"
#include "Helper/lcd.h"
#include "Helper/led.h"
#include "Helper/spi.h"
#include "Helper/uart.h"
#include "Helper/rng.h"

/* USER CODE END Includes */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_SPEED 20
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
LTDC_HandleTypeDef hltdc;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void SystemClock_Config(void);
void print_x_y_data(int16_t X_data, int16_t Y_data);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
enum ball{red_ball = 0, green_ball = 1};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  init_SPI5();
  init_LCD();
  init_LTDC();
  init_RNG();
  init_USART1();
  init_L3GD20();
  init_LEDs();

  // Throw away number. See RNG documentation
  (void)get_random_number();

  startup_sequence();

  // initialized ball and target
  uint16_t current_ball = red_ball;

  uint32_t target_x = get_random_number();
  uint32_t target_y = get_random_number();

  uint32_t ball_x = get_random_number();
  uint32_t ball_y = get_random_number();

  display_target(target_x, target_y);
  display_ball(current_ball, ball_x, ball_y);

  uint16_t red_timer = 0;
  uint16_t green_timer = 0;

  update_red_LED_timer(red_timer);
  update_green_LED_timer(green_timer);

  int16_t X_data, Y_data;
  int16_t X_speed = 0;
  int16_t Y_speed = 0;
  uint8_t level = 0;
  /* End Initialize all configured peripherals */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (level <= 10)
    {
      /* USER CODE END WHILE */
      HAL_Delay(2);

      get_XY_data(&X_data, &Y_data);
      print_x_y_data(X_data, Y_data);

      // update speed when x/y value is large/small enough
      X_speed -= X_data > 2000 ? 1 : 0;
      X_speed += X_data < -2000 ? 1 : 0;
      Y_speed -= Y_data > 2000 ? 1 : 0;
      Y_speed += Y_data < -2000 ? 1 : 0;

      // verify speed doesn't exceed max speed
      X_speed = X_speed > MAX_SPEED ? MAX_SPEED : X_speed;
      X_speed = X_speed < -MAX_SPEED ? -MAX_SPEED : X_speed;
      Y_speed = Y_speed > MAX_SPEED ? MAX_SPEED : Y_speed;
      Y_speed = Y_speed < -MAX_SPEED ? -MAX_SPEED : Y_speed;

      ball_x = ball_x + X_speed / 10;
      ball_y = ball_y + Y_speed / 10;

      display_ball(current_ball, ball_x, ball_y);

      // Detect if ball hits target updating game level as appropriate
      // Each level increases led blink frequency.
      if ((abs(ball_x%290 - target_x%290) < 30) && (abs(ball_y%210 - target_y%210) < 30))
      {
        level++;
        if (1 == level)
        {
          update_red_LED_timer(1000);
        }
        else if (2 == level)
        {
          update_red_LED_timer(500);
        }
        else if (3 == level)
        {
          update_red_LED_timer(250);
        }
        else if (4 == level)
        {
          update_red_LED_timer(100);
        }
        else if (5 == level)
        {
          update_red_LED_timer(50);
        }
        else if (6 == level)
        {
          update_red_LED_timer(0);
          set_red_LED(1);
          update_green_LED_timer(1000);
          current_ball= green_ball;
        }
        else if (7 == level)
        {
          update_green_LED_timer(500);
        }
        else if (8 == level)
        {
          update_green_LED_timer(250);
        }
        else if (9 == level)
        {
          update_green_LED_timer(100);
        }
        else if (10 == level)
        {
          update_green_LED_timer(50);
        }
        else if (level > 10)
        {
          update_green_LED_timer(0);
          set_green_LED(1);
        }

        // move target
        target_x = get_random_number();
        target_y = get_random_number();
        display_target(target_x, target_y);
      }
    }
    else
    {
      display_win_screen();
      HAL_Delay(10000);
    }
  }
}

/*
 * For verifying x and y data from gyroscope
 */
void print_x_y_data(int16_t X_data, int16_t Y_data){
    char buf[10];
    itoa(X_data, buf, 10);
    print("X data: ");
    print(buf);
    itoa(Y_data, buf, 10);
    print(", Y data: ");
    println(buf);
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
  /** Initializes the CPU, AHB and APB buses clocks
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
