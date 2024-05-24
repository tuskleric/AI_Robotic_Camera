
/*
Authored by: Joel Suddaby
Date Created: 18/03/2024



*/

#ifndef STEPPER_CONTROL_H
#define STEPPER_CONTROL_H


// #includes
#include "stdint.h"
#include "common.h"

// Function prototypes
void initialize_motors(void);
static uint32_t RPM_to_Freqency(uint16_t motor_RPM);

void initStepperMotorPWM(uint step_gpio_num, uint motor_RPM);
void rotateStepperMotor(uint step_gpio_num, uint num_ticks);
// void step(bool Direction);
// void rotate(int Steps, bool Direction);





#endif