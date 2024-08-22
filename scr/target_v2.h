
#ifndef TARGET_H
#define TARGET_H

//UART
#define UART_INSTANCE  uart0 // GPIO 16 & 17
#define UART_PARITY_NONE 0

//motors

//motor X
#define STEP_X   21
#define DIR_X    22
#define EN_A     23
#define MS1A 19
#define MS1B 18

//motor Y
#define STEP_Y   10
#define DIR_Y   7

#define EN_B 13
#define MS2A 12
#define MS2B 11
#define ENABLE_12_V 29



#define SLEEP_PIN  24 //any GPIO pin, just an unused pin

//led on PICO
#define LED_PIN 25 //also unused

//encoders
    //tilt
#define ENCODER_A  14
#define ENCODER_B  15

    //pan




#endif

