

#include <avr/io.h>
#include <util/delay.h>


// Low level port/pin definitions
#define sbit(x,PORT) (PORT) |= ( _BV(x) )
#define cbit(x,PORT) (PORT) &= ~( _BV(x) )

int main(void) {

    PUD

    unsigned char counter;
    sbit(DDB5,DDRB) ;      //设置PORTB5输出
    while(1) {
        sbit(PB5, PORTB); //设置PORTB5为高
        counter=0;
        while(counter!=50) {
            _delay_loop_2(3000);
            counter++;
        }

        cbit(PB5, PORTB);
        counter=0;
        while(counter!=50) {
            _delay_loop_2(3000);
            counter++;
        }
    }
    return 1;
}

// int main(void) {
//     unsigned char counter;
//     sbit(DDB5,DDRB) ;      //设置PORTB5输出
//     while(1) {
//         sbit(PORT5, PORTB); //设置PORTB5为高
//         counter=0;
//         while(counter!=50) {
//             _delay_loop_2(3000);
//             counter++;
//         }

//         cbit(PORT5, PORTB);
//         counter=0;
//         while(counter!=50) {
//             _delay_loop_2(3000);
//             counter++;
//         }
//     }
//     return 1;
// }

