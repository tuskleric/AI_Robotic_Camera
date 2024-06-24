
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
void initialize_motors(trinamic_motor_t *motor_x, trinamic_motor_t *motor_y);
static uint32_t RPM_to_Freqency(uint16_t motor_RPM);

void initStepperMotorPWM(uint step_gpio_num, uint motor_RPM);
void rotateStepperMotor(trinamic_motor_t *motor, uint num_ticks);
bool motor_step( trinamic_motor_t *motor);
bool motor_step_off( trinamic_motor_t *motor);
// void step(bool Direction);
// void rotate(int Steps, bool Direction);





#endif