#include "imu.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>

#define I2C_PORT i2c0
static int addr = 0x28;

static float heading = 0.0f;
static float roll = 0.0f;
static float pitch = 0.0f;

// Initialise BNO055 IMU
void imu_init(void) {
    sleep_ms(1000); // Delay for BNO055 boot up
    uint8_t reg = 0x00;
    uint8_t chipID[1];

    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, addr, chipID, 1, false);

    if (chipID[0] != 0xA0) {
        while (1) {
            printf("Chip ID Not Correct - Check Connection!\n");
            sleep_ms(5000);
        }
    }

    uint8_t data[2];

    // Use internal oscillator
    data[0] = 0x3F;
    data[1] = 0x40;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);

    // Reset all interrupt status bits
    data[0] = 0x3F;
    data[1] = 0x01;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);

    // Configure Power Mode
    data[0] = 0x3E;
    data[1] = 0x00;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(50);

    // Set units to degrees
    data[0] = 0x3B;
    data[1] = 0x00;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(30);

    // Set operation mode to Compass (only use magnetometer)
    data[0] = 0x3D;
    data[1] = 0x09; // 0x09 = Compass mode
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(100);
}

// Update IMU readings (specifically for compass mode)
static void imu_update(void) {
    uint8_t euler[6];
    int16_t raw_heading, raw_roll, raw_pitch;
    uint8_t val = 0x1A;  // Euler angles starting at 0x1A

    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);
    i2c_read_blocking(I2C_PORT, addr, euler, 6, false);

    raw_heading = (int16_t)((euler[1] << 8) | euler[0]);

    heading = raw_heading / 16.0f;  // Compass heading (in degrees)
}

// Getter functions for IMU readings
float get_heading(void) {
    imu_update();
    return heading;
}

float get_roll(void) {
    imu_update();
    return roll;
}

float get_pitch(void) {
    imu_update();
    return pitch;
}

//function to get compass heading
// Not actually certain that this is correct, might be worth checking out IMU datasheet and Adafruit forums
// https://forums.adafruit.com/viewtopic.php?t=201547
float get_compass_heading(void) {
    return get_heading();  // Heading in degrees (0 to 360)
}


