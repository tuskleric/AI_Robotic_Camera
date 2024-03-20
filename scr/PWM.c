
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "PWM.h"

uint32_t pwm_set_freq_duty(uint slice_num, uint chan, float freq, uint32_t duty) {

    uint32_t clock = 125000000;
    // Convert frequency from Hz to PWM-friendly format
    uint32_t divider16 = clock / freq / 4096 + (clock % (uint32_t)(freq * 4096) != 0);
    if (divider16 /16 == 0) {
        divider16 = 16;
     }
    uint32_t wrap = clock * 16 / divider16/ freq - 1;
    pwm_set_clkdiv_int_frac(slice_num, divider16/16, divider16 & 0xF);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, chan, wrap * duty /100);

        return wrap;
    
    
}


void GPIO_PWM_Init(uint gpio_num, float pwm_freq, uint32_t pwm_duty) {
    
    //sets GPIO function as PWM
    gpio_set_function(gpio_num, GPIO_FUNC_PWM); // enum GPIO_FUNC_PWM = 4
    
    //find the pwm slice number for the gpio being used
    uint slice_num = pwm_gpio_to_slice_num(gpio_num);
    uint channel = pwm_gpio_to_channel(gpio_num);

    pwm_set_freq_duty(slice_num, channel, pwm_freq, pwm_duty);
    pwm_set_enabled(slice_num, true);
    
}



    


    // // Get some sensible defaults for the slice configuration. By default, the
    // // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    // pwm_config config = pwm_get_default_config();
    // // Set divider, reduces counter clock to sysclock/this value
    // pwm_config_set_clkdiv(&config, 4.f);
    // // Load the configuration into our PWM slice, and set it running.
    // pwm_init(slice_num, &config, true);
