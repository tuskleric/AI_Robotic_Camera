#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

#include "stepper_control.h"
#include "PWM.h"

#define DEFAULT_MOTOR_RPM 5

#define LED_PIN 11
#define EN_PIN 14
#define DIR_PIN 10
#define NSLEEP 12
int led_value =0;

bool repeating_timer_callback(struct repeating_timer *t) {
    led_value = 1 -led_value;

    gpio_put(LED_PIN, led_value);
    return true;
}

int main() {
    
    // Initialize LED GPIO
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_init(EN_PIN);
    gpio_init(DIR_PIN);
    gpio_init(NSLEEP);

    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(EN_PIN, GPIO_OUT);
    gpio_set_dir(DIR_PIN, GPIO_OUT);
    gpio_set_dir(NSLEEP, GPIO_OUT);


    gpio_put(DIR_PIN, 1);
    gpio_put(EN_PIN, 1);
    gpio_put(NSLEEP, 1);

    
    

    struct repeating_timer timer;
    initStepperMotor(LED_PIN, DEFAULT_MOTOR_RPM);
    add_repeating_timer_ms(500, repeating_timer_callback, NULL, &timer);



    while(1) {

    }

}

