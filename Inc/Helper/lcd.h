#ifndef LCD_H
#define LCD_H

void init_LCD();

void draw_rectangle(LTDC_Layer_TypeDef* p_layer,
                    const uint32_t x_start,
                    const uint32_t x_end,
                    const uint32_t y_start,
                    const uint32_t y_end);

void startup_sequence();


#endif