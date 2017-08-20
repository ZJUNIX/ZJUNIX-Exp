#ifndef _PAGE__H
#define _PAGE__H

void init_pgtable();

typedef struct {
    unsigned int reserved1 : 12;
    unsigned int Mask : 16;
    unsigned int reserved0 : 4;
} __PageMask;

typedef struct {
    unsigned int ASID : 8;
    unsigned int reserved : 5;
    unsigned int VPN2 : 19;
} __EntryHi;

typedef struct {
    unsigned int G : 1;
    unsigned int V : 1;
    unsigned int D : 1;
    unsigned int C : 3;
    unsigned int PFN : 24;
    unsigned int reserved : 2;
} __EntryLo;

typedef struct {
    __EntryLo EntryLo0;
    __EntryLo EntryLo1;
    __EntryHi EntryHi;
    __PageMask PageMask;
} PageTableEntry;

#endif
