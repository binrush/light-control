#include <inttypes.h>

#define VIRTUAL_TIMERS_COUNT 4

void timer_start(uint8_t timer_id);

void timer_stop(uint8_t timer_id);

uint16_t timer_get(uint8_t timer_id);

void timers_process();
