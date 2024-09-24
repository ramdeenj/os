__asm__(
    ".global _start\n"
    "_start:\n"
    "mov $0x10000,%esp\n"
    "push %ebx\n"
    "call _kmain"
);

#include "serial.h"
#include "console.h"
#include "kprintf.h"
#include "utils.h"

struct MultibootInfo machineInfo;

void sweet_scroll();

void kmain(struct MultibootInfo* mbi)
{
    serial_init();
    console_init(mbi);

    // Call the scrolling function for the assignment
    sweet_scroll();

    // Output "\nDONE\n" to the serial port
    serial_puts("\nDONE\n");

    while(1);
}
