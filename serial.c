#include "serial.h"
#include "utils.h"

#define SERIAL_STATUS 0x3fd
#define SERIAL_DATA 0x3f8
#define CLEAR_TO_SEND (1<<5)

void serial_init()
{
    // Initialization code for the serial port if needed
}

void serial_putc(char ch)
{
    while (!(inb(SERIAL_STATUS) & CLEAR_TO_SEND))
    {
        // Wait until the port is ready to send data
    }
    outb(SERIAL_DATA, ch);
}

void serial_puts(const char* str)
{
    while (*str)
        serial_putc(*str++);
}
