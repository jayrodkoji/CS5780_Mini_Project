#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

// Initialize user red and green LEDs
void init_LEDs();

// Set red LED state
void set_red_LED(const uint8_t state);
// Set green LED state
void set_green_LED(const uint8_t state);

// Initialize USART1
void init_USART1();

// USART1 functions
void print(char* message);
void println(char* message);

// Initialize SPI5
void init_SPI5();

// L3GD20 functions
uint8_t L3GD20_ID_match();
void init_L3GD20();
void get_XY_data(int16_t* X_data, int16_t* Y_data);

#endif