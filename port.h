#ifndef _port_h
#define _port_h

#include <stdint.h>

typedef struct {
    volatile uint8_t *port;
    uint8_t bit;
} Port;

void Port_Set(Port *port);
void Port_Clear(Port *port);

#endif
