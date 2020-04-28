#ifndef L3GD20_H
#define L3DG20_H

// Returns 1 if L3GD20 ID matches the datasheet.
uint8_t L3GD20_ID_match();

// Initializes SPI5 for L3GD20.
void init_L3GD20();

// Gets XY data from L3GD20
void get_XY_data(int16_t* X_data, int16_t* Y_data);

#endif