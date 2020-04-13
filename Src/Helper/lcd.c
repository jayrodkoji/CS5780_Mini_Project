#include "stm32f429xx.h"
#include "Helper/lcd.h"

void init_LCD_GPIOs()
{
    // Initialize LTDC
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

  // Initialize GPIO for WRX and CS pins
  GPIOD->MODER |= GPIO_MODER_MODE13_0;
  GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED13_0;
  GPIOD->OTYPER &= ~GPIO_OTYPER_OT13;
  GPIOD->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
  GPIOC->MODER |= GPIO_MODER_MODE2_0;
  GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED2_0;
  GPIOC->OTYPER &= ~GPIO_OTYPER_OT2;
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD2_Msk;
}

void init_LTDC()
{
    RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;
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
}

void init_LCD()
{
  init_LCD_GPIOs();

}

void draw_rectangle(LTDC_Layer_TypeDef* p_layer,
                    const uint32_t x_start,
                    const uint32_t x_end,
                    const uint32_t y_start,
                    const uint32_t y_end)
{
  p_layer->WHPCR = x_end << LTDC_LxWHPCR_WHSPPOS_Pos | x_start << LTDC_LxWHPCR_WHSTPOS_Pos;
  p_layer->WVPCR = y_end << LTDC_LxWVPCR_WVSPPOS_Pos | y_start << LTDC_LxWVPCR_WVSTPOS_Pos;
  p_layer->PFCR = 0x01; // RGB888 pixel format
  p_layer->CACR = 255; // constant alpha
  p_layer->CR |= LTDC_LxCR_LEN; // enable layer

  LTDC->SRCR = LTDC_SRCR_IMR;
}