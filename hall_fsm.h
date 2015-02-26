#include "port.h"

#define STATE_HALL_DARK 0
#define STATE_HALL_KITCHEN_LIGHTS 1
#define STATE_HALL_LIGHTS 2

typedef struct {
    uint8_t msg_id_hall;
    uint8_t msg_id_kitchen;
    uint8_t msg_id_off;
    Port *port_kitchen;
    Port *port_hall;
} Hall;

uint8_t hall_fsm(uint8_t state, Hall *hall);
