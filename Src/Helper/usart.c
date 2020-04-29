/**
  ******************************************************************************
  * @file           : usart.c
  * @brief          : Helper functions for usart.
  ******************************************************************************
  * Creators        : Jacob Sorenson & Jared Knight
  * Course          : CS5780
  *                   University of Utah
  */

#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "Helper/usart.h"

void init_USART1()
{
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable USART1 clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock

  // Configure alternate function for PA9-10 (AF7)
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk)) |
                 GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1;
  GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk)) |
                  (0x7 << GPIO_AFRH_AFSEL9_Pos) | (0x7 << GPIO_AFRH_AFSEL10_Pos);
  USART1->BRR = (HAL_RCC_GetHCLKFreq() / 15 / 115200) << USART_BRR_DIV_Mantissa_Pos;
  USART1->CR1 = USART_CR1_TXEIE | USART_CR1_RXNEIE | USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void print(char* message)
{
  while(*message != '\0')
  {
    // Wait for TX empty
    while((USART1->SR & USART_SR_TXE) != USART_SR_TXE);
    USART1->DR = *message++;
  }
}

void println(char* message)
{
  print(message);
  print("\r\n");
}