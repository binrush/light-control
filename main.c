#include <avr/io.h>
#include <avr/interrupt.h>

#include "button_fsm.h"
#include "room_fsm.h"
#include "hall_fsm.h"

#define MSG_BUTTON_ROOM 0
#define MSG_BUTTON_HALL 1
#define MSG_BUTTON_KITCHEN 2
#define MSG_BUTTON_LONG 3

uint8_t button_room_state, button_hall_state, button_kitchen_state;
uint8_t room_state = STATE_ROOM_DARK, hall_state = STATE_HALL_DARK;

Button button_room = {
    .timer_id     = 0,
    .msg_id_short = MSG_BUTTON_ROOM,
    .msg_id_long  = MSG_BUTTON_LONG,
    .pin          = &DDRC,
    .bit          = PC4   
};

Button button_hall = {
    .timer_id     = 1,
    .msg_id_short = MSG_BUTTON_HALL,
    .msg_id_long  = MSG_BUTTON_LONG,
    .pin          = &DDRC,
    .bit          = PC5
};

Button button_kitchen = {
    .timer_id     = 2,
    .msg_id_short = MSG_BUTTON_KITCHEN,
    .msg_id_long  = MSG_BUTTON_LONG,
    .pin          = &DDRD,
    .bit          = PD0
};

Port port_room0   = { .port = &PORTC, .bit = PC0 };
Port port_room1   = { .port = &PORTC, .bit = PC1 };
Port port_hall    = { .port = &PORTC, .bit = PC2 };
Port port_kitchen = { .port = &PORTC, .bit = PC3 };

Room room = {
    .timer_id      = 3,
    .msg_id_switch = MSG_BUTTON_ROOM,
    .msg_id_off    = MSG_BUTTON_LONG,
    .port0         = &port_room0,
    .port1         = &port_room1
};

Hall hall = {
    .msg_id_hall    = MSG_BUTTON_HALL,
    .msg_id_kitchen = MSG_BUTTON_KITCHEN,
    .msg_id_off     = MSG_BUTTON_LONG,
    .port_hall      = &port_hall,
    .port_kitchen   = &port_kitchen
};

ISR(TIMER0_OVF_vect) {
    timers_process();
}

int main() {
    TCCR0 |= 1<<CS01;
    TIMSK |= 1<<TOIE0;

    //output ports
    DDRC |= 1<<PC0 | 1<<PC1 | 1<<PC2 | 1<<PC3;
    msg_init(4);
    button_room_state = button_hall_state = button_kitchen_state = button_fsm_init();
    sei();
    while(1) { 
        button_room_state = button_fsm(button_room_state, &button_room);
        button_hall_state = button_fsm(button_hall_state, &button_hall);
        button_kitchen_state = button_fsm(button_kitchen_state, &button_kitchen);

        room_state = room_fsm(room_state, &room);
        hall_state = hall_fsm(hall_state, &hall);

        msg_process();
    }
    return 0;
}
