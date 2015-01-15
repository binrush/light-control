#include "timers.h"
#include <util/atomic.h>

uint16_t timers[VIRTUAL_TIMERS_COUNT] = {};

void timer_start(uint8_t timer_id) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timers[timer_id] = 1;
    }

}

void timer_stop(uint8_t timer_id) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timers[timer_id] = 0;
    }

}

uint16_t timer_get(uint8_t timer_id) {
    uint16_t value;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        value = timers[timer_id];
    }
    return value;
}

void timers_process() {
   uint8_t i;
   for ( i = 0; i < VIRTUAL_TIMERS_COUNT; i++ ) {
        if ( timers[i] > 0 && timers[i] < UINT16_MAX) {
               timers[i]++;
        }
    } 
}
