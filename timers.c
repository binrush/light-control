#include "timers.h"
#include <util/atomic.h>

typedef struct {
    uint8_t size;
    uint16_t *data;
} Timers;

Timers *timers = NULL;

int8_t timers_init(uint8_t size)
{
    timers = calloc(1, sizeof(Timers));
    if ( timers == NULL ) return -1;
    timers->size = size;
    timers->data = calloc(size, sizeof(uint16_t));
    if ( timers->data == NULL ) {
        free(timers);
        return -1;
    }
    return 0;
}

int8_t timers_destroy() {
    free(timers->data);
    free(timers);
    return 0;
}

int8_t timer_start(uint8_t timer_id) {
    if ( timer_id >= timers->size || timers == NULL ) return -1;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timers->data[timer_id] = 1;
    }
    return 0;
}

int8_t timer_stop(uint8_t timer_id) {
    if ( timer_id >= timers->size || timers == NULL ) return -1;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timers->data[timer_id] = 0;
    }
    return 0;
}

uint16_t timer_get(uint8_t timer_id) {
    if ( timer_id >= timers->size || timers == NULL ) return 0;
    uint16_t value;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        value = timers->data[timer_id];
    }
    return value;
}

int8_t timers_process() {
    if ( timers == NULL ) return -1;
    uint8_t i;
    for ( i = 0; i < timers->size ; i++ ) {
        if ( timers->data[i] > 0 && timers->data[i] < UINT16_MAX) {
               timers->data[i]++;
        }
    }
    return 0;
}
