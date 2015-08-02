#include <inttypes.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "6502.h"
#include "applescr.h"
#include "diskII.h"

#define APPLE_ROM_OFFSET 0xD000
#define APPLE_ROM_SIZE 0x3000
#define APPLE_TEXT_BUFFER_OFFSET 0x400

AppleDiskII disk;

char Disk2_rom[] =
                "\xA2\x20\xA0\x00\xA2\x03\x86\x3C\x8A\x0A\x24\x3C\xF0\x10\x05\x3C"
                "\x49\xFF\x29\x7E\xB0\x08\x4A\xD0\xFB\x98\x9D\x56\x03\xC8\xE8\x10"
                "\xE5\x20\x58\xFF\xBA\xBD\x00\x01\x0A\x0A\x0A\x0A\x85\x2B\xAA\xBD"
                "\x8E\xC0\xBD\x8C\xC0\xBD\x8A\xC0\xBD\x89\xC0\xA0\x50\xBD\x80\xC0"
                "\x98\x29\x03\x0A\x05\x2B\xAA\xBD\x81\xC0\xA9\x56\x20\xA8\xFC\x88"
                "\x10\xEB\x85\x26\x85\x3D\x85\x41\xA9\x08\x85\x27\x18\x08\xBD\x8C"
                "\xC0\x10\xFB\x49\xD5\xD0\xF7\xBD\x8C\xC0\x10\xFB\xC9\xAA\xD0\xF3"
                "\xEA\xBD\x8C\xC0\x10\xFB\xC9\x96\xF0\x09\x28\x90\xDF\x49\xAD\xF0"
                "\x25\xD0\xD9\xA0\x03\x85\x40\xBD\x8C\xC0\x10\xFB\x2A\x85\x3C\xBD"
                "\x8C\xC0\x10\xFB\x25\x3C\x88\xD0\xEC\x28\xC5\x3D\xD0\xBE\xA5\x40"
                "\xC5\x41\xD0\xB8\xB0\xB7\xA0\x56\x84\x3C\xBC\x8C\xC0\x10\xFB\x59"
                "\xD6\x02\xA4\x3C\x88\x99\x00\x03\xD0\xEE\x84\x3C\xBC\x8C\xC0\x10"
                "\xFB\x59\xD6\x02\xA4\x3C\x91\x26\xC8\xD0\xEF\xBC\x8C\xC0\x10\xFB"
                "\x59\xD6\x02\xD0\x87\xA0\x00\xA2\x56\xCA\x30\xFB\xB1\x26\x5E\x00"
                "\x03\x2A\x5E\x00\x03\x2A\x91\x26\xC8\xD0\xEE\xE6\x27\xE6\x3D\xA5"
                "\x3D\xCD\x00\x08\xA6\x2B\x90\xDB\x4C\x01\x08\x00\x00\x00\x00\x00"
                ;

AppleScr apple_win;

void init_disk(void)
{
    memset(&disk, 0, sizeof(disk));
    disk.drives[0].index = disk.drives[0].sect_buf;
    disk.drives[1].index = disk.drives[1].sect_buf;

    FILE *image;
    image = fopen("Master.dsk", "r");
    disk.drives[0].image = image;
}

uint8_t io_write_thing(CPU_6502 *cpu, void *unused, int address, uint8_t val)
{
    if(address == 0xC010)
    {
        cpu->memory[0xC000] &= 0x7F;
    }

    return 0;
}

uint8_t test_read_callback(CPU_6502 *cpu, void *unused, int address)
{
    if((address >> 4) == 0xC0E)
    {
//        printf("0X%04X\n", cpu->pc);
        return diskII_reference(&disk, address, 0);
    }
    printf("0X%04X\n", address);

    switch(address)
    {
        case 0xC000:
            break;
        case 0xC010:
            /* Clear Keyboard Stroke */
            cpu->memory[0xC000] &= 0x7F;
            break;
        case 0xc050:
            apple_win.display_flags |= APPLE_SCR_MODE_GRAPHICS;
            printf("Text\n");
            break;
        case 0xc051:
            apple_win.display_flags &= ~APPLE_SCR_MODE_GRAPHICS;
            printf("Graphics\n");
            break;
        case 0xc052:
            apple_win.display_flags &= ~APPLE_SCR_MODE_MIXED;
            printf("Pure\n");
            break;
        case 0xc053:
            apple_win.display_flags |= APPLE_SCR_MODE_MIXED;
            printf("Mixed\n");
            break;
        case 0xc054:
            apple_win.display_flags &= ~APPLE_SCR_MODE_SECONDARY;
            printf("Primary\n");
            break;
        case 0xc055:
            apple_win.display_flags |= APPLE_SCR_MODE_SECONDARY;
            printf("Secondary\n");
            break;
        case 0xc056:
            apple_win.display_flags &= ~APPLE_SCR_MODE_HIGH_RES;
            printf("Lo-res\n");
            break;
        case 0xc057:
            apple_win.display_flags |= APPLE_SCR_MODE_HIGH_RES;
            printf("Hi-res\n");
            break;
    }

    return cpu->memory[address];
}

