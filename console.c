#include "console.h"
#include "serial.h"
#include "utils.h"
#include "etec3701_10x20.h"
#include "kprintf.h"

static volatile u8* framebuffer;
static unsigned pitch;
static unsigned width;
static unsigned height;
static u16 foregroundColor = 0xffff; // Grey text
static u16 backgroundColor = 0x001F; // Royal blue background

static unsigned currentRow = 0;    
static unsigned currentColumn = 0;

#define CHAR_WIDTH  10
#define CHAR_HEIGHT 20
#define MAX_COLUMNS (width / CHAR_WIDTH)
#define MAX_ROWS    (height / CHAR_HEIGHT)

// Function to clear the entire screen
static void clear_screen()
{
    volatile u8* f8 = framebuffer;
    for(unsigned y = 0; y < height; y++)
    {
        volatile u16* f16 = (volatile u16*) f8;
        for(unsigned x = 0; x < width; x++)
        {
            f16[x] = backgroundColor;
        }
        f8 += pitch;
    }
}

// Function to set a pixel on the screen
void set_pixel(unsigned x, unsigned y, u16 color)
{
    volatile u16* p = (volatile u16*) (framebuffer + y * pitch + x * 2);
    *p = color;
}

// Function to draw a character at a specified row and column
void draw_character_at_cell(unsigned char ch, unsigned row, unsigned column)
{
    unsigned x = column * CHAR_WIDTH;
    unsigned y = row * CHAR_HEIGHT;

    for (int r = 0; r < CHAR_HEIGHT; ++r)
    {
        unsigned row_data = font_data[ch][r];
        for (int c = 0; c < CHAR_WIDTH; ++c)
        {
            if (row_data & (1 << (CHAR_WIDTH - 1 - c))) 
                set_pixel(x + c, y + r, foregroundColor);
            else
                set_pixel(x + c, y + r, backgroundColor);
        }
    }
}

// Scroll screen when cursor reaches the bottom row
void scroll_screen()
{
    // Move rows up by one
    for (unsigned row = 1; row < MAX_ROWS; ++row)
    {
        volatile u8* source = framebuffer + row * CHAR_HEIGHT * pitch;
        volatile u8* dest = framebuffer + (row - 1) * CHAR_HEIGHT * pitch;
        kmemcpy((void*)dest, (void*)source, CHAR_HEIGHT * pitch);
    }

    // Clear the last row
    volatile u8* last_row = framebuffer + (MAX_ROWS - 1) * CHAR_HEIGHT * pitch;
    for (unsigned y = 0; y < CHAR_HEIGHT; ++y)
    {
        volatile u16* f16 = (volatile u16*)(last_row + y * pitch);
        for (unsigned x = 0; x < width; x++)
        {
            f16[x] = backgroundColor; // Royal blue background
        }
    }

    currentRow = MAX_ROWS - 1;
}

// Function to output a single character
void console_putc(char c)
{
    serial_putc(c);

    if (c == '\n')
    {
        currentColumn = 0;
        currentRow++;
        if (currentRow >= MAX_ROWS)
        {
            scroll_screen(); // Scroll if the row exceeds the screen height
        }
    }
    else if (c == '\r')
    {
        currentColumn = 0;
    }
    else if (c == '\t')
    {
        currentColumn = (currentColumn + 8) & ~(8 - 1);
        if (currentColumn >= MAX_COLUMNS)
        {
            currentColumn = 0;
            currentRow++;
            if (currentRow >= MAX_ROWS)
            {
                scroll_screen();
            }
        }
    }
    else if (c == '\f')
    {
        clear_screen();
        currentRow = 0;
        currentColumn = 0;
    }
    else if (c == '\x7f') // Handle backspace
    {
        if (currentColumn > 0)
        {
            currentColumn--;
        }
        else if (currentRow > 0)
        {
            currentRow--;
            currentColumn = MAX_COLUMNS - 1;
        }
        draw_character_at_cell(' ', currentRow, currentColumn);
    }
    else
    {
        draw_character_at_cell(c, currentRow, currentColumn);
        currentColumn++;

        if (currentColumn >= MAX_COLUMNS)
        {
            currentColumn = 0;
            currentRow++;
            if (currentRow >= MAX_ROWS)
            {
                scroll_screen();
            }
        }
    }
}

// Fill the screen with white for testing framebuffer functionality
static void fill_screen_white()
{
    volatile u8* f8 = framebuffer;
    for(unsigned y = 0; y < height; y++)
    {
        volatile u16* f16 = (volatile u16*) f8;
        for(unsigned x = 0; x < width; x++)
        {
            f16[x] = 0xFFFF; // White color
        }
        f8 += pitch;
    }
}

// Console initialization with debug output
void console_init(struct MultibootInfo* mbi)
{
    framebuffer = (volatile u8*) (mbi->fb.addr);
    
    if (framebuffer == 0) {  
        serial_puts("Framebuffer is NULL\n");
        return;
    }

    pitch = mbi->fb.pitch;
    width = mbi->fb.width;
    height = mbi->fb.height;

    // Print framebuffer info to serial for debugging
    kprintf("Framebuffer address: 0x%x\n", (unsigned int)framebuffer);
    kprintf("Pitch: %u\n", pitch);
    kprintf("Width: %u\n", width);
    kprintf("Height: %u\n", height);

    if (pitch == 0 || width == 0 || height == 0) {
        serial_puts("Invalid framebuffer dimensions\n");
        return;
    }

    fill_screen_white(); // Fill the screen with white to test

    serial_puts("Screen filled with white.\n");
}
