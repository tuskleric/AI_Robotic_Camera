
#include <stdlib.h>

#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "stepper_control.h"
#include "PWM.h"

#define DEFAULT_FREQ 0.5
#define DEFAULT_DUTY 50
// const float DEFAULT_FREQ = 0.5;

void initStepperMotor(uint step_gpio_num) {
    

    GPIO_PWM_Init(step_gpio_num, DEFAULT_FREQ, DEFAULT_DUTY);

    




}

// void step(bool Direction){
    
//     //SET MOTOR DIRECTION
//     gpio_put(DIR_PIN, Direction);

//     //PERFORM A STEP

// }

// void setMotorTarget(int16_t angle_deg){

// }


// void rotate(int Steps, bool Direction);


