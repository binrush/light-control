#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


#define STATE_LIGHT_OFF 0
#define STATE_LIGHT_ON_WAIT_RELEASE 1
#define STATE_LIGHT_ON 2
#define STATE_LIGHT_OFF_WAIT_RELEASE 3

#define VIRTUAL_TIMERS_COUNT 2
#define TIMER_BUTTON0 0
#define TIMER_ROOM 1

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

#define BUTTON0_DDR DDRC
#define BUTTON0_PIN PINC
#define BUTTON0_BIT PC5

#define CHAN_DDR DDRC
#define CHAN_PORT PORTC

#define CHAN0_BIT PC0
#define CHAN1_BIT PC1
#define CHAN2_BIT PC2
#define CHAN3_BIT PC3

#define MSG_INACTIVE 0
#define MSG_SENT 1
#define MSG_ACTIVE 2

#define MSG_BUTTON_SHORT 0
#define MSG_BUTTON_LONG 1

#define MESSAGES_COUNT 2

uint8_t button0_state = STATE_BUTTON_RELEASED;
uint8_t room_state = STATE_ROOM_DARK;

uint16_t timers[VIRTUAL_TIMERS_COUNT] = {};
uint8_t messages[MESSAGES_COUNT] = {};

ISR(TIMER0_OVF_vect) {
   uint8_t i;
   for ( i = 0; i < VIRTUAL_TIMERS_COUNT; i++ ) {
        if ( timers[i] > 0 && timers[i] < UINT16_MAX) {
               timers[i]++;
        }
    } 
}

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

void msg_send(uint8_t id) {
    if (messages[id] == MSG_INACTIVE) {
        messages[id] = MSG_SENT;
    }
}

void msg_process() {
    uint8_t i;
    for ( i = 0; i < MESSAGES_COUNT; i++ ) {
        if ( messages[i] == MSG_SENT ) {
            messages[i] = MSG_ACTIVE; 
        } else if (messages[i] == MSG_ACTIVE) {
            messages[i] = MSG_INACTIVE;
        }
    }
}

uint8_t msg_get(uint8_t id) {
    return messages[id] == MSG_ACTIVE;
}

void button_fsm() {
    switch(button0_state) {
        case STATE_BUTTON_RELEASED:
            if ( ! (BUTTON0_PIN & ( 1<< BUTTON0_BIT) ) ) {
                button0_state = STATE_BUTTON_NOISE_DELAY;
                timer_start(TIMER_BUTTON0);
            }
            break;
        case STATE_BUTTON_NOISE_DELAY:
            if (timer_get(TIMER_BUTTON0) >= BUTTON_NOISE_DELAY) {
                button0_state = STATE_BUTTON_COUNT_LENGTH;
            }
            break;
        case STATE_BUTTON_COUNT_LENGTH:
            if ( timer_get(TIMER_BUTTON0) >= BUTTON_LONG_PUSH ) {
                msg_send(MSG_BUTTON_LONG);
                timer_stop(TIMER_BUTTON0);
                button0_state = STATE_BUTTON_WAIT_RELEASE;
            } else if ( BUTTON0_PIN & ( 1<< BUTTON0_BIT )) {
                msg_send(MSG_BUTTON_SHORT);
                button0_state = STATE_BUTTON_RELEASED;
                timer_stop(TIMER_BUTTON0);
            }
            break;
        case STATE_BUTTON_WAIT_RELEASE:
            if ( BUTTON0_PIN & ( 1<< BUTTON0_BIT)) {
                button0_state = STATE_BUTTON_RELEASED;
            }
            break;
        default:
            button0_state = STATE_BUTTON_RELEASED;
            break;
    }
}

void room_fsm() {
    switch(room_state) {
        case STATE_ROOM_DARK:
            if (msg_get(MSG_BUTTON_SHORT)) {
                CHAN_PORT |= 1<<CHAN0_BIT;
                CHAN_PORT |= 1<<CHAN1_BIT;
                room_state = STATE_ROOM_LIGHT_WAIT_SWITCH;
                timer_start(TIMER_ROOM);
            }
            break;
        case STATE_ROOM_LIGHT_WAIT_SWITCH:
            if (msg_get(MSG_BUTTON_SHORT)) {
                CHAN_PORT &= ~(1<<CHAN1_BIT);
                room_state = STATE_ROOM_CHAN0_WAIT_SWITCH;
                timer_start(TIMER_ROOM);
            } else if (timer_get(TIMER_ROOM) >= SWITCH_TIMEOUT) {
                room_state = STATE_ROOM_LIGHT;
                timer_stop(TIMER_ROOM);
            }
            break;
        case STATE_ROOM_CHAN0_WAIT_SWITCH:
            if (msg_get(MSG_BUTTON_SHORT)) {
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
            if (msg_get(MSG_BUTTON_SHORT)) {
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
    

int main() {
    TCCR0 |= 1<<CS01;
    TIMSK |= 1<<TOIE0;

    //output ports
    CHAN_DDR |= 1<<CHAN0_BIT | 1<<CHAN1_BIT | 1<<CHAN2_BIT | 1<<CHAN3_BIT;
    sei();
    while(1) { 
        button_fsm();
        room_fsm();
        msg_process();
    }
    return 0;
}
