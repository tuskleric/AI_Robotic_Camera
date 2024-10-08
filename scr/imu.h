#ifndef IMU_H
#define IMU_H

#include <stdint.h>

// Function Prototypes
void imu_init(void);
float get_heading(void);
float get_roll(void);
float get_pitch(void);

#endif // IMU_H
