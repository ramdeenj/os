#pragma once

#include "utils.h"

#define HEAP_ORDER 19

typedef struct Header_{
    u32 used: 1,
        order: 5,
        prev: 13,
        next: 13;
} Header;

void memory_init();
void initHeader(Header* h, unsigned order);
void* kmalloc(u32 size);
void addToFreeList(Header* h);
Header* removeFromFreeList(unsigned i);
void removeThisNodeFromFreeList(Header* h);
void splitBlockOfOrder(unsigned i);
void kfree(void* v);
int kmemcmp(const void* ptr1, const void* ptr2, const unsigned numOfBytes);
void* kmemset(void* dest, const int value, const unsigned count);

// Paging Functions
struct PageTable{
    unsigned table[1024] __attribute__((aligned(4096)));
};

void setPageTable(struct PageTable* p);
struct PageTable* getPageTable();
void enablePaging();
unsigned getFaultingAddress();
void pagingInit(struct MultibootInfo* info);