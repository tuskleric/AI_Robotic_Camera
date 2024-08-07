#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "pico/stdlib.h"
#include "hardware/adc.h"



uint32_t get_temp(uint16_t temp_adc_raw);
void battery_monitor_init();


#endif //BATTERY_MONITOR_H