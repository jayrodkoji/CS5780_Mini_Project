/**
  ******************************************************************************
  * @file           : l3gd20.c
  * @brief          : Helper file to enable communication with l3gd20 gyroscope
  ******************************************************************************
  * Creators        : Jacob Sorenson & Jared Knight
  * Course          : CS5780
  *                   University of Utah
  */

#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "Helper/l3gd20.h"
#include "Helper/spi.h"
#include "Helper/usart.h"

// CS set and reset
#define CS_SET GPIOC->ODR |= GPIO_ODR_OD1;
#define CS_RESET GPIOC->ODR &= ~GPIO_ODR_OD1;

void init_L3GD20()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable GPIOC clock

    // Setup PC1 (CS)
    GPIOC->MODER |= GPIO_MODER_MODE1_0;
    GPIOC->OSPEEDR |= GPIO_MODER_MODE1_0;
    CS_SET;

    write_L3GD20_register(0x21, 0x20); // Normal mode 7.2Hz HPF
    write_L3GD20_register(0x23, 0x30); // 2000 dps
    write_L3GD20_register(0x24, 0x13); // High pass filter enable with LPF2 configuration
    write_L3GD20_register(0x20, 0xDB); // Enable X and Y axis at 760 Hz
}

void read_L3GD20_register(uint8_t address, uint8_t* value)
{
  CS_RESET;
  write_SPI5((address | (1 << 7))); // Read from address
  *value = write_SPI5(0);
  CS_SET;
}

void write_L3GD20_register(uint8_t address, uint8_t value)
{
  CS_RESET;
  write_SPI5(address); // Write to address
  write_SPI5(value);
  CS_SET;
}

uint8_t L3GD20_ID_match()
{
  init_SPI5();
  uint8_t ID;
  read_L3GD20_register(0x0F, &ID);

  if (0xD4 == ID)
  {
    return 1;
  }
  return 0;
}

void get_XY_data(int16_t* X_data, int16_t* Y_data)
{
  uint8_t temp;
  read_L3GD20_register(0x28, &temp);
  *X_data = temp << 8;
  read_L3GD20_register(0x29, &temp);
  *X_data |= temp;
  read_L3GD20_register(0x2A, &temp);
  *Y_data = temp << 8;
  read_L3GD20_register(0x2B, &temp);
  *Y_data |= temp;
}