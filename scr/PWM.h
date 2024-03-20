


#ifndef PWM_H
#define PWM_H

#include "stdint.h"


uint32_t pwm_set_freq_duty(uint slice_num, uint chan, float freq, uint32_t duty);
void GPIO_PWM_Init(uint gpio_num, float pwm_freq, uint32_t pwm_duty);

#endif