#ifndef LED_H
#define LED_H

// Initialize user red and green LEDs
void init_LEDs();

// Set red LED state
void set_red_LED(const uint8_t state);
// Set green LED state
void set_green_LED(const uint8_t state);

// Update red LED timer
void update_red_LED_timer(const uint16_t time_ms);
// Update green LED timer
void update_green_LED_timer(const uint16_t time_ms);

#endif