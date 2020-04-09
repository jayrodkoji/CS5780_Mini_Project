#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "Helper/led.h"

void init_LEDs()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; // Enable GPIOG clock

    GPIOG->MODER = (GPIOG->MODER & ~(GPIO_MODER_MODE13_Msk | GPIO_MODER_MODE14_Msk)) |
                   GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0;
    GPIOG->OTYPER &= ~(GPIO_OTYPER_OT13_Msk | GPIO_OTYPER_OT14_Msk);
    GPIOG->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED13_Msk | GPIO_OSPEEDR_OSPEED14_Msk);
    GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD13_Msk | GPIO_PUPDR_PUPD14_Msk);
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