
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "stepper_control.h"
#include "PWM.h"

#define DEFAULT_FREQ 1
#define DEFAULT_DUTY 85
#define SPR 200

// const float DEFAULT_FREQ = 0.5;

static uint32_t RPM_to_Freqency(uint16_t motor_RPM) {
    uint32_t freq = (motor_RPM * SPR) / 60;

    return freq;

}
void initStepperMotorPWM(uint step_gpio_num, uint motor_RPM) {
    
    uint32_t motor_freq = RPM_to_Freqency(motor_RPM);

    GPIO_PWM_Init(step_gpio_num, motor_freq, DEFAULT_DUTY);

}
void rotateStepperMotor(uint step_gpio_num, uint num_ticks) {
    for (uint i = 0; i < num_ticks; i++) {
        gpio_put(step_gpio_num, 1);
        sleep_us(500);
        gpio_put(step_gpio_num, 0);
        sleep_us(500);
    }
}

// void step(bool Direction){
    
//     //SET MOTOR DIRECTION
//     gpio_put(DIR_PIN, Direction);

//     //PERFORM A STEP

// }

// void setMotorTarget(int16_t angle_deg){

// }


// void rotate(int Steps, bool Direction);


