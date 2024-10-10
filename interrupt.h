#pragma once
#pragma pack(push, 1)

#include "utils.h"
#include "timer.h"

#define SEGMENT_PRESENT 0x80
#define SEGMENT_RING_0 0x00
#define SEGMENT_RING_3 0x60
#define SEGMENT_IS_CODE_OR_DATA 0x10
#define SEGMENT_TYPE_DATA 2
#define SEGMENT_TYPE_CODE 10
#define SEGMENT_32_BIT 0x40
#define SEGMENT_LIMIT_32_BIT 0x80
#define SEGMENT_LIMIT_HIGH_NIBBLE 0xf

struct GDTEntry {
    u16 limitBits0to15;
    u16 baseBits0to15;
    u8 baseBits16to23;
    u8 flags;
    u8 flagsAndLimitBits16to19;
    u8 baseBits24to31;
};

// Global variable
static struct GDTEntry gdt[3];

struct LGDT{
    u16 size;
    struct GDTEntry* addr;
};

struct InterruptContext{
    // We pushed these in midlevel handler
    u32 eax, ebx, ecx, edx, esi, edi, ebp;
    // We pushed this in lowlevel handler
    u32 interruptNumber;
    // CPU pushed these (except maybe errorcode)
    u32 errorcode, eip, cs, eflags;
    // CPU pushed these if there was a ring transition
    u32 esp, ss;
};

struct IDTEntry{
    u16 addrLow;     // Address of lowlevel handler
    u16 selector;    // GDT entry
    u8 zero;         // Must be zero
    u8 flags;        // 0x8e for interrupt handler
    u16 addrHigh;    // Address of lowlevel handler
};

struct LIDT{
    u16 size;               //table size, bytes
    struct IDTEntry* addr;  //table start
};

#define NUM_INTERRUPTS 49
static struct IDTEntry idt[NUM_INTERRUPTS];
extern void* lowlevel_addresses[];

void interrupt_init();
void gdt_init();
void idt_inti();
void highlevel_handler(struct InterruptContext* ctx);

#define MAX_HANDLERS 4

typedef void (*InterruptHandler)(struct InterruptContext* ctx);

// Handler for each interrupt/exception from 0...49; up to 4 handlers per interrupt
static InterruptHandler handlers[NUM_INTERRUPTS][MAX_HANDLERS];

void panic(const char *);
void register_interrupt_handler(unsigned interrupt, InterruptHandler func);
void panic2(void* eip, const char* msg);

void divideByZero(struct InterruptContext* ctx);
void illegalOpcode(struct InterruptContext* ctx);
void generalFault(struct InterruptContext* ctx);

void ackPic1(struct InterruptContext* ctx);
void ackPic2(struct InterruptContext* ctx);
void interrupt_enable();

void timer0Handler(struct InterruptContext* ctx);
void timerHandler(struct InterruptContext* ctx);

#pragma pack(pop)