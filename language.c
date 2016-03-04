#include "language.h"

#include "6502.h"

#define WRITE_SWITCH 0x1
#define READ_SWITCH 0x2
#define UNUSED_SWITCH 0x4
#define BANK_SWITCH 0x8

uint8_t _write_bank(CPU_6502 *cpu, void *ctx, int address, uint8_t val)
{
    LanguageCard *card = (LanguageCard *) ctx;

    if(address >= 0xE000)
    {
        card->high_bank[address - 0xE000] = val;
    }
    else
    {
        card->low_banks[card->bank][address - 0xD000] = val;
    }

    return 0;
}

uint8_t _read_bank(CPU_6502 *cpu, void *ctx, int address)
{
    LanguageCard *card = (LanguageCard *) ctx;

    if(address >= 0xE000)
    {
        return card->high_bank[address - 0xE000];
    }
    else
    {
        return card->low_banks[card->bank][address - 0xD000];
    }

    return 0;
}

uint8_t _language_do_nothing(CPU_6502 *cpu, void *ctx, int address, uint8_t val)
{
    return 0;
}

uint8_t language_io_switch(void *ctx, int addr)
{
    int i;
    LanguageCard *card = (LanguageCard *) ctx;
    AppleII *apple = card->apple;

    int command = addr & 0xF;

    printf("Called card addr 0x%04X\n", addr);

    /* 1 = RAM, 0 = ROM */
    if(command & WRITE_SWITCH)
    {
        for(i = 0xD0; i < 0x100; i++)
        {
            cpu6502_add_write_memory_map(&apple->cpu, card, _write_bank, i);
        }
        printf("Write bank\n");
    }
    else
    {
        for(i = 0xD0; i < 0x100; i++)
        {
            cpu6502_add_write_memory_map(&apple->cpu, NULL, _language_do_nothing, i);
        }
        printf("Write ROM\n");
    }

    /* READ == WRITE ? RAM : ROM */
    if(((command & READ_SWITCH) != 0) == ((command & WRITE_SWITCH) != 0))
    {
        if(card->last_write)
        {
            for(i = 0xD0; i < 0x100; i++)
            {
                cpu6502_add_read_memory_map(&apple->cpu, card, _read_bank, i);
            }
        }
        printf("Read bank\n");
        card->last_write = 1;
    }
    else
    {
        for(i = 0xD0; i < 0x100; i++)
        {
            cpu6502_add_read_memory_map(&apple->cpu, NULL, NULL, i);
        }
        printf("Read ROM\n");
        card->last_write = 0;
    }

    if(command & BANK_SWITCH)
    {
        card->bank = 0;
        printf("bank 0\n");
    }
    else
    {
        card->bank = 1;
        printf("bank 1\n");
    }

    return 0;
}

void language_init(LanguageCard *card, AppleII *apple)
{
    card->apple = apple;
}
