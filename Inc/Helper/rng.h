#ifndef RNG_H
#define RNG_H

#include "stdint.h"

// Initialize random number generator
void init_RNG();

// Returns a random number
uint32_t get_random_number();

#endif