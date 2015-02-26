#ifndef _timers_h
#define _timers_h

#include <stdint.h>
#include <stdlib.h>

int8_t timers_init(uint8_t size);

int8_t timers_destroy();

int8_t timer_start(uint8_t timer_id);

int8_t timer_stop(uint8_t timer_id);

uint16_t timer_get(uint8_t timer_id);

int8_t timers_process();

#endif
