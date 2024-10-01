__asm__(
    ".global _start\n"
    "_start:\n"
    "mov $0x10000,%esp\n"
    "push %ebx\n"
    "call _kmain\n"
    "hlt\n"
);

#include "serial.h"
#include "console.h"
#include "kprintf.h"
#include "utils.h"
#include "interrupt.h"
#include "timer.h"

struct MultibootInfo machineInfo;

void sweet();

void kmain(struct MultibootInfo* mbi)
{
    serial_init();
    console_init(mbi);

   // Initialize interrupts
    interrupt_init();

    // Enable the timer
    timer_init(12);
    
    // Enable interrupts
    interrupt_enable();

    sweet();

    while(1);
    
}

