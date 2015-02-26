#include <stdint.h>
#include "timers.h"
#include "messages.h"

#define STATE_BUTTON_RELEASED 0
#define STATE_BUTTON_NOISE_DELAY 1
#define STATE_BUTTON_COUNT_LENGTH 2
#define STATE_BUTTON_WAIT_RELEASE 3

#define BUTTON_NOISE_DELAY 25
#define BUTTON_LONG_PUSH 500

typedef struct {
    uint8_t timer_id;
    uint8_t msg_id_short;
    uint8_t msg_id_long;
    volatile uint8_t *pin;
    uint8_t bit;
} Button;

int8_t Button_Read(Button *button);

uint8_t button_fsm_init();
uint8_t button_fsm(uint8_t state, Button *button);
