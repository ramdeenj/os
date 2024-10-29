#pragma once

#include "utils.h"
#include "interrupt.h"
#include "errno.h"
#include "pci.h"
#include "memory.h"

void disk_init();

#pragma pack(push,1)
struct PhysicalRegionDescriptor{
    void* address;
    u16 byteCount;
    u16 flags;       //0x8000=last one, 0x0000=not last
};
#pragma pack(pop)

static struct PhysicalRegionDescriptor* physicalRegionDescriptor;

typedef void (*disk_callback_t)(int, void*, void*);

void disk_interrupt(struct InterruptContext* ctx);
void disk_read_sectors(
                    unsigned firstSector,
                    unsigned numSectors,
                    disk_callback_t callback,
                    void* callback_data);

// Filesystem items
#pragma pack(push,1)

struct GUID {
    u32 time_low;
    u16 time_mid;
    u16 time_high_version;
    u16 clock;
    u8 node[6];
};

struct GPTEntry {
    struct GUID type;
    struct GUID guid;
    u32 firstSector;
    u32 firstSector64;
    u32 lastSector;
    u32 lastSector64;
    u32 attributes;
    u32 attributes64;
    u16 name[36];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct VBR{
    char jmp[3];
    char oem[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 reserved_sectors;
    u8 num_fats;
    u16 UNUSED_num_root_dir_entries;
    u16 UNUSED_num_sectors_small;
    u8 id ;
    u16 UNUSED_sectors_per_fat_12_16;
    u16 sectors_per_track;
    u16 num_heads;
    u32 first_sector;
    u32 num_sectors;
    u32 sectors_per_fat;
    u16 flags;
    u16 version;
    u32 root_cluster;
    u16 fsinfo_sector;
    u16 backup_boot_sector;
    char reservedField[12];
    u8 drive_number;
    u8 flags2;
    u8 signature;
    u32 serial_number;
    char label[11];
    char identifier[8];
    char code[420];
    u16 checksum;
};
#pragma pack(pop)

typedef void (*disk_metadata_callback_t)(void);
void disk_read_metadata( disk_metadata_callback_t kmain_callback );
u32 clusterNumberToSectorNumber( u32 clnum );
void readRoot();

// Root directory items
#pragma pack(push,1)
struct DirEntry {
    char base[8];
    char ext[3];
    u8 attributes;
    u8 reserved;
    u8 creationTimeCentiseconds;
    u16 creationTime;
    u16 creationDate;
    u16 lastAccessDate;
    u16 clusterHigh;
    u16 lastModifiedTime;
    u16 lastModifiedDate;
    u16 clusterLow;
    u32 size;
};
#pragma pack(pop)

#pragma pack(push,1)
struct LFNEntry {
    unsigned char sequenceNumber;
    char name0[10];             //5 characters
    char attribute;             //always 15
    char zero;                  //always zero
    char checksum;
    char name1[12];             //6 characters
    unsigned short alsozero;    //always zero
    char name2[4];              //2 characters
};
#pragma pack(pop)

void parseFilename(struct DirEntry* entry, char* buffer);