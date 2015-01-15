#include <inttypes.h>

#define VIRTUAL_TIMERS_COUNT 2
#define TIMER_BUTTON0 0
#define TIMER_ROOM 1

void timer_start(uint8_t timer_id);

void timer_stop(uint8_t timer_id);

uint16_t timer_get(uint8_t timer_id);

void timers_process();
