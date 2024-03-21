
#include <stdlib.h>

#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "stepper_control.h"
#include "PWM.h"

#define DEFAULT_FREQ 1
#define DEFAULT_DUTY 50
#define SPR 200

// const float DEFAULT_FREQ = 0.5;

uint32_t RPM_to_Freqency(uint16_t motor_RPM) {
    uint32_t freq = (motor_RPM * SPR) / 60;

    return freq;

}
void initStepperMotor(uint step_gpio_num, uint motor_RPM) {
    
    uint32_t motor_freq = RPM_to_Freqency(motor_RPM);

    GPIO_PWM_Init(step_gpio_num, motor_freq, DEFAULT_DUTY);

    




}

// void step(bool Direction){
    
//     //SET MOTOR DIRECTION
//     gpio_put(DIR_PIN, Direction);

//     //PERFORM A STEP

// }

// void setMotorTarget(int16_t angle_deg){

// }


// void rotate(int Steps, bool Direction);


