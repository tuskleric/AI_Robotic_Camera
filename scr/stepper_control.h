
/*
Authored by: Joel Suddaby
Date Created: 18/03/2024



*/

#ifndef STEPPER_CONTROL_H
#define STEPPER_CONTROL_H


// #includes
#include "stdint.h"


uint32_t RPM_to_Freqency(uint16_t motor_RPM);

void initStepperMotor(uint step_gpio_num, uint motor_RPM);
// void step(bool Direction);
// void rotate(int Steps, bool Direction);





#endif