#include "messages.h"

Messages *messages = NULL;

void msg_init(uint8_t size)
{
    messages = calloc(1, sizeof(Messages));
    messages->size = size;
    messages->data = calloc(1, sizeof(uint8_t)*size);
}

void msg_destroy()
{
    free(messages->data);
    free(messages);
}

int8_t msg_send(uint8_t id) {
    if ( id >= messages->size ) return -1;
    
    if (messages->data[id] == MSG_INACTIVE) {
        messages->data[id] = MSG_SENT;
    }
    return 0;
}

void msg_process() {
    uint8_t i;
    for ( i = 0; i < messages->size; i++ ) {
        if ( messages->data[i] == MSG_SENT ) {
            messages->data[i] = MSG_ACTIVE; 
        } else if (messages->data[i] == MSG_ACTIVE) {
            messages->data[i] = MSG_INACTIVE;
        }
    }
}

uint8_t msg_get(uint8_t id) {
    if ( id >= messages->size ) return 0;

    return messages->data[id] == MSG_ACTIVE;
}

