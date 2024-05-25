#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"

#include "stepper_control.h"
#include "PWM.h"

#define DEFAULT_MOTOR_RPM 1
#define STEPS_PER_REV 200
#define STEPPING_MODE 32
#define GEAR_RATIO 30

#define DIR_PIN    10
#define STEP_PIN   11
#define NSLEEP     12
#define NRST       13
#define EN_PIN     15
#define SLEEP_PIN  17 //any GPIO pin
#define ENCODERA   20
#define ENCODERB   19
#define LED_PIN    PICO_DEFAULT_LED_PIN

// Define constants for 12-hour cycle
#define TWELVE_HOURS_IN_SECONDS 43200


void sleep_for(uint32_t duration_seconds) {
    // Convert duration to milliseconds and call sleep_ms()
    sleep_ms(duration_seconds * 1000);
}


void sleep_ISR() {
    //Interrupt Service Routine for Sleeping.
    gpio_put(EN_PIN, 0);

    // // Go to sleep until we see a high edge on GPIO 10
    // sleep_goto_dormant_until_edge_high(17);
}

void awake_ISR() {
    //Interrupt Service Routine for Sleeping.
    gpio_put(EN_PIN, 1);

    // // Go to sleep until we see a high edge on GPIO 10
    // sleep_goto_dormant_until_edge_high(17);
}

int position = 0;

void encoder_callback(uint gpio, uint32_t event) {
    switch (event)
    {
        case GPIO_IRQ_EDGE_FALL:
            position += (ENCODERA == gpio)*((gpio_get(ENCODERB)) ? -1 : 1) + (ENCODERB == gpio)*((gpio_get(ENCODERA)) ? 1 : -1);
            break;
        case GPIO_IRQ_EDGE_RISE:
            position += (ENCODERA == gpio)*((gpio_get(ENCODERB)) ? 1 : -1) + (ENCODERB == gpio)*((gpio_get(ENCODERA)) ? -1 : 1);
            break;
    }

}

uint8_t tester[2];

void I2CIRQHandler() {

    uint32_t intr_stat = i2c0->hw->intr_stat;
    i2c_read_blocking (i2c_default, 0x15, &tester, 2, true);
    
    // if (intr_stat & I2C_IC_INTR_STAT_R_RX_FULL_BITS) {
    //     i2c_read_blocking (i2c_default, 0x15, &tester, 1, true);
    // }

    // if (intr_stat & I2C_IC_INTR_STAT_R_TX_ABRT_BITS) {
    //     i2c_write_blocking (i2c_default, 0x15, &tester, 1, true);
    // }
}


int main() {

    // Initialize LED GPIO
    stdio_init_all();

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    i2c_set_slave_mode (i2c_default, true, 0x15);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    gpio_init(STEP_PIN);
    gpio_init(EN_PIN);
    gpio_init(NRST);
    gpio_init(DIR_PIN);
    gpio_init(NSLEEP);
    gpio_init(SLEEP_PIN);
    gpio_init(ENCODERA);
    gpio_init(ENCODERB);
    

    gpio_set_dir(STEP_PIN, GPIO_OUT);
    gpio_set_dir(EN_PIN, GPIO_OUT);
    gpio_set_dir(NRST, GPIO_OUT);
    gpio_set_dir(DIR_PIN, GPIO_OUT);
    gpio_set_dir(NSLEEP, GPIO_OUT);
    gpio_set_dir(SLEEP_PIN, GPIO_IN);
    gpio_set_dir(ENCODERA, GPIO_IN);
    gpio_set_dir(ENCODERB, GPIO_IN);


    gpio_put(DIR_PIN, 1);
    gpio_put(STEP_PIN, 0);
    gpio_put(EN_PIN, 0);
    gpio_put(NSLEEP, 1);
    gpio_put(NRST, 1);

    sleep_ms(5000);

    
    

 
    //initStepperMotorPWM(STEP_PIN, DEFAULT_MOTOR_RPM*STEPPING_MODE);
    // Set up interrupt handler for the button press
    bool on_state = true;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(ENCODERA, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, encoder_callback);

    gpio_set_irq_enabled_with_callback(ENCODERB, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, encoder_callback);
    

    // Enable the I2C interrupts we want to process
    i2c0->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);

    // Set up the interrupt handler to service I2C interrupts
    irq_set_exclusive_handler(I2C0_IRQ, I2CIRQHandler);
        
    // Enable I2C interrupt
    irq_set_enabled(I2C0_IRQ, true);

    while(1) {

        printf("%d %d\n", tester, position);
        uint16_t dist = 0;
        dist = (tester[1] << 8) | tester[0];
        rotateStepperMotor(STEP_PIN, STEPPING_MODE*STEPS_PER_REV*dist/360);
        tester[0] = 0;
        tester[1] = 0;
        sleep_ms(500);

        // if (!on_state & gpio_get(SLEEP_PIN)) {
        //     gpio_put(EN_PIN, 0);
        //     gpio_put(25, 0);
        //     on_state = true;
        // } else if (on_state & !gpio_get(SLEEP_PIN)) {
        //     gpio_put(EN_PIN, 1);
        //     gpio_put(25, 1);
        //     on_state = false;
        // }

        // if (on_state) {
        //     gpio_put(EN_PIN, 0);
        //     rotateStepperMotor(STEP_PIN, STEPPING_MODE*STEPS_PER_REV*GEAR_RATIO/10);
        //     gpio_put(EN_PIN, 1);
        //     sleep_ms(10000);
        // }

        // if (gpio_get(SLEEP_PIN) == 0) {
        //     sleep_ms(100000);
        //     //gpio_put(EN_PIN, 0);
        // }

    }

}

