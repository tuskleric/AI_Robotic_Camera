
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"


#include "stepper_control.h"
#include "PWM.h"
#include "target.h"

#define DEFAULT_FREQ 1
#define DEFAULT_DUTY 
#define SPR 200




// Function to initialize motors
void initialize_motors(trinamic_motor_t *motor_x, trinamic_motor_t *motor_y) {
    // Initialize motor_x
    motor_x->id = 1; // Example: Set motor ID
    motor_x->axis = 'X'; // Example: Set motor axis
    motor_x->address = 0; // Example: Set UART address
    motor_x->seq = 0; // Example: Set sequence number
    motor_x->cs_pin = NULL; // Example: Set CS pin data
    motor_x->dir_pin = DIR_X;
    motor_x->step_pin = STEP_X;
    
    
    // Initialize motor_y
    motor_y->id = 2; // Example: Set motor ID
    motor_y->axis = 'Y'; // Example: Set motor axis
    motor_y->address = 1; // Example: Set UART address
    motor_y->seq = 0; // Example: Set sequence number
    motor_y->cs_pin = NULL; // Example: Set CS pin data
    motor_y->dir_pin = DIR_Y;
    motor_y->step_pin = STEP_Y;

    
}

// const float DEFAULT_FREQ = 0.5;

static uint32_t RPM_to_Freqency(uint16_t motor_RPM) {
    uint32_t freq = (motor_RPM * SPR) / 60;

    return freq;

}
void initStepperMotorPWM(uint step_gpio_num, uint motor_RPM) {
    
    uint32_t motor_freq = RPM_to_Freqency(motor_RPM);

    //GPIO_PWM_Init(step_gpio_num, motor_freq, DEFAULT_DUTY);

}
void rotateStepperMotor(trinamic_motor_t *motor, uint num_ticks) {
    uint32_t start_time = time_us_32();
    uint32_t delay_enlapsed_us =0;
    gpio_put(motor->dir_pin, 1);
    for (uint i =0; i <num_ticks; i ++) {
        gpio_put(motor->step_pin,1);
        sleep_us(100);
        gpio_put(motor->step_pin,0);
        sleep_us(100);


    }
    
  
}

bool motor_step(trinamic_motor_t *motor) {
    
    gpio_put(motor->dir_pin, 1);   // temp for testing

    gpio_put(motor->step_pin,1);

    return true;

}

bool motor_step_off( trinamic_motor_t *motor) {
    
    gpio_put(motor->step_pin,0);
    return false; 

}

// void step(bool Direction){
    
//     //SET MOTOR DIRECTION
//     gpio_put(DIR_PIN, Direction);

//     //PERFORM A STEP

// }

// void setMotorTarget(int16_t angle_deg){

// }


// void rotate(int Steps, bool Direction);


