#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/uart.h"

#include "scheduler.h"
#include "tick.h"
#include "stepper_control.h"
#include "PWM.h"
#include "battery_monitor.h"
#include "tmc2209.h"
#include "common.h"
#include "target.h"



#define ADC_TEMP_CH 4

#define DEFAULT_MOTOR_RPM 1
#define STEPS_PER_REV 200
#define STEPPING_MODE 1
#define GEAR_RATIO 3.2
#define GP19 19


// #define NSLEEP     12
// #define NRST       13

// Define constants for 12-hour cycle
#define TWELVE_HOURS_IN_SECONDS 43200

#define UART_INSTANCE  uart0
#define UART_PARITY_NONE 0

volatile bool led_state = false;
volatile bool step_active = false;
volatile bool motor_on_state = false;



/*
 * Maximum value for the ADC clock divider. Results in about 7.6 Hz, or
 * approximately one measurement every 131 ms.
 */
#define ADC_MAX_CLKDIV (65535.f + 255.f/256.f)
float voltage = 10;
const float conversion_factor = 3.3f / (1 << 12);
static int adc_avail = 0;
static void adc_isr(void)
{
	float val = adc_fifo_get();
	voltage = val * conversion_factor;
    adc_avail = 1;

}


void sleep_for(uint32_t duration_seconds) {
    // Convert duration to milliseconds and call sleep_ms()
    sleep_ms(duration_seconds * 1000);
}


// void sleep_ISR() {
//     //Interrupt Service Routine for Sleeping.
//     gpio_put(EN_A, 0);

//     // // Go to sleep until we see a high edge on GPIO 10
//     // sleep_goto_dormant_until_edge_high(17);
// }

// void awake_ISR() {
//     //Interrupt Service Routine for Sleeping.
//     gpio_put(EN_A, 1);

//     // // Go to sleep until we see a high edge on GPIO 10
//     // sleep_goto_dormant_until_edge_high(17);
// }


int position = 0;

void encoder_callback(uint gpio, uint32_t event) {
    switch (event)
    {
        case GPIO_IRQ_EDGE_FALL:
            position += (ENCODER_A == gpio)*((gpio_get(ENCODER_B)) ? -1 : 1) + (ENCODER_B == gpio)*((gpio_get(ENCODER_A)) ? 1 : -1);
            if (position >=2000) {
                position = position -2000;
            };
            if (position <= -2000) {
                position = position + 2000;
            };
            break;
        case GPIO_IRQ_EDGE_RISE:
            position += (ENCODER_A == gpio)*((gpio_get(ENCODER_B)) ? 1 : -1) + (ENCODER_B == gpio)*((gpio_get(ENCODER_A)) ? -1 : 1);
            if (position >=2000) {
                position = position -2000;
            };
            if (position <= -2000) {
                position = position + 2000;
            };
            break;
        // printf("position: %d\n",position);
    }

}





scheduler kernal;
// // Create a TMC2209 driver instance
TMC2209_t driver_X;
TMC2209_t driver_Y;
// Define any necessary global variables
trinamic_motor_t motor_x;
trinamic_motor_t motor_y;



void led_toggle_task(void) {
    led_state = !led_state;
    //gpio_put(STEP_Y, led_state);
    gpio_put(25,led_state);

}

void motor_step_task(void) {
    // switch (step_active) {
    //     case false:
    //         step_active = motor_step(&motor_x);
    //         break;
    //     case true:
    //         step_active = motor_step(&motor_x);
    // }
    motor_on_state = !motor_on_state;
    gpio_put(STEP_X, motor_on_state);

}
void get_current_task(void) {
    printf("current motor_x: %d \n",TMC2209_GetCurrent(&driver_X));
    printf("current motor_y: %d \n",TMC2209_GetCurrent(&driver_Y));
}

