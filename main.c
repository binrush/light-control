#include <avr/io.h>

#define STATE_LIGHT_OFF 0
#define STATE_LIGHT_ON_WAIT_RELEASE 1
#define STATE_LIGHT_ON 2
#define STATE_LIGHT_OFF_WAIT_RELEASE 3


uint8_t state = STATE_LIGHT_OFF;

int main() {
    DDRC = 1<<PC0|1<<PC1|1<<PC2|1<<PC3;
    DDRC = 0;
    PORTC = 0;

    state = STATE_LIGHT_OFF;
    while(1) {
        switch (state)  {
            case STATE_LIGHT_OFF:
                if ( PINC & ( 1<<PC4 ) ) {
                    state = STATE_LIGHT_ON_WAIT_RELEASE;
                    PORTC |= 1<<PC0; 
                }
                break;
            case STATE_LIGHT_ON_WAIT_RELEASE:
                if ( !( PINC & ( 1<<PC4 ) ) ) {
                    state = STATE_LIGHT_ON;
                }
                break;
            case STATE_LIGHT_ON:
                if ( PINC & ( 1<<PC4 ) ) {
                    state = STATE_LIGHT_OFF_WAIT_RELEASE;
                    PORTC &= ~(1<<PC0); 
                }
                break;
            case STATE_LIGHT_OFF_WAIT_RELEASE:
                if ( !( PINC & ( 1<<PC4 ) ) ) {
                    state = STATE_LIGHT_OFF;
                }
                break;
            default:
                break;
        }
    }
    return 0;
}
