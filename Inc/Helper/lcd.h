#ifndef LCD_H
#define LCD_H

// Initializes LCD with power-up sequence and SPI communication
void init_LCD();

// Set up LTDC peripheral
void init_LTDC();

// Outputs game startup sequence to LCD.
void startup_sequence();

// Display target on screen
void display_target(uint32_t x, uint32_t y);

// Display ball on screen
void display_ball(uint16_t ball, uint32_t x, uint32_t y);

// Display the winning screen
void display_win_screen();

#endif