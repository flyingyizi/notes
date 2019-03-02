

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    unsigned char counter;
    DDRB=0xff;      //设置PORTB输出
    while(1) {
        PORTB=0xff; //设置PORTB为高
        counter=0;
        while(counter!=50) {
            _delay_loop_2(30000);
            counter++;
        }
        PORTB=0x00;
        counter=0;
        while(counter!=50) {
            _delay_loop_2(30000);
            counter++;
        }
    }
    return 1;
}