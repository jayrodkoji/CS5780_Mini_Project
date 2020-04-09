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
#include "stm32f429xx.h"
#include "main.h"
#include "Helper/l3gd20.h"
#include "Helper/led.h"
#include "Helper/spi.h"
#include "Helper/usart.h"

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
  /* USER CODE BEGIN 2 */
  init_LEDs();
  
  // Initialize USART1 (ST-Link)
  init_USART1();

  // Initialize SPI5 (gyroscope)
  init_SPI5();

  if (L3GD20_ID_match())
  {
    set_green_LED(1);
  }

  init_L3GD20();

  // Initialize LTDC
  RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;
  // Add alternate function GPIOs for LTDC
  // GPIOA 3(AF14) 4(AF14) 6(AF14) 11(AF14) 12(AF14)
  // GPIOB 0(AF9) 1(AF9) 8(AF14) 9(AF14) 10(AF14) 11(AF14)
  // GPIOC 6(AF14) 7(AF14) 10(AF14)
  // GPIOD 3(AF14) 6(AF14)
  // GPIOF 10(AF14)
  // GPIOG 6(AF14) 7(AF14) 10(AF9) 11(AF14) 12(AF9)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN |
                  RCC_AHB1ENR_GPIOBEN |
                  RCC_AHB1ENR_GPIOCEN |
                  RCC_AHB1ENR_GPIODEN |
                  RCC_AHB1ENR_GPIOFEN |
                  RCC_AHB1ENR_GPIOGEN;
  GPIOA->MODER |= 0x02802280;
  GPIOA->OSPEEDR |= 0x02802280;
  GPIOA->AFR[0] |= 0x0E0EE000;
  GPIOA->AFR[1] |= 0x000EE000;

  GPIOB->MODER |= 0x00AA000A;
  GPIOB->OSPEEDR |= 0x00AA000A;
  GPIOB->AFR[0] |= 0x00000099;
  GPIOB->AFR[1] |= 0x0000EEEE;

  GPIOC->MODER |= 0x0020A000;
  GPIOC->OSPEEDR |= 0x0020A000;
  GPIOC->AFR[0] |= 0xEE000000;
  GPIOC->AFR[1] |= 0x00000E00;

  GPIOD->MODER |= 0x00002080;
  GPIOD->OSPEEDR |= 0x00002080;
  GPIOD->AFR[0] |= 0x0E00E000;

  GPIOF->MODER |= 0x00200000;
  GPIOF->OSPEEDR |= 0x00200000;
  GPIOF->AFR[1] |= 0x00000E00;

  GPIOG->MODER |= 0x02A0A000;
  GPIOG->OSPEEDR |= 0x02A0A000;
  GPIOG->AFR[0] |= 0xEE000000;
  GPIOG->AFR[1] |= 0x0009E900;


  // LCD constants
  const uint16_t LCD_WIDTH = 240;
  const uint16_t LCD_HEIGHT = 320;

  const uint16_t HFP = 10;
  const uint16_t HSYNC = 10;
  const uint16_t HBP = 20; // was 10 but seemed wrong per datasheet

  const uint16_t VFP = 4;
  const uint16_t VSYNC = 2;
  const uint16_t VBP = 2;

  const uint16_t ACTIVE_W = HSYNC + HBP + LCD_WIDTH - 1;
  const uint16_t ACTIVE_H = VSYNC + VBP + LCD_HEIGHT - 1;
  const uint16_t TOTAL_W = HSYNC + HBP + LCD_WIDTH + HFP - 1;
  const uint16_t TOTAL_H = VSYNC + VBP + LCD_HEIGHT + VFP - 1;

  LTDC->SSCR = ((HSYNC - 1) << 16) | (VSYNC - 1);
  LTDC->BPCR = ((HBP - 1) << 16) | (VBP - 1);
  LTDC->AWCR = (ACTIVE_W << 16) | ACTIVE_H;
  LTDC->TWCR = (TOTAL_W << 16) | TOTAL_H;
  LTDC->BCCR = 0xFF << LTDC_BCCR_BCRED_Pos; // RED
  LTDC_Layer1->DCCR = 0xFF << LTDC_LxDCCR_DCALPHA_Pos | 0xFF << LTDC_LxDCCR_DCGREEN_Pos;
    
  /* configure layer 1  From http:\//www.lucadavidian.com/2017/10/02/stm32-using-the-ltdc-display-controller/*/
  LTDC_Layer1->WHPCR = 269 << LTDC_LxWHPCR_WHSPPOS_Pos | 30 << LTDC_LxWHPCR_WHSTPOS_Pos;
  LTDC_Layer1->WVPCR = 323 << LTDC_LxWVPCR_WVSPPOS_Pos | 4 << LTDC_LxWVPCR_WVSTPOS_Pos;
  LTDC_Layer1->PFCR = 0x01;
  //LTDC_Layer1->CFBAR = (uint32_t)framebuffer;
    
  LTDC_Layer1->CFBLR = 240 * 3 << LTDC_LxCFBLR_CFBP_Pos | 240 * 3 + 3 << LTDC_LxCFBLR_CFBLL_Pos;
  LTDC_Layer1->CFBLNR = 320;
    
  LTDC_Layer1->DCCR = 0xFF << LTDC_LxDCCR_DCALPHA_Pos | 0xFF << LTDC_LxDCCR_DCGREEN_Pos;
  LTDC_Layer1->CACR = 255;
  LTDC_Layer1->CR |= LTDC_LxCR_LEN;
  // end layer1 config

  LTDC->SRCR = LTDC_SRCR_IMR;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int16_t X_data;
  int16_t Y_data;
  int64_t X_pos = 0;
  int64_t Y_pos = 0;
  char message[10];
  while (1)
  {
    /* USER CODE END WHILE */
    HAL_Delay(100);
    get_XY_data(&X_data, &Y_data);
    X_pos += X_data;
    Y_pos += Y_data;

    print("X_data: ");
    itoa(X_data/1000, message, 10);
    print(message);
    print(", Y_data: ");
    itoa(Y_data/1000, message, 10);
    print(message);
    print(", X_pos: ");
    itoa(X_pos/1000, message, 10);
    print(message);
    print(", Y_pos: ");
    itoa(Y_pos/1000, message, 10);
    println(message);


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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* initialize LTDC LCD clock */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 60;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
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
