#include "port.h"

void Port_Set(Port *port)
{
   *(port->port) ^= 1<<port->bit;
}

void Port_Clear(Port *port)
{
    *(port->port) &= ~(1<<port->bit);
}    
