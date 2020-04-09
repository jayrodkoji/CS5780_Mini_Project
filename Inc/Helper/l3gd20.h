#ifndef L3GD20_H
#define L3DG20_H

// L3GD20 functions
uint8_t L3GD20_ID_match();
void init_L3GD20();
void get_XY_data(int16_t* X_data, int16_t* Y_data);

#endif