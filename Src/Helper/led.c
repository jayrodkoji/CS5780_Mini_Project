/**
  ******************************************************************************
  * @file           : led.c
  * @brief          : Helper functions for lcd manipulation
  ******************************************************************************
  * Creators        : Jacob Sorenson & Jared Knight
  * Course          : CS5780
  *                   University of Utah
  */

#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "Helper/led.h"

void TIM2_IRQHandler(void)
{
  TIM2->SR &= ~TIM_SR_CC1IF;
  GPIOG->ODR ^= GPIO_ODR_OD14;
}

void TIM3_IRQHandler(void)
{
  TIM3->SR &= ~TIM_SR_CC1IF;
  GPIOG->ODR ^= GPIO_ODR_OD13;
}

void update_red_LED_timer(const uint16_t time_ms)
{
  TIM2->ARR = time_ms;
  TIM2->CNT=1;
}

void update_green_LED_timer(const uint16_t time_ms)
{
  TIM3->ARR = time_ms;
  TIM3->CNT = 1;
}

void init_LEDs()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; // Enable GPIOG clock

  GPIOG->MODER = (GPIOG->MODER & ~(GPIO_MODER_MODE13_Msk | GPIO_MODER_MODE14_Msk)) |
                 GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0;
  GPIOG->OTYPER &= ~(GPIO_OTYPER_OT13_Msk | GPIO_OTYPER_OT14_Msk);
  GPIOG->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED13_Msk | GPIO_OSPEEDR_OSPEED14_Msk);
  GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk | GPIO_PUPDR_PUPD14_Msk);

  // Initialize timers
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // Red timer
  TIM2->PSC = 7999; // 1ms timer count
  TIM2->DIER = TIM_DIER_CC1IE;
  update_red_LED_timer(0);
  TIM2->CR1 |= TIM_CR1_CEN;
  NVIC_EnableIRQ(TIM2_IRQn);

  // Green timer
  TIM3->PSC = 7999; // 1ms timer count
  TIM3->DIER = TIM_DIER_CC1IE;
  update_green_LED_timer(0);
  TIM3->CR1 |= TIM_CR1_CEN;
  NVIC_EnableIRQ(TIM3_IRQn);
}

void set_red_LED(const uint8_t state)
{
  if (state == 0)
  {
    GPIOG->ODR &= ~GPIO_ODR_OD14;
  }
  else
  {
    GPIOG->ODR |= GPIO_ODR_OD14;
  }
}

void set_green_LED(const uint8_t state)
{
  if (state == 0)
  {
    GPIOG->ODR &= ~GPIO_ODR_OD13;
  }
  else
  {
    GPIOG->ODR |= GPIO_ODR_OD13;
  }
}