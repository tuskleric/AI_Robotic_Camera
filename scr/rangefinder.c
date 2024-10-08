// rangefinder.c

#include "rangefinder.h"
#include <stdio.h>
#include <stdlib.h>  // For atof() function
#include "pico/stdlib.h"
#include "hardware/uart.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

// Function to convert an array of hex strings to a float
float hexArrayToFloat(const char *hexArray[], int length) {
    // Allocate a buffer to hold the resulting string (assuming each hex code is 2 characters)
    char str[length + 1];
    
    // Convert the hex array to characters and construct a string
    for (int i = 0; i < length; i++) {
        // Convert hex string to integer, then to character
        int value;
        sscanf(hexArray[i], "%x", &value);  // Convert hex string to integer
        str[i] = (char)value;               // Convert integer to ASCII character
    }
    
    // Null terminate the string
    str[length] = '\0';
    
    // Convert the string to a float
    float result = strtof(str, NULL);
    
    return result;
}

void rangefinder_init() {
    uart_init(uart1, 9600);  // Initialize UART0 at 9600 baud for the sensor
    gpio_set_function(8, GPIO_FUNC_UART);  // Set GPIO 0 as UART TX
    gpio_set_function(9, GPIO_FUNC_UART);  // Set GPIO 1 as UART RX

    // Turn on the sensor
    char turnOn[] = {0x80, 0x06, 0x05, 0x01, 0x74};
    uart_write_blocking(uart1, (uint8_t *)turnOn, sizeof(turnOn));
    sleep_ms(200);
}

// Function to calculate checksum
uint8_t calculate_checksum(uint8_t *data, int length) {
    uint8_t checksum = 0;
    for (int i = 0; i < length; i++) {
        checksum += data[i];
    }
    return ~checksum + 1;  // Return the two's complement (inverse + 1)
}

// Function to take a measurement and return the distance
float rangefinder_get_distance() {
    // Command to take a single measurement
    char singleMeasurementCommand[] = {0x80, 0x06, 0x02, 0x78};
    // Command to read cached data
    char readCacheCommand[] = {0x80, 0x06, 0x07, 0x73};

    uint8_t data[11] = {0};

    // 1. Send the single measurement command
    uart_write_blocking(uart0, (uint8_t *)singleMeasurementCommand, sizeof(singleMeasurementCommand));

    // Give the sensor a short delay to perform the measurement
    sleep_ms(200);

    // 2. Send the read cache command
    uart_write_blocking(uart0, (uint8_t *)readCacheCommand, sizeof(readCacheCommand));

    // 3. Read the 11-byte response from the sensor
    if (uart_is_readable_within_us(uart0, 100000)) {  // Wait for data to be readable
        for (int i = 0; i < 11; i++) {
            data[i] = uart_getc(uart0);
        }

        // Print the raw data for debugging
        printf("Raw Data: ");
        for (int i = 0; i < 11; i++) {
            printf("%02X ", data[i]);  // Print in hexadecimal format
        }
        printf("\n");

        // 4. Calculate checksum from the first 10 bytes
        uint8_t calculatedChecksum = calculate_checksum(data, 10);
        printf("Calculated Checksum: %02X, Received Checksum: %02X\n", calculatedChecksum, data[10]);

        // 5. Check if the checksum is valid
        if (data[10] == calculatedChecksum) {
            // If there is no error in the data, parse the distance value
            if (data[3] == 'E' && data[4] == 'R' && data[5] == 'R') {
                printf("Error: %c%c%c\n", data[3], data[4], data[5]);  // Handle error case
                return -1.0;  // Indicate an error
            } else {
                // Extract the relevant ASCII characters from data[] for the distance string
                char distanceStr[8] = {0};  // String to hold the distance in ASCII
                distanceStr[0] = data[2];  // First digit
                distanceStr[1] = data[3];  // Second digit
                distanceStr[2] = data[4];  // Third digit
                distanceStr[3] = data[5];      // Decimal point
                distanceStr[4] = data[6];  // First decimal digit
                distanceStr[5] = data[7];  // Second decimal digit
                distanceStr[6] = data[8];  // Third decimal digit
                distanceStr[7] = '\0';     // Null terminate the string

                // Convert the ASCII string to a float using strtof
                float distance = strtof(distanceStr, NULL);
                
                printf("Distance = %s M\n", distanceStr);  // Print the distance value
                printf("Variable Distance = %f M\n", distance);  // Print the distance value
                gpio_put(LED_PIN, 1);  // Turn on LED to indicate valid reading
                return distance;
            }
        } else {
            // If checksum is incorrect, print "Invalid Data!"
            printf("Invalid Data!\n");
            return -1.0;  // Indicate an error
        }
    }
    return -1.0;  // Indicate an error if no data was readable
}



