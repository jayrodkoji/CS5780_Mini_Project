#include "stm32f429xx.h"
#include "Helper/lcd.h"
#include "Helper/spi.h"

// Commands
#define ILI9341_RESET         0x01
#define ILI9341_SLEEP_OUT     0x11
#define ILI9341_GAMMA         0x26
#define ILI9341_DISPLAY_OFF   0x28
#define ILI9341_DISPLAY_ON    0x29
#define ILI9341_COLUMN_ADDR   0x2A
#define ILI9341_PAGE_ADDR     0x2B
#define ILI9341_GRAM          0x2C
#define ILI9341_MAC           0x36
#define ILI9341_PIXEL_FORMAT  0x3A
#define ILI9341_WDB           0x51
#define ILI9341_WCD           0x53
#define ILI9341_RGB_INTERFACE 0xB0
#define ILI9341_FRC           0xB1
#define ILI9341_BPC           0xB5
#define ILI9341_DFC           0xB6
#define ILI9341_POWER1        0xC0
#define ILI9341_POWER2        0xC1
#define ILI9341_VCOM1         0xC5
#define ILI9341_VCOM2         0xC7
#define ILI9341_POWERA        0xCB
#define ILI9341_POWERB        0xCF
#define ILI9341_PGAMMA        0xE0
#define ILI9341_NGAMMA        0xE1
#define ILI9341_DTCA          0xE8
#define ILI9341_DTCB          0xEA
#define ILI9341_POWER_SEQ     0xED
#define ILI9341_3GAMMA_EN     0xF2
#define ILI9341_INTERFACE     0xF6
#define ILI9341_PRC           0xF7

// CS and WRX set and reset
#define CS_SET GPIOC->ODR |= GPIO_ODR_OD2;
#define CS_RESET GPIOC->ODR &= ~GPIO_ODR_OD2;
#define WRX_SET GPIOD->ODR |= GPIO_ODR_OD13;
#define WRX_RESET GPIOD->ODR &= ~GPIO_ODR_OD13;

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

void send_spi5(uint8_t data)
{
  while ((SPI5->SR & SPI_SR_TXE) == 0 || (SPI5->SR & SPI_SR_BSY));
  SPI5->DR = data; // Write data
  while ((SPI5->SR & SPI_SR_RXNE) == 0 || (SPI5->SR & SPI_SR_BSY));
}

void ILI9341_SendCommand(uint8_t data) {
  WRX_RESET;
  CS_RESET;
  send_spi5(data);
  CS_SET;
}

void ILI9341_SendData(uint8_t data) {
  WRX_SET;
  CS_RESET;
  send_spi5(data);
  CS_SET;
}

void ILI9341_Delay(volatile uint32_t delay) {
	for (; delay != 0; delay--); 
}

void power_up_LCD()
{
  /* CS high */
  CS_SET;
  
  /* Init SPI */
  // init_SPI5();
  
  /* Initialization sequence */
  ILI9341_SendCommand(0xCA);
  ILI9341_SendData(0xC3);
  ILI9341_SendData(0x08);
  ILI9341_SendData(0x50);
  ILI9341_SendCommand(ILI9341_POWERB);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0xC1);
  ILI9341_SendData(0x30);
  ILI9341_SendCommand(ILI9341_POWER_SEQ);
  ILI9341_SendData(0x64);
  ILI9341_SendData(0x03);
  ILI9341_SendData(0x12);
  ILI9341_SendData(0x81);
  ILI9341_SendCommand(ILI9341_DTCA);
  ILI9341_SendData(0x85);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x78);
  ILI9341_SendCommand(ILI9341_POWERA);
  ILI9341_SendData(0x39);
  ILI9341_SendData(0x2C);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x34);
  ILI9341_SendData(0x02);
  ILI9341_SendCommand(ILI9341_PRC);
  ILI9341_SendData(0x20);
  ILI9341_SendCommand(ILI9341_DTCB);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendCommand(ILI9341_FRC);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x1B);
  ILI9341_SendCommand(ILI9341_DFC);
  ILI9341_SendData(0x0A);
  ILI9341_SendData(0xA2);
  ILI9341_SendCommand(ILI9341_POWER1);
  ILI9341_SendData(0x10);
  ILI9341_SendCommand(ILI9341_POWER2);
  ILI9341_SendData(0x10);
  ILI9341_SendCommand(ILI9341_VCOM1);
  ILI9341_SendData(0x45);
  ILI9341_SendData(0x15);
  ILI9341_SendCommand(ILI9341_VCOM2);
  ILI9341_SendData(0x90);
  ILI9341_SendCommand(ILI9341_MAC);
  ILI9341_SendData(0xC8);
  ILI9341_SendCommand(ILI9341_3GAMMA_EN);
  ILI9341_SendData(0x00);
  ILI9341_SendCommand(ILI9341_RGB_INTERFACE);
  ILI9341_SendData(0xC2);
  ILI9341_SendCommand(ILI9341_DFC);
  ILI9341_SendData(0x0A);
  ILI9341_SendData(0xA7);
  ILI9341_SendData(0x27);
  ILI9341_SendData(0x04);

  ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0xEF);

  ILI9341_SendCommand(ILI9341_PAGE_ADDR);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x01);
  ILI9341_SendData(0x3F);
  ILI9341_SendCommand(ILI9341_INTERFACE);
  ILI9341_SendData(0x01);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x06);

  ILI9341_SendCommand(ILI9341_GRAM);
  ILI9341_Delay(1000000);

  ILI9341_SendCommand(ILI9341_GAMMA);
  ILI9341_SendData(0x01);

  ILI9341_SendCommand(ILI9341_PGAMMA);
  ILI9341_SendData(0x0F);
  ILI9341_SendData(0x29);
  ILI9341_SendData(0x24);
  ILI9341_SendData(0x0C);
  ILI9341_SendData(0x0E);
  ILI9341_SendData(0x09);
  ILI9341_SendData(0x4E);
  ILI9341_SendData(0x78);
  ILI9341_SendData(0x3C);
  ILI9341_SendData(0x09);
  ILI9341_SendData(0x13);
  ILI9341_SendData(0x05);
  ILI9341_SendData(0x17);
  ILI9341_SendData(0x11);
  ILI9341_SendData(0x00);
  ILI9341_SendCommand(ILI9341_NGAMMA);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x16);
  ILI9341_SendData(0x1B);
  ILI9341_SendData(0x04);
  ILI9341_SendData(0x11);
  ILI9341_SendData(0x07);
  ILI9341_SendData(0x31);
  ILI9341_SendData(0x33);
  ILI9341_SendData(0x42);
  ILI9341_SendData(0x05);
  ILI9341_SendData(0x0C);
  ILI9341_SendData(0x0A);
  ILI9341_SendData(0x28);
  ILI9341_SendData(0x2F);
  ILI9341_SendData(0x0F);

  ILI9341_SendCommand(ILI9341_SLEEP_OUT);
  ILI9341_Delay(1000000);
  ILI9341_SendCommand(ILI9341_DISPLAY_ON);

  ILI9341_SendCommand(ILI9341_GRAM);
}

void display_on()
{
  ILI9341_SendCommand(ILI9341_DISPLAY_ON);
}

void display_off()
{
  ILI9341_SendCommand(ILI9341_DISPLAY_OFF);
}

void init_LCD()
{
  init_LCD_GPIOs();

  power_up_LCD();

  display_on();
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