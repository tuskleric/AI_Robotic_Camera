// rangefinder.h

#ifndef RANGEFINDER_H
#define RANGEFINDER_H

#include <stdint.h>

// Function declarations
void rangefinder_init();
uint8_t calculate_checksum(uint8_t *data, int length);
float rangefinder_get_distance();
float hexArrayToFloat(const char *hexArray[], int length);

#endif // RANGEFINDER_H
