#include "apple_II.h"

/* XXX FOR HACKS */
#include "diskII.h"

#include <SDL2/SDL.h>
#include <stdio.h>

int debug = 0;

uint8_t _io_read_callback(CPU_6502 *cpu, void *ctx, int addr)
{
    AppleII *apple = (AppleII *) ctx;

    int semi_page = addr >> 4;
    if(semi_page >= 0xC08)
    {
        /* Memory map for card io switches */
        if(apple->cards[semi_page - 0xC08].callback) 
        {
            return apple->cards[semi_page - 0xC08].callback(apple->cards[semi_page - 0xC08].ctx, addr);
        }
    }
    else
    {
        /* System io switches */
        switch(addr)
        {
            case 0xC000: /* Read keyboard */
                break;
            case 0xC010: /* Clear keyboard */
                cpu->memory[0xC000] &= 0x7F;
                break;
            case 0xC011:
                printf("Co11\n");
                getchar();
                break;
            case 0xC012:
                printf("Co12\n");
                getchar();
                break;
            case 0xC050:
                apple->scr.display_flags |= APPLE_SCR_MODE_GRAPHICS;
                printf("Graphics\n");
                printf("0x%X\n", apple->scr.display_flags);
                break;
            case 0xC051:
                apple->scr.display_flags &= ~APPLE_SCR_MODE_GRAPHICS;
                printf("Text\n");
                break;
            case 0xC052:
                apple->scr.display_flags &= ~APPLE_SCR_MODE_MIXED;
                printf("Pure\n");
                break;
            case 0xC053:
                apple->scr.display_flags |= APPLE_SCR_MODE_MIXED;
                printf("Mixed\n");
                break;
            case 0xC054:
                apple->scr.display_flags &= ~APPLE_SCR_MODE_SECONDARY;
                printf("Primary\n");
                break;
            case 0xC055:
                apple->scr.display_flags |= APPLE_SCR_MODE_SECONDARY;
                printf("Secondary\n");
                break;
            case 0xC056:
                apple->scr.display_flags &= ~APPLE_SCR_MODE_HIGH_RES;
                printf("Lo-res\n");
                break;
            case 0xC057:
                apple->scr.display_flags |= APPLE_SCR_MODE_HIGH_RES;
                printf("Hi-res\n");
                break;
            default:
                printf("Unknown switch 0x%04X\n", addr);
        }
    }

    return cpu->memory[addr];
}

uint8_t _io_write_callback(CPU_6502 *cpu, void *ctx, int addr, uint8_t val)
{
    AppleII *apple = (AppleII *) ctx;

    int semi_page = addr >> 4;
    if(semi_page >= 0xC08)
    {
        /* Memory map for card io switches */
        if(apple->cards[semi_page - 0xC08].callback) 
        {
            return apple->cards[semi_page - 0xC08].callback(apple->cards[semi_page - 0xC08].ctx, addr);
        }
    }

    if(addr == 0xc010)
    {
        cpu->memory[0xC000] &= 0x7F;
    }

    return 0;
}

/* Used for read-only data */
uint8_t _do_nothing(CPU_6502 *cpu, void *unused, int addr, uint8_t val)
{
    return 0;
}

/* Expects a 12K rom to put in high memory */
int apple_II_init(AppleII *apple, uint8_t *rom)
{
    int i;

    /* XXX Eh */
    SDL_Init(0);

    /* We're going to load the ROM in directly, since it isn't a whole 64K */
    cpu6502_init(&apple->cpu, NULL);

    /* Set the io memory maps */
    /* TODO: make ROM read-only */
    cpu6502_add_read_memory_map(&apple->cpu, apple, _io_read_callback, 0xC0);
    cpu6502_add_write_memory_map(&apple->cpu, apple, _io_write_callback, 0xC0);

    for(i = 0xD0; i < 0x100; i++)
    {
        cpu6502_add_write_memory_map(&apple->cpu, NULL, _do_nothing, i);
    }

    /* Loads in ROM directly */
    memcpy(apple->cpu.memory + APPLE_II_ROM, rom, APPLE_II_ROM_SIZE);

    /* Make a new screen */
    if( apple_scr_init(&apple->scr,
            apple->cpu.memory + APPLE_II_TEXT_BUFFER,
            apple->cpu.memory + APPLE_II_HI_RES_BUFFER) )
    {
        printf("Error: Could not init screen\n");
        return 1;
    }

    /* Clear out the peripheral cards */
    memset(&apple->cards, 0, sizeof(PeripheralCard) * APPLE_II_NUM_CARDS);

    return 0;
}

int apple_II_run(AppleII *apple)
{
    cpu6502_reset(&apple->cpu);
    /* TODO: Manage the screen re-rendering better */

    int time = SDL_GetTicks();

    for(;;)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                            apple->cpu.memory[0xC000] = '\r' | 0x80;
                            break;

                        case SDLK_BACKSPACE:
                        case SDLK_LEFT:
                            apple->cpu.memory[0xC000] = 0x08 | 0x80;
                            break;

                        case SDLK_RIGHT:
                            apple->cpu.memory[0xC000] = 0x15 | 0x80;
                            break;

                        case SDLK_F2:
                            printf("Resetting CPU\n");
                            cpu6502_reset(&apple->cpu);
                            break;

                        case SDLK_F3:
                            debug = 1;
                            break;

                        /* XXX Hacky code to allow on-the-fly img loading */
                        case SDLK_F4:
                            {
                                char filebuf[256];
                                fgets(filebuf, 256, stdin);
                                filebuf[strlen(filebuf)-1] = '\0';
                                printf("\"%s\"\n", filebuf);
                                FILE *img = fopen(filebuf, "r");
                                AppleDiskII *disk = (AppleDiskII *) apple->cards[0x06].ctx;
                                disk->drives[0].image = img;
                            }
                    }
                    break;

                case SDL_TEXTINPUT:
                    apple->cpu.memory[0xC000] = event.text.text[0] | 0x80;
                    break;

                case SDL_QUIT:
                    return 0;
            }
        }

        int cycles = cpu6502_run(&apple->cpu, 5000);

        apple_scr_redraw(&apple->scr);

        /* TIMING THINGS */
        int ntime = SDL_GetTicks();
        int delta = ntime - time;
        time = ntime;

        if(delta < 5)
        {
            SDL_Delay(5 - delta);
        }
    }
}

/* The function pointers may be NULL, as well as the context */
/* Slot should be [0,7] */
int apple_II_add_card(AppleII *apple, void *ctx, int slot, AppleII_RefFunc control, cpu6502_read_func rom)
{
    /* No rom mapping for slot 0 */
    if(slot != 0)
    {
        cpu6502_add_read_memory_map(&apple->cpu, ctx, rom, 0xC0 + slot);
    }

    /* Insert the card */
    apple->cards[slot].callback = control;
    apple->cards[slot].ctx = ctx;

    return 0;
}
