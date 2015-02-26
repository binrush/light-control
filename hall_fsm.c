#include "hall_fsm.h"
#include "messages.h"
#include "timers.h"


uint8_t hall_fsm(uint8_t state, Hall *hall) {
    switch ( state ) {
        case STATE_HALL_DARK:
            if ( msg_get(hall->msg_id_hall) ) {
                Port_Set(hall->port_hall);
                Port_Set(hall->port_kitchen);
                state = STATE_HALL_LIGHTS;
            } else if ( msg_get(hall->msg_id_kitchen) ) {
                Port_Set(hall->port_kitchen);
                state = STATE_HALL_KITCHEN_LIGHTS;
            }
            break;
        case STATE_HALL_KITCHEN_LIGHTS:
            if ( msg_get(hall->msg_id_hall) ) {
                Port_Set(hall->port_hall);
                state = STATE_HALL_LIGHTS;
            } else if ( msg_get(hall->msg_id_kitchen) || msg_get(hall->msg_id_off) ) {
                Port_Clear(hall->port_kitchen);
                state= STATE_HALL_DARK;
            }
            break;
        case STATE_HALL_LIGHTS:
            if ( msg_get(hall->msg_id_hall) || msg_get(hall->msg_id_off)) {
                Port_Clear(hall->port_kitchen);
                Port_Clear(hall->port_hall);
                state = STATE_HALL_DARK;
            } 
            break;
        default:
            Port_Clear(hall->port_kitchen);
            Port_Clear(hall->port_hall);
            state = STATE_HALL_DARK;
            break;
    }
    return state;
}

