/**
  ******************************************************************************
  * @file           : spi.c
  * @brief          : Helper functions for spi
  ******************************************************************************
  * Creators        : Jacob Sorenson & Jared Knight
  * Course          : CS5780
  *                   University of Utah
  */

#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "Helper/spi.h"

void init_SPI5()
{
  // Return early if already enabled
  if ((SPI5->CR1 & SPI_CR1_SPE_Msk) != 0)
  {
    return;
  }

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; // Enable GPIOF clock

  // Configure alternate function for PF6-9 (AF5)
  GPIOF->MODER = (GPIOF->MODER & ~(GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk |
                                   GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk)) |
                 GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1 |
                 GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1;
  GPIOF->OSPEEDR = (GPIOF->OSPEEDR & ~(GPIO_OSPEEDR_OSPEED6_Msk | GPIO_OSPEEDR_OSPEED7_Msk |
                                       GPIO_OSPEEDR_OSPEED8_Msk | GPIO_OSPEEDR_OSPEED9_Msk)) |
                   GPIO_OSPEEDR_OSPEED6_1 | GPIO_OSPEEDR_OSPEED7_1 |
                   GPIO_OSPEEDR_OSPEED8_1 | GPIO_OSPEEDR_OSPEED9_1;
  GPIOF->AFR[0] = (GPIOF->AFR[0] & ~(GPIO_AFRL_AFSEL6_Msk | GPIO_AFRL_AFSEL7_Msk)) |
                  (0x5 << GPIO_AFRL_AFSEL6_Pos) | (0x5 << GPIO_AFRL_AFSEL7_Pos);
  GPIOF->AFR[1] = (GPIOF->AFR[0] & ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk)) |
                  (0x5 << GPIO_AFRH_AFSEL8_Pos) | (0x5 << GPIO_AFRH_AFSEL9_Pos);

  RCC->APB2ENR |= RCC_APB2ENR_SPI5EN; // Enable SPI5 clock

  // Since PCLK frequency is 16MHz, set BR[2:0] to 0 for 8MHz clock
  SPI5->CR1 &= ~SPI_CR1_SPE; // Disable
  SPI5->CR1 |= SPI_CR1_BR_2; // 32 prescaler
  SPI5->CR1 |= SPI_CR1_SSI; // Software slave mode
  SPI5->CR1 |= SPI_CR1_SSM; // Software slave mode
  SPI5->CR1 |= SPI_CR1_MSTR; // Master mode
  SPI5->CR1 |= SPI_CR1_SPE; // Enable
}

uint8_t write_SPI5(uint8_t data)
{
  while ((SPI5->SR & SPI_SR_TXE) == 0 || (SPI5->SR & SPI_SR_BSY));
  SPI5->DR = data; // Write data
  while ((SPI5->SR & SPI_SR_RXNE) == 0 || (SPI5->SR & SPI_SR_BSY));

  return SPI5->DR; // Read data
}