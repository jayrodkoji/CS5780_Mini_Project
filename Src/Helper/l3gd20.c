#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include "Helper/l3gd20.h"

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