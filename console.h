#ifndef CONSOLE_H
#define CONSOLE_H

void console_putc(char c);

#endif // CONSOLE_H

#pragma once

struct MultibootInfo;       //forward declaration
void console_init(struct MultibootInfo* info);

void scroll_console();

void console_invert_pixel(unsigned x, unsigned y);
