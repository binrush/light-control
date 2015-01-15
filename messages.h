#include <inttypes.h>

#ifndef _messages

#define _messages

#define MSG_INACTIVE 0
#define MSG_SENT 1
#define MSG_ACTIVE 2

#define MSG_BUTTON_SHORT 0
#define MSG_BUTTON_LONG 1

#define MESSAGES_COUNT 2


void msg_send(uint8_t id);

void msg_process();

uint8_t msg_get(uint8_t id);

#endif
