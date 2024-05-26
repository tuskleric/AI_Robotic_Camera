#ifndef TICK_H
#define TICK_H

#include <stdint.h>


#define TIMER_NOT_EXPIRED 0U
#define TIMER_EXPIRED     1U
#define TIMER_NOT_ENABLED 2U

#define SINGLE_MODE    0U //Timer fires once 
#define CONTINOUS_MODE 1U //Timer will restart it's self once elapsed

typedef struct
{
	uint32_t u32_tick_count;
}sys_timer;

typedef struct
{
	uint32_t u32_t0_ms;       //Stores start time 
	uint32_t u32_timer_period; 
	uint8_t u8_mode;
	bool timer_enabled;

}systick_handler_t;

void init_systick();
uint32_t get_systick();
void init_non_blocking_timer(systick_handler_t *timer, uint32_t time_period, uint8_t u8_user_mode);
void start_non_blocking_timer(systick_handler_t *timer);
void stop_non_blocking_timer(systick_handler_t *timer);
uint8_t non_blocking_timer_expired(systick_handler_t *timer);  

#endif 