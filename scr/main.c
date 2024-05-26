#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/uart.h"


#include "stepper_control.h"
#include "PWM.h"
#include "battery_monitor.h"
#include "tmc2209.h"
#include "common.h"
#include "tick.h"
#include "scheduler.h"


#define ADC_TEMP_CH 4

#define DEFAULT_MOTOR_RPM 1
#define STEPS_PER_REV 200
#define STEPPING_MODE 1
#define GEAR_RATIO 3.2

#define DIR_PIN    10
#define STEP_PIN   11
#define NSLEEP     12
#define NRST       13
#define EN_PIN     15
#define SLEEP_PIN  17 //any GPIO pin

#define ENCODER_A  7
#define ENCODER_B  8
// Define constants for 12-hour cycle
#define TWELVE_HOURS_IN_SECONDS 43200

#define UART_INSTANCE  uart1
#define UART_PARITY_NONE 0

const uint LED_PIN = PICO_DEFAULT_LED_PIN;



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

void systick_callback() {
    //interrupt_Handler here.
}


systick_handler_t led_timer;

void toggle_led(void)
{
    printf("getting to here");
    gpio_put(LED_PIN, !gpio_get(LED_PIN));

}

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
    }

}
    scheduler kernal; 
    
    
int main() {
    stdio_init_all();

    // Initialise UART 0
    // Initialize the UART with a baud rate (e.g., 9600)
    uart_init(UART_INSTANCE, 9600);

    // Set UART format (8 data bits, 1 stop bit, no parity)
    uart_set_format(UART_INSTANCE, 8, 1, UART_PARITY_NONE);
    
     // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);
    // Create a TMC2209 driver instance
    TMC2209_t driver_instance;

    // Get default configuration parameters
    const trinamic_cfg_params_t *default_params = TMC2209_GetConfigDefaults();

    // Initialize the driver instance with default values
    //TMC2209_SetDefaults(&driver_instance);

    // Initialize the driver
    //bool init_success = TMC2209_Init(&driver_instance);
        // Set the desired VACTUAL value
    //uint32_t newVACTUAL = 11930; // Set to your desired value

    // // Prepare the datagram
    // TMC2209_datagram_t vactual_datagram;
    // vactual_datagram.addr.reg = TMC2209Reg_VACTUAL;
    // vactual_datagram.payload.vactual.value = newVACTUAL;
    // vactual_datagram.payload.tpwmthrs.value = 1406;

    // // Write the updated VACTUAL value to the driver
    // bool success = TMC2209_WriteRegister(&driver_instance, &vactual_datagram);
    
    


    gpio_init(STEP_PIN);
    gpio_init(EN_PIN);
    gpio_init(NRST);
    gpio_init(DIR_PIN);
    gpio_init(NSLEEP);
    gpio_init(SLEEP_PIN);
    gpio_init(25);
    gpio_init(ENCODER_A);
    gpio_init(ENCODER_B);
    gpio_init(LED_PIN);
    gpio_set_dir(STEP_PIN, GPIO_OUT);
    gpio_set_dir(EN_PIN, GPIO_OUT);
    gpio_set_dir(NRST, GPIO_OUT);
    gpio_set_dir(DIR_PIN, GPIO_OUT);
    gpio_set_dir(NSLEEP, GPIO_OUT);
    gpio_set_dir(SLEEP_PIN, GPIO_IN);
    gpio_set_dir(25, GPIO_OUT);
    gpio_set_dir(ENCODER_A, GPIO_IN);
    gpio_set_dir(ENCODER_B, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(DIR_PIN, 1);
    gpio_put(STEP_PIN, 0);
    gpio_put(EN_PIN, 0);
    gpio_put(NSLEEP, 1);
    gpio_put(NRST, 1);

   

 
    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(12, GPIO_FUNC_UART);
    gpio_set_function(13, GPIO_FUNC_UART);

    uart_set_fifo_enabled(UART_INSTANCE, 1);
    uart_putc(uart0, 0x4);

        	/* Initiate asynchronous ADC temperature sensor reads */
	adc_init();
    adc_gpio_init(26);
	adc_select_input(0);
	adc_irq_set_enabled(true);
    /* Write to FIFO length 1, and retain the ERR bit. */
	adc_fifo_setup(true, false, 1, false, true);
	adc_set_clkdiv(ADC_MAX_CLKDIV);
    
	irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_isr);
	
	irq_set_enabled(ADC_IRQ_FIFO, true);
	adc_run(true);
    
    gpio_set_irq_enabled_with_callback(ENCODER_A,0x4|0x8,1, encoder_callback);
    gpio_set_irq_enabled_with_callback(ENCODER_B,0x4|0x8,1, encoder_callback);
    sleep_ms(5000);


    

 
    //initStepperMotorPWM(STEP_PIN, DEFAULT_MOTOR_RPM*STEPPING_MODE);
    // Set up interrupt handler for the button press
    //gpio_set_irq_enabled(SLEEP_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    //gpio_set_irq_enabled_with_callback(SLEEP_PIN, GPIO_IRQ_EDGE_FALL, true, &sleep_ISR);
    //gpio_set_irq_enabled_with_callback(SLEEP_PIN, GPIO_IRQ_EDGE_RISE, true, &awake_ISR);
    bool on_state = true;

    // init_non_blocking_timer(&led_timer, 1000, CONTINOUS_MODE);
    // init_systick();

    // start_non_blocking_timer(&led_timer);

    kernal.tickPeriod = 10000;
    
    kernal_init();

    struct repeating_timer timer;
    add_repeating_timer_us(kernal.tickPeriod, alarm_callback, NULL, &timer);
    taskId_t task1_id = register_task(toggle_led, 1, 200000); // Task 1 with priority 1 and period 200000 us (200 ms)

    while(1) {
    //     if (task1_id.Id == 0 && task1_id.Id < MAX_TASKS) {
    //     printf("kernal_task period confirm %lld us\n", kernal.tasks[0].period);
    // } else {
    //     printf("Failed to register task\n");
    // }
        kernal_start();
        // if (non_blocking_timer_expired(&led_timer) == TIMER_EXPIRED) 
        // {
        //     toggle_led();
        // }
        // //printf("current: %d\n", TMC2209_GetCurrent (&driver_instance));
        // // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
         
        // // uint16_t result = adc_read();
        // if (adc_avail) {
        //     printf("voltage: %f \n", voltage );
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
        
        //rotateStepperMotor(STEP_PIN, 51200);//STEPPING_MODE*STEPS_PER_REV*GEAR_RATIO/10);
        //sleep_ms(10000);

        // if (gpio_get(SLEEP_PIN) == 0) {
        //     sleep_ms(100000);
        //     //gpio_put(EN_PIN, 0);
        // }

    }

}

