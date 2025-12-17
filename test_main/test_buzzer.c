#include "buzzer.h"
#include <util/delay.h>

int main(void) {
    init_timer0();

    while (1) {
        play_twinkle_twinkle();
        _delay_ms(1000); // Pause for 1 second before repeating
    }
}
