#include "messages.h"

uint8_t messages[MESSAGES_COUNT] = {};

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

