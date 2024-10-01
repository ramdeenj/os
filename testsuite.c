#include "kprintf.h"

void halt();
unsigned get_uptime();

unsigned div1000(unsigned numerator){
    //0.001 * 2**32 = 4294967
    unsigned long long magic = 4294967;

    //compute:  (numerator+500) * 0.001 * 2**32
    //This is the same as  numerator * 0.001 * 2**32 + 0.5 * 2**32
    unsigned long long tmp = magic * (numerator+500);

    //undo the multiply by 2**32
    return (unsigned)(tmp >> 32);
}

void sweet()
{
    unsigned nextprint=0;
    while(1){
        unsigned t = get_uptime();
        if( t >= nextprint ){
            kprintf("Uptime (seconds): %d\n", div1000(t) );
            nextprint = t + 1000;
        }
        halt();
    }
}
