/**
  ******************************************************************************
  * @file           : RNG.c
  * @brief          : Helper functions for RNG to Initialize and get random numbers
  ******************************************************************************
  * Creators        : Jacob Sorenson & Jared Knight
  * Course          : CS5780
  *                   University of Utah
  */

#include "Helper/rng.h"
#include "stm32f429xx.h"

void init_RNG()
{
  RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN; // Enable clock
  RNG->CR |= RNG_CR_RNGEN;           // Enable RNG
}

uint32_t get_random_number()
{
  while(RNG->SR & !(RNG->SR & 0x1)){}  // Wait until random number is ready
  return RNG->DR;
}