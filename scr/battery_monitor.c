
#include "battery_monitor.h"
#include "hardware/adc.h"

void battery_monitor_init()
{




}

uint32_t get_temp(uint16_t temp_adc_raw)
{
	uint16_t raw;

	raw = temp_adc_raw;

	// if (raw == UINT16_MAX)
	// 	return UINT32_MAX;
	// if ((raw & ADC_FIFO_ERR_BITS) != 0)
	// 	return UINT32_MAX;
	// /*
	//  * This formula is equivalent to the temperature sensor conversion
	//  * formula given in the RP2040 datasheet and the SDK docs, except
	//  * for degrees Kelvin, and for the fixed-point Q18.14 result.
	//  */
	return 11638810 - 7670 * raw;
}
