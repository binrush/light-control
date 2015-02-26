#include "room_fsm.h"
#include "messages.h"
#include "timers.h"

uint8_t room_fsm(uint8_t state, Room *room) {
    switch( state ) {
        case STATE_ROOM_DARK:
            if ( msg_get(room->msg_id_switch )) {
                Port_Set(room->port0);
                Port_Set(room->port1);
                state = STATE_ROOM_LIGHT_WAIT_SWITCH;
                timer_start(room->timer_id);
            }
            break;
        case STATE_ROOM_LIGHT_WAIT_SWITCH:
            if ( msg_get(room->msg_id_switch) ) {
                Port_Clear(room->port1);
                state = STATE_ROOM_CHAN0_WAIT_SWITCH;
                timer_start(room->timer_id);
            } else if ( timer_get(room->timer_id) >= SWITCH_TIMEOUT ) {
                state = STATE_ROOM_LIGHT;
                timer_stop(room->timer_id);
            }
            break;
        case STATE_ROOM_CHAN0_WAIT_SWITCH:
            if ( msg_get(room->msg_id_switch) ) {
                Port_Clear(room->port0);
                Port_Set(room->port1);
                state = STATE_ROOM_LIGHT;
                timer_stop(room->timer_id);
                break;
            } else if ( timer_get(room->timer_id) >= SWITCH_TIMEOUT ) {
                state = STATE_ROOM_LIGHT;
                timer_stop(room->timer_id);
            }
            break;
        case STATE_ROOM_LIGHT:
            if ( msg_get( room->msg_id_switch ) || msg_get(room->msg_id_off) ) {
                Port_Clear(room->port0);
                Port_Clear(room->port1);
                state = STATE_ROOM_DARK;
            }
            break;
        default:
            Port_Clear(room->port0);
            Port_Clear(room->port1);
            state = STATE_ROOM_DARK;
            timer_stop(room->timer_id); 
            break;
    }
    return state;
}

