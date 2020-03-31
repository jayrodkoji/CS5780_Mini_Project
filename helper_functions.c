#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "helper_functions.h"

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

void init_SPI5()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable GPIOC clock

  // Setup PC1 to be an input
  // Physically connect PF6(NSS) to PC1 on the board.
  GPIOC->MODER &= ~GPIO_MODER_MODE1_Msk;

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
  SPI5->CR1 &= ~SPI_CR1_BR_Msk;
  SPI5->CR2 |= SPI_CR2_FRF; // TI mode
  SPI5->CR1 |= SPI_CR1_DFF; // 16-bit data format
  SPI5->CR1 &= ~SPI_CR1_LSBFIRST; // MSB first
  SPI5->CR1 |= SPI_CR1_MSTR; // Master mode
  SPI5->CR1 |= SPI_CR1_SPE; // Enable
}

void read_L3GD20_register(uint8_t address, uint8_t* value)
{
  while(SPI5->SR & SPI_SR_BSY); // Wait until not busy
  while(!(SPI5->SR & SPI_SR_TXE)); // Wait for transmit empty
  SPI5->DR = (address | (1 << 7)) << 8; // Read from address
  while(!(SPI5->SR & SPI_SR_TXE)); // Wait for transmit empty
  while(!(SPI5->SR & SPI_SR_RXNE)); // Wait for receive not empty
  *value = SPI5->DR & 0xFF;
}

void write_L3GD20_register(uint8_t address, uint8_t value)
{
  while(SPI5->SR & SPI_SR_BSY); // Wait until not busy
  while(!(SPI5->SR & SPI_SR_TXE)); // Wait for transmit empty
  SPI5->DR = (address << 8) | value; // Write to address
  while(!(SPI5->SR & SPI_SR_TXE)); // Wait for transmit empty
  while(!(SPI5->SR & SPI_SR_RXNE)); // Wait for receive not empty
}

uint8_t L3GD20_ID_match()
{
  uint8_t ID;
  read_L3GD20_register(0x0F, &ID);

  if (0xD4 == ID)
  {
    return 1;
  }
  return 0;
}

void init_L3GD20()
{
  write_L3GD20_register(0x20, 0xB); // Enable X and Y axis at 95 Hz
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