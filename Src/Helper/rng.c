#include "Helper/rng.h"
#include "stm32f429xx.h"

void init_RNG()
{
  RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN; // Enable clock

  RNG->CR |= RNG_CR_RNGEN;
}

uint32_t get_random_number()
{
  while((RNG->SR & RNG_SR_DRDY_Msk) == 0); // Wait until random number is ready

  return RNG->DR;
}