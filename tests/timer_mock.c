#include <timers.h>

static uint16_t fake_timer = 0;

int8_t timers_init(uint8_t size)
{
    return 0;
}

int8_t timers_destroy()
{
    return 0;
}

int8_t timer_start(uint8_t timer_id)
{
    fake_timer = 1;
    return 0;
}

int8_t timer_stop(uint8_t timer_id)
{
    fake_timer = 0;
    return 0;
}

uint16_t timer_get(uint8_t timer_id)
{
    return fake_timer;
}

int8_t timers_process()
{
    return 0;
}

void timer_set(uint16_t value) {
    if  ( fake_timer > 0 ) fake_timer = value;
}
