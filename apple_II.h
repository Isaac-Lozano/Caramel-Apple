#ifndef __APPLE_II_H__
#define __APPLE_II_H__

#include <inttypes.h>

#include "6502.h"
#include "applescr.h"

#define APPLE_II_TEXT_BUFFER 0x400
#define APPLE_II_HI_RES_BUFFER 0x2000

#define APPLE_II_ROM 0xD000
#define APPLE_II_ROM_SIZE 0x3000

#define APPLE_II_NUM_CARDS 8

typedef struct _apple_II AppleII;

typedef uint8_t (*AppleII_RefFunc)(void *ctx, int address);

typedef struct _peripheral_card
{
    void *ctx;

    AppleII_RefFunc callback;
} PeripheralCard;

typedef struct _apple_II
{
    CPU_6502 cpu;
    AppleScr scr;

    PeripheralCard cards[APPLE_II_NUM_CARDS];
} AppleII;

int apple_II_init(AppleII *apple, uint8_t *rom);
int apple_II_run(AppleII *apple);
int apple_II_add_card(AppleII *apple, void *ctx, int slot, AppleII_RefFunc control, cpu6502_read_func rom);

#endif /* __APPLE_II_H__ */
