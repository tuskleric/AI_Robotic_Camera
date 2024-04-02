#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"


#include "stepper_control.h"
#include "PWM.h"

#define DEFAULT_MOTOR_RPM 100

#define DIR_PIN   10
#define STEP_PIN   11
#define NSLEEP   12
#define NRST   13
#define EN_PIN 15
#define SLEEP_PIN 17//any GPIO pin

// Define constants for 12-hour cycle
#define TWELVE_HOURS_IN_SECONDS 43200


void sleep_for(uint32_t duration_seconds) {
    // Convert duration to milliseconds and call sleep_ms()
    sleep_ms(duration_seconds * 1000);
}


void sleep_ISR() {
    //Interrupt Service Routine for Sleeping.
    gpio_put(EN_PIN, 0);
    initStepperMotor(STEP_PIN, 0);

    // // Go to sleep until we see a high edge on GPIO 10
    // sleep_goto_dormant_until_edge_high(17);
}


int main() {

    // Initialize LED GPIO
    stdio_init_all();
    cyw43_arch_init();

    gpio_init(STEP_PIN);
    gpio_init(EN_PIN);
    gpio_init(NRST);
    gpio_init(DIR_PIN);
    gpio_init(NSLEEP);
    gpio_init(SLEEP_PIN);

    gpio_set_dir(STEP_PIN, GPIO_OUT);
    gpio_set_dir(EN_PIN, GPIO_OUT);
    gpio_set_dir(NRST, GPIO_OUT);
    gpio_set_dir(DIR_PIN, GPIO_OUT);
    gpio_set_dir(NSLEEP, GPIO_OUT);
    gpio_set_dir(SLEEP_PIN, GPIO_IN);


    gpio_put(DIR_PIN, 1);
    gpio_put(EN_PIN, 0);
    gpio_put(NSLEEP, 1);
    gpio_put(NRST, 1);

    
    

 
    initStepperMotor(STEP_PIN, DEFAULT_MOTOR_RPM);
    // Set up interrupt handler for the button press
    gpio_set_irq_enabled_with_callback(SLEEP_PIN, GPIO_IRQ_EDGE_FALL, true, &sleep_ISR);



    while(1) {
        
        
        

        // if (gpio_get(SLEEP_PIN) == 0) {
        //     sleep_ms(100000);
        //     //gpio_put(EN_PIN, 0);
        // }

    }

}

