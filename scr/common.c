/*
 * common.c - shared code for Trinamic drivers
 *
 * v0.0.2 / 2021-08-05 / (c) Io Engineering / Terje
 */

/*

Copyright (c) 2021, Terje Io
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its contributors may
be used to endorse or promote products derived from this software without
specific prior written permission..

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "common.h"
#include "blink.h"


static uint8_t tmc_motors = 0;

// 1 - 256 in steps of 2^value is valid
bool tmc_microsteps_validate (uint16_t microsteps)
{
    uint_fast8_t i = 8, count = 0;

    if(microsteps <= 256) do {
        if(microsteps & 0x01)
            count++;
        microsteps >>= 1;
    } while(i--);

    return count == 1;
}

uint8_t tmc_microsteps_to_mres (uint16_t microsteps)
{
    uint8_t value = 0;

    microsteps = microsteps == 0 ? 1 : microsteps;

    while((microsteps & 0x01) == 0) {
        value++;
        microsteps >>= 1;
    }

    return 8 - (value > 8 ? 8 : value);
}

uint32_t tmc_calc_tstep (trinamic_config_t *config, float mm_sec, float steps_mm)
{
    uint32_t den = (uint32_t)(256.0f * mm_sec * steps_mm);

    return den ? (config->microsteps * config->f_clk) / den : 0;
}

float tmc_calc_tstep_inv (trinamic_config_t *config, uint32_t tstep, float steps_mm)
{
    return tstep == 0 ? 0.0f : (float)(config->f_clk * config->microsteps) / (256.0f * (float)tstep * steps_mm);
}

void tmc_motors_set (uint8_t motors)
{
    tmc_motors = motors;
}

uint8_t tmc_motors_get (void)
{
    return tmc_motors;
}


// Function to calculate the CRC
uint8_t calculate_crc(uint8_t *data, uint8_t length) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
    }
    return crc;
}


void tmc_uart_write(trinamic_motor_t driver, TMC_uart_write_datagram_t *datagram) {

    // Calculate CRC for the message (excluding the CRC byte itself)
    datagram->msg.crc = calculate_crc(datagram->data, sizeof(datagram->data) - 1);

    // Write the datagram over UART
    uart_write_blocking(UART_INSTANCE, datagram->data, sizeof(datagram->data));
    // while (1) {
    //     printf("data %02X", datagram->data);    
    // }

}

// Function to read from UART
TMC_uart_write_datagram_t *tmc_uart_read(trinamic_motor_t driver, TMC_uart_read_datagram_t *datagram) {


    static TMC_uart_write_datagram_t write_datagram; // Static allocation for returning write datagram
    


    datagram->msg.crc = calculate_crc(datagram->data, sizeof(datagram->data) - 1);
    // Write the datagram over UART
    //while(1){printf("data %x %x %x %x %x %u\n", datagram->data[0],datagram->data[1],datagram->data[2],datagram->data[3], datagram->data[4] ,sizeof(datagram->data)); gpio_put(25,1);}
    //while(1){
    uart_write_blocking(UART_INSTANCE, datagram->data, sizeof(datagram->data));
    // Calculate the CRC for the datagram
    // while(1){  
    if(uart_is_readable(UART_INSTANCE)) { 
            
    // Read data from UART into the provided buffer
        uart_read_blocking(UART_INSTANCE, write_datagram.data, sizeof(datagram->data)+4);sleep_ms(10);}
        gpio_put(25,1);
            //while(1) {printf("data %x %x %x %x %x %x %x %x %u\n", write_datagram.data[0],write_datagram.data[1],write_datagram.data[2],write_datagram.data[3], write_datagram.data[4],write_datagram.data[5],write_datagram.data[6], write_datagram.data[7],sizeof(write_datagram.data)); gpio_put(25,1);}
        //while(1) {
            //uart_write_blocking(UART_INSTANCE, write_datagram.data, sizeof(write_datagram.data));sleep_ms(10);
            
   
        
    

    write_datagram.msg.crc = calculate_crc(write_datagram.data, sizeof(datagram->data)-1);
     // Process received data if needed
    if (datagram->msg.crc != write_datagram.msg.crc) {

        //TODO write a warning blink pattern for CRC error
        //warning_blink_1();
        while(1) {
            printf("something is wrong, input crc: %d, output crc: %d\n",datagram->msg.crc, write_datagram.msg.crc);  
        }
        
    } else {
        
        return &write_datagram;
    }
}