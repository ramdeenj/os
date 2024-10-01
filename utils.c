#include "utils.h"

void outb(u16 port, u8 value){
    __asm__ volatile("outb %%al,%%dx" : : "a"(value), "d"(port) );
}

u8 inb(u16 port){
    u32 tmp;
    __asm__ volatile("inb %%dx,%%al" : "=a"(tmp) : "d"(port) );
    return (u8)tmp;
}

void kmemcpy( void* dest, const void* src, unsigned size)
{
    char* dp = (char*) dest;
    char* sp = (char*) src;

    while(size--)
        *dp++ = *sp++;
}

void halt() {
    __asm__ volatile("hlt");
}