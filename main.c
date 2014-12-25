#include <avr/io.h>
#include <avr/interrupt.h>


#define STATE_LIGHT_OFF 0
#define STATE_LIGHT_ON_WAIT_RELEASE 1
#define STATE_LIGHT_ON 2
#define STATE_LIGHT_OFF_WAIT_RELEASE 3

#define VIRTUAL_TIMERS_COUNT 2
#define TIMER_BUTTON0 0

#define BUTTON_NOISE_DELAY 100
#define BUTTON_LONG_PUSH 1000

#define STATE_BUTTON_RELEASED 0
#define STATE_BUTTON_NOISE_DELAY 1
#define STATE_BUTTON_COUNT_LENGTH 2
#define STATE_BUTTON_WAIT_RELEASE 3

#define STATE_ROOM_DARK 0
#define STATE_ROOM_CHAN0 1
#define STATE_ROOM_CHAN2 2
#define STATE_ROOM_LIGHT 3

#define BUTTON0_DDR DDRC
#define BUTTON0_PIN PINC
#define BUTTON0_BIT PC4

#define MSG_INACTIVE 0
#define MSG_SENT 1
#define MSG_ACTIVE 2

#define MSG_BUTTON_SHORT 0
#define MSG_BUTTON_LONG 1

#define MESSAGES_COUNT 2

uint8_t button0_state = STATE_BUTTON_RELEASED;
uint8_t room_state = STATE_ROOM_DARK;

uint16_t timers[VIRTUAL_TIMERS_COUNT];
uint8_t messages[MESSAGES_COUNT];

ISR(TIMER0_OVF_vect) {
    uint8_t i;
    for ( i = 0; i < VIRTUAL_TIMERS_COUNT; i++ ) {
        if ( timers[i] > 0 && timers[i] < UINT16_MAX) {
            timers[i]++;
        }
    } 
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
            if ( BUTTON0_PIN & ( 1<< BUTTON0_BIT) ) {
                button0_state = STATE_BUTTON_NOISE_DELAY;
                timers[TIMER_BUTTON0]++;
            }
            break;
        case STATE_BUTTON_NOISE_DELAY:
            if (timers[TIMER_BUTTON0] >= BUTTON_NOISE_DELAY) {
                button0_state = STATE_BUTTON_WAIT_RELEASE;                
            }
            break;
        case STATE_BUTTON_COUNT_LENGTH:
            if ( ! (BUTTON0_PIN & ( 1<< BUTTON0_BIT)))  {
                msg_send(MSG_BUTTON_SHORT);
                button0_state = STATE_BUTTON_RELEASED;
                break;
            }
            if ( timers[TIMER_BUTTON0] >= BUTTON_LONG_PUSH ) {
                msg_send(MSG_BUTTON_LONG);
                timers[TIMER_BUTTON0] = 0;
                button0_state = STATE_BUTTON_WAIT_RELEASE;
            }
            break;
        case STATE_BUTTON_WAIT_RELEASE:
            if ( ! (BUTTON0_PIN & ( 1<< BUTTON0_BIT))) {
                button0_state = STATE_BUTTON_RELEASED;
            }
            break;
        default:
            button0_state = STATE_BUTTON_RELEASED;
    }
}

void room_fsm() {
    switch(room_state) {
        case STATE_ROOM_DARK;
            if (msg_get(MSG_BUTTON_SHORT)) {
                
            }
            break;
        case STATE_ROOM_LIGHT;
            break;
        case STATE_ROOM_CHAN0;
            break;
        case STATE_ROOM_CHAN1;
            break;
        default:
            break;
    }
}
    

int main() {
    //timer
    TCCR0 |= 1<<CS02|1<<CS00;
    TIMSK |= 1<<TOIE0;
    
    button_fsm();
    msg_process();
    return 0;
}
