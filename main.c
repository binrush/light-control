#include <avr/io.h>
#include <avr/interrupt.h>

#include "messages.h"
#include "timers.h"

#define TIMER_BUTTON0 0
#define TIMER_BUTTON1 1
#define TIMER_BUTTON2 2
#define TIMER_ROOM 3

#define MSG_BUTTON0_SHORT 0
#define MSG_BUTTON0_LONG 1
#define MSG_BUTTON1_SHORT 2
#define MSG_BUTTON1_LONG 3
#define MSG_BUTTON2_SHORT 4
#define MSG_BUTTON2_LONG 5

#define BUTTON_NOISE_DELAY 25
#define BUTTON_LONG_PUSH 500
#define SWITCH_TIMEOUT 500

#define STATE_BUTTON_RELEASED 0
#define STATE_BUTTON_NOISE_DELAY 1
#define STATE_BUTTON_COUNT_LENGTH 2
#define STATE_BUTTON_WAIT_RELEASE 3

#define STATE_ROOM_DARK 0
#define STATE_ROOM_LIGHT_WAIT_SWITCH 1
#define STATE_ROOM_CHAN0_WAIT_SWITCH 2
#define STATE_ROOM_LIGHT 3

#define STATE_KITCHEN_DARK 0
#define STATE_KITCHEN_LIGHTS_ONE 1
#define STATE_KITCHEN_LIGHTS 2

#define BUTTON0_DDR DDRC
#define BUTTON0_PIN PINC
#define BUTTON0_BIT PC4
#define BUTTON1_DDR DDRC
#define BUTTON1_PIN PINC
#define BUTTON1_BIT PC5
#define BUTTON2_DDR DDRD
#define BUTTON2_PIN PIND
#define BUTTON2_BIT PD0

#define CHAN_DDR DDRC
#define CHAN_PORT PORTC

#define CHAN0_BIT PC0
#define CHAN1_BIT PC1
#define CHAN2_BIT PC2
#define CHAN3_BIT PC3


uint8_t button0_state = STATE_BUTTON_RELEASED, button1_state = STATE_BUTTON_RELEASED, button2_state = STATE_BUTTON_RELEASED;
uint8_t room_state = STATE_ROOM_DARK, kitchen_state = STATE_KITCHEN_DARK;


ISR(TIMER0_OVF_vect) {
    timers_process();
}



uint8_t button_fsm(uint8_t state, uint8_t timer, uint8_t msg_short, uint8_t msg_long, volatile uint8_t *pin, uint8_t bit) {
    switch(state) {
        case STATE_BUTTON_RELEASED:
            if ( ! (*pin & ( 1<< bit) ) ) {
                state = STATE_BUTTON_NOISE_DELAY;
                timer_start(timer);
            }
            break;
        case STATE_BUTTON_NOISE_DELAY:
            if (timer_get(timer) >= BUTTON_NOISE_DELAY) {
                state = STATE_BUTTON_COUNT_LENGTH;
            }
            break;
        case STATE_BUTTON_COUNT_LENGTH:
            if ( timer_get(timer) >= BUTTON_LONG_PUSH ) {
                msg_send(msg_long);
                timer_stop(timer);
                state = STATE_BUTTON_WAIT_RELEASE;
            } else if ( *pin & ( 1<< bit)) {
                msg_send(msg_short);
                state = STATE_BUTTON_RELEASED;
                timer_stop(timer);
            }
            break;
        case STATE_BUTTON_WAIT_RELEASE:
            if ( *pin & ( 1<< bit)) {
                state = STATE_BUTTON_RELEASED;
            }
            break;
        default:
            state = STATE_BUTTON_RELEASED;
            break;
    }
    return state;
}