uint8_t periferal_rom(CPU_6502 *cpu, void *unused, int address)
{
    return Disk2_rom[address - 0xC600];
}

uint8_t do_nothing(CPU_6502 *cpu, void *unused, int address, uint8_t val)
{
    printf("Doing nothing at addr 0x%04X\n", address);
    /* Not gonna write. */
    return 0;
}

int main(int argc, char **argv)
{
    /* XXX ROM HACK */
    Disk2_rom[0x4C] = 0xA9;
    Disk2_rom[0x4D] = 0x00;
    Disk2_rom[0x4E] = 0xEA;

    if(argc < 2)
    {
        fprintf(stderr, "Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }

    /* Eh. */
    SDL_Init(0);

    CPU_6502 apple_cpu;
    cpu6502_init(&apple_cpu, NULL);

    cpu6502_add_read_memory_map(&apple_cpu, NULL, test_read_callback, 0xC0);
    cpu6502_add_read_memory_map(&apple_cpu, NULL, periferal_rom, 0xC6);
    cpu6502_add_write_memory_map(&apple_cpu, NULL, do_nothing, 0xC6);
    cpu6502_add_write_memory_map(&apple_cpu, NULL, io_write_thing, 0xC0);

    int i;
    for(i = 0xD0; i < 0x100; i++)
    {
        cpu6502_add_write_memory_map(&apple_cpu, NULL, do_nothing, i);
    }

    /* Global variables are bad */
    init_disk();

    /* Load in the ROM */
    FILE *f = fopen(argv[1], "r");
    size_t len = fread(apple_cpu.memory + APPLE_ROM_OFFSET, 1, APPLE_ROM_SIZE, f);
    fclose(f);

    /* HACKY BOOT CODE!!!!! XXX */
//    FILE *img = fopen("Master.dsk", "r");
//    fseek(img, (0) * 256, 0);
//    fread(apple_cpu.memory + 0x800, 1, 256, img);
//    fseek(img, (9) * 256, 0);
//    fread(apple_cpu.memory + 0xBF00, 1, 256, img);
//    fclose(img);

    printf("Read %d bytes.\n", len);

    /* File wasn't large enough. */
    if(len < APPLE_ROM_SIZE)
    {
        fprintf(stderr, "File too small\n");
        exit(EXIT_FAILURE);
    }


    /* Make a screen */
    int ret = apple_scr_init(&apple_win, apple_cpu.memory + APPLE_TEXT_BUFFER_OFFSET, apple_cpu.memory + 0x2000);
    if(ret)
    {
        fprintf(stderr, "Error initializing screen\n");
        exit(EXIT_FAILURE);
    }

    apple_cpu.memory[0xC600] = 0xA2;
    apple_cpu.memory[0xC601] = 0x20;

    cpu6502_reset(&apple_cpu);

    SDL_StartTextInput();
    for(;;)
    {
        int time = SDL_GetTicks();

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_RETURN:
                            apple_cpu.memory[0xC000] = '\r' | 0x80;
                            break;
                        case SDLK_BACKSPACE:
                        case SDLK_LEFT:
                            apple_cpu.memory[0xC000] = 0x08 | 0x80;
                            break;
                        case SDLK_RIGHT:
                            apple_cpu.memory[0xC000] = 0x15 | 0x80;
                            break;
                        case SDLK_F2:
                            printf("Resetting CPU\n");
                            cpu6502_reset(&apple_cpu);
                            break;
                    }
                    break;

                case SDL_TEXTINPUT:
                    apple_cpu.memory[0xC000] = event.text.text[0] | 0x80;
                    break;

                case SDL_QUIT:
                    /* Rather do this than have a bool that is checked each loop */
                    goto end;
            }
        }

        int cycles = cpu6502_run(&apple_cpu, 10000);

        apple_scr_redraw(&apple_win);

//        int delta = SDL_GetTicks() - ((time + cycles)/2);
//        if(delta > 0)
//        {
//            SDL_Delay(delta);
//        }
    }

end:
    SDL_Quit();

    return 0;
}
