#ifndef __6502_H__
#define __6502_H__

#include <inttypes.h>

#define CPU6502_MEM_SIZE 0x10000 /* 64 KiB */
#define CPU6502_RESET 0xFFFC
#define CPU6502_IRQ 0xFFFE

typedef struct _cpu_6502 CPU_6502;

typedef uint8_t (*cpu6502_read_func)(CPU_6502*, void*, int);
typedef uint8_t (*cpu6502_write_func)(CPU_6502*, void*, int, uint8_t);

typedef struct _cpu_6502_write_mmap
{
    void *ctx;
    cpu6502_write_func callback;
} cpu6502_WriteMMap;

typedef struct _cpu_6502_read_mmap
{
    void *ctx;
    cpu6502_read_func callback;
} cpu6502_ReadMMap;

typedef struct _cpu_6502
{
    /* Memory */
    /* Memory mapping will be done in read/write memory */
    uint8_t memory[CPU6502_MEM_SIZE];

    /* General Registers */
    uint8_t acc;
    uint8_t x;
    uint8_t y;

    /* Special Registers */
    uint8_t sp; /* Stack Pointer */
    uint16_t pc; /* Program Counter */

    union
    {
        uint8_t ps;

        struct
        {
            int c : 1;
            int z : 1;
            int i : 1;
            int d : 1;
            int b : 1;
            int UNUSED : 1;
            int v : 1;
            int s : 1;
        } flags;
    } ps;

/* PS FLAGS */
#define CPU6502_FLAG_S 0x80
#define CPU6502_FLAG_V 0x40
#define CPU6502_FLAG_UNUSED 0x20
#define CPU6502_FLAG_B 0x10
#define CPU6502_FLAG_D 0x08
#define CPU6502_FLAG_I 0x04
#define CPU6502_FLAG_Z 0x02
#define CPU6502_FLAG_C 0x01 

    /* Clock Cycles */
    /* Unused for now... Probably will be removed or replaced. */
    int cycles;

    /* Memory Mapping Callbacks */
    /* One callback per page */
    cpu6502_ReadMMap read_callbacks[0x100];
    cpu6502_WriteMMap write_callbacks[0x100];
} CPU_6502;

int cpu6502_excecute_opcode(CPU_6502 *cpu);
int cpu6502_run(CPU_6502 *cpu, int cycles);
void cpu6502_reset(CPU_6502 *cpu);
int cpu6502_init(CPU_6502 *cpu, uint8_t *mem);
int cpu6502_add_read_memory_map(CPU_6502 *cpu, void *ctx, cpu6502_read_func callback, int page);
int cpu6502_add_write_memory_map(CPU_6502 *cpu, void *ctx, cpu6502_write_func callback, int page);

#endif /* __6502_H__ */