void room_fsm() {
    switch(room_state) {
        case STATE_ROOM_DARK:
            if (msg_get(MSG_BUTTON1_SHORT)) {
                CHAN_PORT |= 1<<CHAN0_BIT|1<<CHAN1_BIT;
                room_state = STATE_ROOM_LIGHT_WAIT_SWITCH;
                timer_start(TIMER_ROOM);
            }
            break;
        case STATE_ROOM_LIGHT_WAIT_SWITCH:
            if (msg_get(MSG_BUTTON1_SHORT)) {
                CHAN_PORT &= ~(1<<CHAN1_BIT);
                room_state = STATE_ROOM_CHAN0_WAIT_SWITCH;
                timer_start(TIMER_ROOM);
            } else if (timer_get(TIMER_ROOM) >= SWITCH_TIMEOUT) {
                room_state = STATE_ROOM_LIGHT;
                timer_stop(TIMER_ROOM);
            }
            break;
        case STATE_ROOM_CHAN0_WAIT_SWITCH:
            if (msg_get(MSG_BUTTON1_SHORT)) {
                CHAN_PORT &= ~(1<<CHAN0_BIT);
                CHAN_PORT |= 1<<CHAN1_BIT;
                room_state = STATE_ROOM_LIGHT;
                timer_stop(TIMER_ROOM);
                break;
            }
            if (timer_get(TIMER_ROOM) >= SWITCH_TIMEOUT) {
                room_state = STATE_ROOM_LIGHT;
                timer_stop(TIMER_ROOM);
            }
            break;
        case STATE_ROOM_LIGHT:
            if (msg_get(MSG_BUTTON1_SHORT)) {
                CHAN_PORT &= ~(1<<CHAN0_BIT|1<<CHAN1_BIT);
                room_state = STATE_ROOM_DARK;
            }
            break;
        default:
            CHAN_PORT &= ~(1<<CHAN0_BIT|1<<CHAN1_BIT);
            room_state = STATE_ROOM_DARK;
            timer_stop(TIMER_ROOM); 
            break;
    }
}

void kitchen_fsm() {
    switch ( kitchen_state ) {
        case STATE_KITCHEN_DARK:
            if ( msg_get(MSG_BUTTON0_SHORT) ) {
                CHAN_PORT |= 1<<CHAN2_BIT|1<<CHAN3_BIT;
                kitchen_state = STATE_KITCHEN_LIGHTS;
            } else if ( msg_get(MSG_BUTTON2_SHORT) ) {
                CHAN_PORT |= 1<<CHAN3_BIT;
                kitchen_state = STATE_KITCHEN_LIGHTS_ONE;
            }
            break;
        case STATE_KITCHEN_LIGHTS_ONE:
            if ( msg_get(MSG_BUTTON0_SHORT) ) {
                CHAN_PORT |= 1<<CHAN2_BIT;
                kitchen_state = STATE_KITCHEN_LIGHTS;
            } else if (msg_get(MSG_BUTTON2_SHORT) ) {
                CHAN_PORT &= ~(1<<CHAN3_BIT);
                kitchen_state= STATE_KITCHEN_DARK;
            }
            break;
        case STATE_KITCHEN_LIGHTS:
            if ( msg_get(MSG_BUTTON0_SHORT) ) {
                CHAN_PORT &= ~(1<<CHAN2_BIT|1<<CHAN3_BIT);
                kitchen_state = STATE_KITCHEN_DARK;
            }
            break;
        default:
            CHAN_PORT &= ~(1<<CHAN2_BIT|1<<CHAN3_BIT);
            kitchen_state = STATE_KITCHEN_DARK;
            break;
    }
}


int main() {
    TCCR0 |= 1<<CS01;
    TIMSK |= 1<<TOIE0;

    //output ports
    CHAN_DDR |= 1<<CHAN0_BIT | 1<<CHAN1_BIT | 1<<CHAN2_BIT | 1<<CHAN3_BIT;
    sei();
    while(1) { 
        button0_state = button_fsm(button0_state, TIMER_BUTTON0, MSG_BUTTON0_SHORT, MSG_BUTTON0_LONG, &BUTTON0_PIN, BUTTON0_BIT);
        button1_state = button_fsm(button1_state, TIMER_BUTTON1, MSG_BUTTON1_SHORT, MSG_BUTTON1_LONG, &BUTTON1_PIN, BUTTON1_BIT);
        button2_state = button_fsm(button2_state, TIMER_BUTTON2, MSG_BUTTON2_SHORT, MSG_BUTTON2_LONG, &BUTTON2_PIN, BUTTON2_BIT);
        room_fsm();
        kitchen_fsm();
        msg_process();
    }
    return 0;
}
