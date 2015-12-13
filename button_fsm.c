#include "button_fsm.h"
#include "timers.h"

int8_t Button_Read(Button *button) {
    return *(button->pin) & (1<<button->bit);
}

uint8_t button_fsm_init() {
    return STATE_BUTTON_RELEASED;
}

uint8_t button_fsm(uint8_t state, Button *button) {
    switch(state) {
        case STATE_BUTTON_RELEASED:
            if ( ! Button_Read(button) ) {
                state = STATE_BUTTON_NOISE_DELAY;
                timer_start(button->timer_id);
            }
            break;
        case STATE_BUTTON_NOISE_DELAY:
            if (timer_get(button->timer_id) >= BUTTON_NOISE_DELAY) {
                if ( ! Button_Read(button) ) {
                    state = STATE_BUTTON_COUNT_LENGTH;
                } else {
                    state = STATE_BUTTON_RELEASED;
                }
            }
            break;
        case STATE_BUTTON_COUNT_LENGTH:
            if ( timer_get(button->timer_id) >= BUTTON_LONG_PUSH ) {
                msg_send(button->msg_id_long);
                timer_stop(button->timer_id);
                state = STATE_BUTTON_WAIT_RELEASE;
            } else if ( Button_Read(button) ) {
                msg_send(button->msg_id_short);
                state = STATE_BUTTON_RELEASED;
                timer_stop(button->timer_id);
            }
            break;
        case STATE_BUTTON_WAIT_RELEASE:
            if ( Button_Read(button) ) {
                state = STATE_BUTTON_RELEASED;
            }
            break;
        default:
            state = STATE_BUTTON_RELEASED;
            break;
    }
    return state;
}