int main() {
    gpio_put(DIR_X,1);
    stdio_init_all();
    gpio_init(ENABLE_12_V);
    gpio_set_dir(ENABLE_12_V,GPIO_OUT);
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(GP19);
    gpio_init(STEP_Y);

    gpio_init(EN_B);
    // gpio_init(NRST);

    gpio_init(DIR_Y);
    
    gpio_init(MS2A);
    gpio_init(MS2B);
    gpio_init(STEP_X);
    gpio_init(DIR_X);
    gpio_init(EN_A);
    gpio_init(MS1A);
    gpio_init(MS1B);
    // gpio_init(NSLEEP);
    gpio_init(SLEEP_PIN);
    gpio_init(25);
    gpio_init(ENCODER_A);
    gpio_init(ENCODER_B);
    gpio_set_dir(GP19, GPIO_OUT);
    gpio_set_dir(STEP_X, GPIO_OUT);
    gpio_set_dir(STEP_Y, GPIO_OUT);
    gpio_set_dir(EN_A, GPIO_OUT);
    gpio_set_dir(EN_B, GPIO_OUT);
    // gpio_set_dir(NRST, GPIO_OUT);
    gpio_set_dir(DIR_X, GPIO_OUT);
    gpio_set_dir(DIR_Y, GPIO_OUT);
    // gpio_set_dir(NSLEEP, GPIO_OUT);
    gpio_set_dir(MS1A, GPIO_OUT);
    gpio_set_dir(MS1B, GPIO_OUT);
    gpio_set_dir(MS2A, GPIO_OUT);
    gpio_set_dir(MS2B, GPIO_OUT);
    gpio_set_dir(SLEEP_PIN, GPIO_IN);
    gpio_set_dir(25, GPIO_OUT);
    gpio_set_dir(ENCODER_A, GPIO_IN);
    gpio_set_dir(ENCODER_B, GPIO_IN);

  
    gpio_put(LED_PIN, 0);
    
    gpio_put(GP19, 0);
    gpio_put(DIR_X, 1);
    gpio_put(STEP_X, 1);
    gpio_put(DIR_Y, 1);
    gpio_put(STEP_Y, 1);
    gpio_put(EN_A, 0);
    gpio_put(EN_B, 0);
    gpio_put(MS1A, 0);
    gpio_put(MS1B, 0);
    gpio_put(MS2A, 1);
    gpio_put(MS2B, 0);
    // gpio_put(NSLEEP, 1);
    // gpio_put(NRST, 1);

       // Initialise UART 0
    // Initialize the UART with a baud rate (e.g., 9600)
    uart_init(UART_INSTANCE, 115200);

    // Set UART format (8 data bits, 1 stop bit, no parity)
    uart_set_format(UART_INSTANCE, 8, 1, UART_PARITY_NONE);
    
     // Set the GPIO pin mux to the UART - 16 is TX, 17 is RX
    gpio_set_function(16, GPIO_FUNC_UART);
    gpio_set_function(17, GPIO_FUNC_UART);
    // took soo long to find but this stops default UART0 pins interfaring with stuff
    gpio_set_function(0, GPIO_FUNC_NULL);
    gpio_set_function(1, GPIO_FUNC_NULL);
    
    // uart_set_fifo_enabled(UART_INSTANCE, 1);


    //power to motor drivers
    gpio_put(ENABLE_12_V,1);
     initialize_motors(&motor_x, &motor_y);

    // // Get default configuration parameters
    // const trinamic_cfg_params_t *default_params = TMC2209_GetConfigDefaults();

    // // Initialize the driver instance with default values
    // TMC2209_SetDefaults(&driver_X);
    // TMC2209_SetDefaults(&driver_Y);
    // driver_X.config.motor = motor_x;
    // driver_Y.config.motor = motor_y;
    // // Initialize the drivers 
    // TMC2209_datagram_t test_datagram;
    // test_datagram.payload.ioin.ms1;
    
    
    
    // //while(1) {printf("success: dir = %x \n",test_datagram.payload.ioin.dir);}
    // bool init_success_x = TMC2209_Init(&driver_X);
    // bool init_success_y = TMC2209_Init(&driver_Y);
    
    // TMC2209_SetMicrosteps(&driver_X, TMC2209_Microsteps_128);
    // TMC2209_SetCurrent (&driver_X, 100, 50);
    // TMC2209_SetCurrent (&driver_Y, 100, 50);

    gpio_set_irq_enabled_with_callback(ENCODER_A,0x4|0x8,1, encoder_callback);
    gpio_set_irq_enabled_with_callback(ENCODER_B,0x4|0x8,1, encoder_callback);
    sleep_ms(5000);
    gpio_put(25,1);
  

    

 
    //initStepperMotorPWM(STEP_X, DEFAULT_MOTOR_RPM*STEPPING_MODE);
    // Set up interrupt handler for the button press
    //gpio_set_irq_enabled(SLEEP_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    //gpio_set_irq_enabled_with_callback(SLEEP_PIN, GPIO_IRQ_EDGE_FALL, true, &sleep_ISR);
    //gpio_set_irq_enabled_with_callback(SLEEP_PIN, GPIO_IRQ_EDGE_RISE, true, &awake_ISR);
    // bool on_state = true;
    // const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    kernal.tickPeriod = 100;  // in microseconds ----> 10KHz
    kernal_init();



    //task frequency must be less then 10KHz
    struct repeating_timer timer;
    add_repeating_timer_us(kernal.tickPeriod, alarm_callback, NULL, &timer);
    taskId_t led_toggle_task_id = register_task(led_toggle_task,1,5);

    //taskId_t motor_step_task_id = register_task(motor_step_task,2,500);
    //taskId_t get_current_task_id = register_task(get_current_task,3,1000000);
    while(1) {
         
        
       kernal_start();
       printf("success");
    
        // printf("current: %d\n", TMC2209_GetCurrent (&driver_X));
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
         
        // uint16_t result = adc_read();
        // if (adc_avail) {
        //     //printf("voltage: %f \n", voltage );
        //     adc_avail=0;
        // };
        // sleep_ms(500);
        // gpio_put(LED_PIN, 1);
        // sleep_ms(250);
        // gpio_put(LED_PIN, 0);
        
        // sleep_ms(250);
        // if (on_state & gpio_get(SLEEP_PIN)) {
        //     gpio_put(EN_PIN, 0);
        //     gpio_put(25, 0);
        //     on_state = false;
        // } else if (!on_state & !gpio_get(SLEEP_PIN)) {
        //     gpio_put(EN_PIN, 1);
        //     gpio_put(25, 1);
        //     on_state = true;
        // }
        
       // rotateStepperMotor(&motor_x, 51200);//STEPPING_MODE*STEPS_PER_REV*GEAR_RATIO/10);
        //rotateStepperMotor(&motor_y, 51200);//STEPPING_MODE*STEPS_PER_REV*GEAR_RATIO/10);

        //sleep_ms(10000);

        // if (gpio_get(SLEEP_PIN) == 0) {
        //     sleep_ms(100000);
        //     //gpio_put(EN_PIN, 0);
        // }

    }

}

