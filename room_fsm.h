#include "port.h"

#define STATE_ROOM_DARK 0
#define STATE_ROOM_LIGHT_WAIT_SWITCH 1
#define STATE_ROOM_CHAN0_WAIT_SWITCH 2
#define STATE_ROOM_LIGHT 3

#define SWITCH_TIMEOUT 500

typedef struct {
    uint8_t timer_id;
    uint8_t msg_id_switch;
    uint8_t msg_id_off;
    Port *port0;
    Port *port1;
} Room;

uint8_t room_fsm(uint8_t state, Room *room);
