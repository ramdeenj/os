__asm__(
    ".global _start\n"
    "_start:\n"
    "mov $0x10000,%esp\n"
    "push %ebx\n"
    "call _kmain"
);

#include "kprintf.h"
#include "utils.h"
#include "serial.h"
#include "console.h"
#include "interrupt.h"
#include "timer.h"
#include "disk.h"
#include "exec.h"

struct MultibootInfo machineInfo;

// Remove or comment out the declaration of 'sweet()'
// void sweet();

void kmain2() {
    exec("HELLO.EXE", 0x400000, exec_transfer_control, 0);
    // Comment out or remove the call to 'sweet()'
    // sweet();
}

void kmain(struct MultibootInfo* mbi) {
    // Setup console data
    kmemcpy(&machineInfo, mbi, sizeof(struct MultibootInfo));

    // Setup console graphics
    console_init(&machineInfo);

    // Initialize interrupts
    interrupt_init();

    // Enable the timer
    timer_init(12);

    // Initialize memory management
    memory_init();

    // Initialize the disk system
    disk_init();

    // Enable interrupts
    interrupt_enable();

    // Read the Volume Boot Record and then call 'kmain2()'
    disk_read_metadata(kmain2);

    // Remove or comment out any additional calls to 'sweet()'
    // sweet();

    // Infinite loop to prevent the kernel from exiting
    while(1)
        halt();
}
