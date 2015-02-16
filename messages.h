#ifndef _messages
#define _messages

#include <stdint.h>
#include <stdlib.h>


#define MSG_INACTIVE 0
#define MSG_SENT 1
#define MSG_ACTIVE 2
#define MSG_INVALID 255

typedef struct {
    uint8_t *data;
    uint8_t size;
} Messages;

void msg_init(uint8_t size);

void msg_destroy();

int8_t msg_send(uint8_t id);

void msg_process();

uint8_t msg_get(uint8_t id);

#endif
