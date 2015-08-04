#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#include "apple_II.h"

typedef struct _language_card
{
    AppleII *apple;

    uint8_t high_bank[0x2000];
    uint8_t low_banks[2][0x1000];

    int last_write;

    int bank;
} LanguageCard;

uint8_t language_io_switch(void *ctx, int addr);
void language_init(LanguageCard *card, AppleII *apple, uint8_t *rom);

#endif /* __LANGUAGE_H__ */
