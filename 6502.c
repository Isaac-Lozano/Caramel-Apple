/* A 6502 cpu emulator.
 * Created by OnVar
 *
 * TODO:
 * + Proper cycle counts
 * + Writing memory maps
 * + Interrupts
 * + All opcodes
 * + Stuff.
 * + Possibly typecast things?
 *  - uint8_t -> cpu6502_byte
 *  - uint16_t -> cpu6502_addr
 */

#include "6502.h"

#include <string.h>
#include <stdio.h>

#define SET_SIGN(num) ((cpu->ps.flags.s) = (num & 0xFF) >> 7)
#define SET_ZERO(num) ((cpu->ps.flags.z) = (num & 0xFF) == 0 ? 1 : 0)
#define SET_CARRY(num) (cpu->ps.flags.c = (num) > 0xFF ? 1 : 0)
#define SET_SUB_CARRY(num) (cpu->ps.flags.c = (unsigned int) (num) < 0x100 ? 1 : 0)


/* These macros are defined for the different addressing modes of the CPU. */

#define GET_IMMEDIATE(src) \
    do{\
        src = _read_byte(cpu, cpu->pc);\
        cpu->pc += 1;\
    } while(0)

#define GET_ZERO_PAGE(src) \
    do{\
        uint8_t addr = _read_byte(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 1;\
    } while(0)

#define GET_ZERO_PAGE_X(src) \
    do{\
        uint8_t addr = cpu->x + _read_byte(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 1;\
    } while(0)

#define GET_ZERO_PAGE_Y(src) \
    do{\
        uint8_t addr = cpu->y + _read_byte(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 1;\
    } while(0)

#define GET_ABSOLUTE(src) \
    do{\
        uint16_t addr = _read_short(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 2;\
    } while(0)

#define GET_ABSOLUTE_X(src) \
    do{\
        uint16_t addr = cpu->x + _read_short(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 2;\
    } while(0)

#define GET_ABSOLUTE_Y(src) \
    do{\
        uint16_t addr = cpu->y + _read_short(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 2;\
    } while(0)

#define GET_INDIRECT(src) \
    do{\
        uint16_t addr = _read_short(cpu, cpu->pc);\
        src = _read_short(cpu, addr);\
        cpu->pc += 2;\
    } while(0)

#define GET_INDIRECT_X(src) \
    do{\
        uint8_t addr_addr = cpu->x + _read_byte(cpu, cpu->pc);\
        uint16_t addr = _read_short(cpu, addr_addr);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 1;\
    } while(0)

#define GET_INDIRECT_Y(src) \
    do{\
        uint8_t addr_addr = _read_byte(cpu, cpu->pc);\
        uint16_t addr = cpu->y + _read_short(cpu, addr_addr);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 1;\
    } while(0)

/* Some opcodes have us save data in the same address we got it from. */

/* addr must be a uint8_t!!! */
#define GET_ZERO_PAGE_SAVE_ADDR(src, addr) \
    do{\
        addr = _read_byte(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 1;\
    } while(0)

/* addr must be a uint8_t!!! */
#define GET_ZERO_PAGE_X_SAVE_ADDR(src, addr) \
    do{\
        addr = cpu->x + _read_byte(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 1;\
    } while(0)

/* addr must be a uint16_t!!! */
#define GET_ABSOLUTE_SAVE_ADDR(src, addr) \
    do{\
        addr = _read_short(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 2;\
    } while(0)

/* addr must be a uint16_t!!! */
#define GET_ABSOLUTE_X_SAVE_ADDR(src, addr) \
    do{\
        addr = cpu->x + _read_short(cpu, cpu->pc);\
        src = _read_byte(cpu, addr);\
        cpu->pc += 2;\
    } while(0)


/* Stack operations */

/* num should be a uint8_t */
#define PUSH(num) \
    do{\
        _write_byte(cpu, 0x0100 | cpu->sp, num);\
        cpu->sp--;\
    } while(0)

/* num should be a uint8_t */
#define PULL(num) \
    do{\
        cpu->sp++;\
        num = _read_byte(cpu, 0x0100 | cpu->sp);\
    } while(0)


void cpu6502_print_status(CPU_6502 *cpu)
{
    printf("acc = 0x%02x\n", cpu->acc);
    printf("x = 0x%02x\n", cpu->x);
    printf("y = 0x%02x\n", cpu->y);
    printf("pc = 0x%04x\n", cpu->pc);
    printf("sp = 0x%02x\n", cpu->sp);
    printf("ps = 0x%02x (", cpu->ps.ps);
    if(cpu->ps.flags.s)
    {
        printf("S");
    }
    else
    {
        printf("s");
    }
    if(cpu->ps.flags.v)
    {
        printf("V");
    }
    else
    {
        printf("v");
    }
    if(cpu->ps.flags.UNUSED)
    {
        printf("U");
    }
    else
    {
        printf("u");
    }
    if(cpu->ps.flags.b)
    {
        printf("B");
    }
    else
    {
        printf("b");
    }
    if(cpu->ps.flags.d)
    {
        printf("D");
    }
    else
    {
        printf("d");
    }
    if(cpu->ps.flags.i)
    {
        printf("I");
    }
    else
    {
        printf("i");
    }
    if(cpu->ps.flags.z)
    {
        printf("Z");
    }
    else
    {
        printf("z");
    }
    if(cpu->ps.flags.c)
    {
        printf("C");
    }
    else
    {
        printf("c");
    }
    printf(")\n");
    printf("\n");
}

uint8_t inline _read_byte(CPU_6502 *cpu, int address)
{
    /* Check if callback exsists for this page.
     * If so, use it.
     */

    uint8_t val;

    if(cpu->read_callbacks[address >> 8].callback)
    {
        val = cpu->read_callbacks[address >> 8].callback(cpu, cpu->read_callbacks[address >> 8].ctx, address);
    }
    else
    {
        val = cpu->memory[address];
    }

#if DEBUG
    printf("Reading 0x%02X from 0x%04X\n", val, address);
#endif

    return val;
}

uint16_t inline _read_short(CPU_6502 *cpu, int address)
{
    return _read_byte(cpu, address) | (_read_byte(cpu, address+1) << 8);
}

void inline _write_byte(CPU_6502 *cpu, int address, uint8_t val)
{
#if DEBUG
    printf("Writing 0x%02x to 0x%04x\n", val, address);
#endif

    /* Check if callback exsists for this page.
     * If so, use it.
     */
    if(cpu->write_callbacks[address >> 8].callback)
    {
        cpu->write_callbacks[address >> 8].callback(cpu, cpu->read_callbacks[address >> 8].ctx, address, val);
        return;
    }

    cpu->memory[address] = val;
}

void inline _write_short(CPU_6502 *cpu, int address, uint16_t val)
{
    _write_byte(cpu, address, val & 0xFF);
    _write_byte(cpu, address+1, val & 0xFF00 >> 8);
}

int cpu6502_excecute_opcode(CPU_6502 *cpu)
{
    int cycles = 0;
    uint8_t op = _read_byte(cpu, cpu->pc);
    cpu->pc += 1;
#if DEBUG
    printf("OP: 0x%02x\n", op);
#endif

    int src;
    switch(op)
    {
        /* ADC */
        case 0x69:
            {
                GET_IMMEDIATE(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0x65:
            {
                GET_ZERO_PAGE(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0x75:
            {
                GET_ZERO_PAGE_X(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0x6D:
            {
                GET_ABSOLUTE(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0x7D:
            {
                GET_ABSOLUTE_X(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0x79:
            {
                GET_ABSOLUTE_Y(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0x61:
            {
                GET_INDIRECT_X(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0x71:
            {
                GET_INDIRECT_Y(src);
                int temp = cpu->acc + src + (cpu->ps.flags.c & 0x01);
                cpu->ps.flags.v = (~(cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }


        /* AND */
        case 0x29:
            {
                GET_IMMEDIATE(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x25:
            {
                GET_ZERO_PAGE(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x35:
            {
                GET_ZERO_PAGE_X(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x2D:
            {
                GET_ABSOLUTE(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x3D:
            {
                GET_ABSOLUTE_X(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x39:
            {
                GET_ABSOLUTE_Y(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x21:
            {
                GET_INDIRECT_X(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x31:
            {
                GET_INDIRECT_Y(src);
                cpu->acc &= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }


        /* ASL */
        case 0x0A:
            {
                /* Not using the SET_CARRY macro here because
                 * we're going to keep the calculations on
                 * one variable (cpu->acc) which is a 8-bit
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                cpu->ps.flags.c = (cpu->acc & 0x80) >> 7;
                cpu->acc <<= 1;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x06:
            {
                uint16_t addr;
                GET_ZERO_PAGE_SAVE_ADDR(src, addr);
                src <<= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x16:
            {
                uint16_t addr;
                GET_ZERO_PAGE_X_SAVE_ADDR(src, addr);
                src <<= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x0E:
            {
                uint16_t addr;
                GET_ABSOLUTE_SAVE_ADDR(src, addr);
                src <<= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x1E:
            {
                uint16_t addr;
                GET_ABSOLUTE_X_SAVE_ADDR(src, addr);
                src <<= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }


        /* BIT */
        case 0x24:
            {
                GET_ZERO_PAGE(src);

                SET_ZERO(src & cpu->acc);
                SET_SIGN(src);
                /* We set this manually because
                 * it is quite different than how
                 * it is normally set.
                 */
                cpu->ps.flags.v = (src & 0x40) >> 6;
                cycles += 3;
                break;
            }
        case 0x2C:
            {
                GET_ABSOLUTE(src);

                SET_ZERO(src & cpu->acc);
                SET_SIGN(src);
                /* We set this manually because
                 * it is quite different than how
                 * it is normally set.
                 */
                cpu->ps.flags.v = (src & 0x40) >> 6;
                cycles += 4;
                break;
            }


        /* BPL */
        case 0x10:
            {
                GET_IMMEDIATE(src);
                if(!cpu->ps.flags.s)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BMI */
        case 0x30:
            {
                GET_IMMEDIATE(src);
                if(cpu->ps.flags.s)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BVC */
        case 0x50:
            {
                GET_IMMEDIATE(src);
                if(!cpu->ps.flags.v)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BVS */
        case 0x70:
            {
                GET_IMMEDIATE(src);
                if(cpu->ps.flags.v)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BCC */
        case 0x90:
            {
                GET_IMMEDIATE(src);
                if(!cpu->ps.flags.c)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BCS */
        case 0xB0:
            {
                GET_IMMEDIATE(src);
                if(cpu->ps.flags.c)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BNE */
        case 0xD0:
            {
                GET_IMMEDIATE(src);
                if(!cpu->ps.flags.z)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BEQ */
        case 0xF0:
            {
                GET_IMMEDIATE(src);
                if(cpu->ps.flags.z)
                {
                    /* TODO: cycle stuff. */
                    cpu->pc += (int8_t) src;
                }
                cycles += 2;
                break;
            }


        /* BRK */
        case 0x00:
            {
                /* Increment pc first.
                 * It is dumb.
                 */
                cpu->pc += 1;
                PUSH(cpu->pc >> 8);
                PUSH(cpu->pc & 0xFF);
                PUSH(cpu->ps.ps | CPU6502_FLAG_B);
                cpu->ps.flags.i = 1;

                cpu->pc = _read_short(cpu, CPU6502_IRQ);
                cycles += 7;
                break; /* /pun */
            }


        /* CMP */
        case 0xC9:
            {
                GET_IMMEDIATE(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xC5:
            {
                GET_ZERO_PAGE(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xD5:
            {
                GET_ZERO_PAGE_X(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xCD:
            {
                GET_ABSOLUTE(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xDD:
            {
                GET_ABSOLUTE_X(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xD9:
            {
                GET_ABSOLUTE_Y(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xC1:
            {
                GET_INDIRECT_X(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xD1:
            {
                GET_INDIRECT_Y(src);
                int temp = cpu->acc - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->acc >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }


        /* CPX */
        case 0xE0:
            {
                GET_IMMEDIATE(src);
                int temp = cpu->x - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->x >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xE4:
            {
                GET_ZERO_PAGE(src);
                int temp = cpu->x - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->x >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xEC:
            {
                GET_ABSOLUTE(src);
                int temp = cpu->x - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->x >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }


        /* CPY */
        case 0xC0:
            {
                GET_IMMEDIATE(src);
                int temp = cpu->y - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->y >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xC4:
            {
                GET_ZERO_PAGE(src);
                int temp = cpu->y - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->y >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }
        case 0xCC:
            {
                GET_ABSOLUTE(src);
                int temp = cpu->y - src;

                /* Set manually because it's different. */
                cpu->ps.flags.c = cpu->y >= src ? 1 : 0;
                SET_SIGN(temp);
                SET_ZERO(temp);
                cycles += 2;
                break;
            }


        /* DEC */
        case 0xC6:
            {
                uint16_t addr;
                GET_ZERO_PAGE_SAVE_ADDR(src, addr);
                src--;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0xD6:
            {
                uint16_t addr;
                GET_ZERO_PAGE_X_SAVE_ADDR(src, addr);
                src--;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0xCE:
            {
                uint16_t addr;
                GET_ABSOLUTE_SAVE_ADDR(src, addr);
                src--;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0xDE:
            {
                uint16_t addr;
                GET_ABSOLUTE_X_SAVE_ADDR(src, addr);
                src--;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }


        /* EOR */
        case 0x49:
            {
                GET_IMMEDIATE(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x45:
            {
                GET_ZERO_PAGE(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x55:
            {
                GET_ZERO_PAGE_X(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x4D:
            {
                GET_ABSOLUTE(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x5D:
            {
                GET_ABSOLUTE_X(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x59:
            {
                GET_ABSOLUTE_Y(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x41:
            {
                GET_INDIRECT_X(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x51:
            {
                GET_INDIRECT_Y(src);
                cpu->acc ^= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }


        /* CLC */
        case 0x18:
            {
                cpu->ps.flags.c = 0;

                cycles += 2;
                break;
            }


        /* SEC */
        case 0x38:
            {
                cpu->ps.flags.c = 1;

                cycles += 2;
                break;
            }


        /* CLI */
        case 0x58:
            {
                cpu->ps.flags.i = 0;

                cycles += 2;
                break;
            }


        /* SEI */
        case 0x78:
            {
                cpu->ps.flags.i = 1;

                cycles += 2;
                break;
            }


        /* CLV */
        case 0xB8:
            {
                cpu->ps.flags.v = 0;

                cycles += 2;
                break;
            }


        /* CLD */
        case 0xD8:
            {
                cpu->ps.flags.d = 0;

                cycles += 2;
                break;
            }


        /* SED */
        case 0xF8:
            {
                cpu->ps.flags.d = 1;

                cycles += 2;
                break;
            }


        /* INC */
        case 0xE6:
            {
                uint16_t addr;
                GET_ZERO_PAGE_SAVE_ADDR(src, addr);
                src++;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0xF6:
            {
                uint16_t addr;
                GET_ZERO_PAGE_X_SAVE_ADDR(src, addr);
                src++;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0xEE:
            {
                uint16_t addr;
                GET_ABSOLUTE_SAVE_ADDR(src, addr);
                src++;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0xFE:
            {
                uint16_t addr;
                GET_ABSOLUTE_X_SAVE_ADDR(src, addr);
                src++;

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }


        /* JMP */
        /* This opcode has some out-of-ordinary properties.
         * We're gonna do read/write manually here.
         */
        case 0x4C:
            {
                /* Absolute */
                cpu->pc = _read_short(cpu, cpu->pc);
                cycles += 2;
                break;
            }
        case 0x6C:
            {
                /* Indirect */
                src = _read_short(cpu, cpu->pc);
                cpu->pc = _read_byte(cpu, src) | (_read_byte(cpu, (src+1) & 0xFFFF) << 8);
                cycles += 2;
                break;
            }


        /* JSR */
        case 0x20:
            {
                /* It decrements pc first.
                 * I don't understand why they do it
                 * but they do.
                 */
                uint16_t addr = _read_short(cpu, cpu->pc);
                cpu->pc += 1; /* Equal to pc += 2; pc--; */
                PUSH(cpu->pc >> 8);
                PUSH(cpu->pc & 0xFF);
                cpu->pc = addr;
                cycles += 2;
                break;
            }


        /* LDA */
        case 0xA9:
            {
                GET_IMMEDIATE(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0xA5:
            {
                GET_ZERO_PAGE(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 3;
                break;
            }
        case 0xB5:
            {
                GET_ZERO_PAGE_X(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 4;
                break;
            }
        case 0xAD:
            {
                GET_ABSOLUTE(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 4;
                break;
            }
        case 0xBD:
            {
                GET_ABSOLUTE_X(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 4;
                break;
            }
        case 0xB9:
            {
                GET_ABSOLUTE_Y(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 4;
                break;
            }
        case 0xA1:
            {
                GET_INDIRECT_X(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 4;
                break;
            }
        case 0xB1:
            {
                GET_INDIRECT_Y(src);
                cpu->acc = src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 4;
                break;
            }


        /* LDX */
        case 0xA2:
            {
                GET_IMMEDIATE(src);
                cpu->x = src;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 2;
                break;
            }
        case 0xA6:
            {
                GET_ZERO_PAGE(src);
                cpu->x = src;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 3;
                break;
            }
        case 0xB6:
            {
                GET_ZERO_PAGE_Y(src);
                cpu->x = src;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 4;
                break;
            }
        case 0xAE:
            {
                GET_ABSOLUTE(src);
                cpu->x = src;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 4;
                break;
            }
        case 0xBE:
            {
                GET_ABSOLUTE_Y(src);
                cpu->x = src;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 4;
                break;
            }


        /* LDY */
        case 0xA0:
            {
                GET_IMMEDIATE(src);
                cpu->y = src;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 2;
                break;
            }
        case 0xA4:
            {
                GET_ZERO_PAGE(src);
                cpu->y = src;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 3;
                break;
            }
        case 0xB4:
            {
                GET_ZERO_PAGE_X(src);
                cpu->y = src;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 4;
                break;
            }
        case 0xAC:
            {
                GET_ABSOLUTE(src);
                cpu->y = src;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 4;
                break;
            }
        case 0xBC:
            {
                GET_ABSOLUTE_X(src);
                cpu->y = src;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 4;
                break;
            }


        /* LSR */
        case 0x4A:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing LSR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                cpu->ps.flags.c = (cpu->acc & 0x01);
                cpu->acc >>= 1;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x46:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing LSR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ZERO_PAGE_SAVE_ADDR(src, addr);
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x56:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing LSR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ZERO_PAGE_X_SAVE_ADDR(src, addr);
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x4E:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing LSR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ABSOLUTE_SAVE_ADDR(src, addr);
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x5E:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing LSR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ABSOLUTE_X_SAVE_ADDR(src, addr);
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }


        /* NOP */
        case 0xEA:
            {
                cycles += 2;
                break;
            }


        /* ORA */
        case 0x09:
            {
                GET_IMMEDIATE(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x05:
            {
                GET_ZERO_PAGE(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x15:
            {
                GET_ZERO_PAGE_X(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x0D:
            {
                GET_ABSOLUTE(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x1D:
            {
                GET_ABSOLUTE_X(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x19:
            {
                GET_ABSOLUTE_Y(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x01:
            {
                GET_INDIRECT_X(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x11:
            {
                GET_INDIRECT_Y(src);
                cpu->acc |= src;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }


        /* TAX */
        case 0xAA:
            {
                cpu->x = cpu->acc;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 2;
                break;
            }


        /* TXA */
        case 0x8A:
            {
                cpu->acc = cpu->x;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }


        /* DEX */
        case 0xCA:
            {
                cpu->x--;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 2;
                break;
            }


        /* INX */
        case 0xE8:
            {
                cpu->x++;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 2;
                break;
            }


        /* TAY */
        case 0xA8:
            {
                cpu->y = cpu->acc;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }


        /* TYA */
        case 0x98:
            {
                cpu->acc = cpu->y;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 2;
                break;
            }


        /* DEY */
        case 0x88:
            {
                cpu->y--;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 2;
                break;
            }


        /* INY */
        case 0xC8:
            {
                cpu->y++;

                SET_SIGN(cpu->y);
                SET_ZERO(cpu->y);
                cycles += 2;
                break;
            }


        /* ASL */
        case 0x2A:
            {
                /* Not using the SET_CARRY macro here because
                 * we're going to keep the calculations on
                 * one variable (cpu->acc) which is a 8-bit
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                int temp = cpu->ps.flags.c;
                cpu->ps.flags.c = (cpu->acc & 0x80) >> 7;
                cpu->acc <<= 1;
                cpu->acc |= temp & 0x01;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x26:
            {
                uint16_t addr;
                GET_ZERO_PAGE_SAVE_ADDR(src, addr);
                src <<= 1; 
                src |= cpu->ps.flags.c & 0x01;

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x36:
            {
                uint16_t addr;
                GET_ZERO_PAGE_X_SAVE_ADDR(src, addr);
                src <<= 1; 
                src |= cpu->ps.flags.c & 0x01;

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x2E:
            {
                uint16_t addr;
                GET_ABSOLUTE_SAVE_ADDR(src, addr);
                src <<= 1; 
                src |= cpu->ps.flags.c & 0x01;

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x3E:
            {
                uint16_t addr;
                GET_ABSOLUTE_X_SAVE_ADDR(src, addr);
                src <<= 1; 
                src |= cpu->ps.flags.c & 0x01;

                SET_SIGN(src);
                SET_ZERO(src);
                SET_CARRY(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }


        /* ROR */
        case 0x6A:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing ROR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                int temp = cpu->ps.flags.c;
                cpu->ps.flags.c = (cpu->acc & 0x01);
                cpu->acc >>= 1;
                cpu->acc |= temp << 7;

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 2;
                break;
            }
        case 0x66:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing ROR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ZERO_PAGE_SAVE_ADDR(src, addr);
                src |= cpu->ps.flags.c << 8; /* Will be shifted in. */
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x76:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing ROR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ZERO_PAGE_X_SAVE_ADDR(src, addr);
                src |= cpu->ps.flags.c << 8; /* Will be shifted in. */
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x6E:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing ROR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ABSOLUTE_SAVE_ADDR(src, addr);
                src |= cpu->ps.flags.c << 8; /* Will be shifted in. */
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }
        case 0x7E:
            {
                /* Not using the SET_CARRY macro here because
                 * It's a bit weird when doing ROR on a
                 * number. We have to set the carry bit
                 * manually before we do the shift.
                 */
                uint16_t addr;
                GET_ABSOLUTE_X_SAVE_ADDR(src, addr);
                src |= cpu->ps.flags.c << 8; /* Will be shifted in. */
                cpu->ps.flags.c = (src & 0x01);
                src >>= 1; 

                SET_SIGN(src);
                SET_ZERO(src);
                _write_byte(cpu, addr, src);
                cycles += 2;
                break;
            }


        /* RTI */
        case 0x40:
            {
                uint16_t addr;
                PULL(cpu->ps.ps);
                PULL(src);
                addr = src;
                PULL(src);
                addr |= src << 8;
                cpu->pc = addr;

                cycles += 6;
                break;
            }


        /* RTS */
        case 0x60:
            {
                uint16_t addr;
                PULL(src);
                addr = src;
                PULL(src);
                addr |= src << 8;
                cpu->pc = addr + 1;

                cycles += 6;
                break;
            }


        /* SBC */
        case 0xE9:
            {
                GET_IMMEDIATE(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0xE5:
            {
                GET_ZERO_PAGE(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0xF5:
            {
                GET_ZERO_PAGE_X(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0xED:
            {
                GET_ABSOLUTE(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0xFD:
            {
                GET_ABSOLUTE_X(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0xF9:
            {
                GET_ABSOLUTE_Y(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0xE1:
            {
                GET_INDIRECT_X(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }
        case 0xF1:
            {
                GET_INDIRECT_Y(src);
                int temp = cpu->acc - src - (cpu->ps.flags.c ? 0 : 1);
                cpu->ps.flags.v = ((cpu->acc ^ src) & (cpu->acc ^ temp) & 0x80) >> 7; /* Overflow check. */
                                                  /* TODO: Possibly mcaro? */
                SET_SIGN(temp);
                SET_ZERO(temp);
                SET_SUB_CARRY(temp);
                /* TODO: Decimal mode */
                cpu->acc = temp & 0xFF;
                cycles += 2;
                break;
            }


        /* STA */
        case 0x85:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Zero Page */
                src = _read_byte(cpu, cpu->pc);
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->acc);
                cycles += 2;
                break;
            }
        case 0x95:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Zero Page X */
                src = (cpu->x + _read_byte(cpu, cpu->pc)) & 0xFF;
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->acc);
                cycles += 2;
                break;
            }
        case 0x8D:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Absolute */
                src = _read_short(cpu, cpu->pc);
                cpu->pc += 2;

                _write_byte(cpu, src, cpu->acc);
                cycles += 2;
                break;
            }
        case 0x9D:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Absolute X */
                src = cpu->x + _read_short(cpu, cpu->pc);
                cpu->pc += 2;

                _write_byte(cpu, src, cpu->acc);
                cycles += 2;
                break;
            }
        case 0x99:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Absolute Y */
                src = cpu->y + _read_short(cpu, cpu->pc);
                cpu->pc += 2;

                _write_byte(cpu, src, cpu->acc);
                cycles += 2;
                break;
            }
        case 0x81:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Indirect X */
                src = (cpu->x + _read_byte(cpu, cpu->pc)) & 0xFF;
                src = _read_short(cpu, src);
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->acc);
                cycles += 2;
                break;
            }
        case 0x91:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Indirect Y */
                src = _read_byte(cpu, cpu->pc);
                src = (cpu->y + _read_short(cpu, src)) & 0xFFFF; /* TODO: src is possibly uint16_t */
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->acc);
                cycles += 2;
                break;
            }


        /* TXS */
        case 0x9A:
            {
                cpu->sp = cpu->x;

                cycles += 2;
                break;
            }


        /* TSX */
        case 0xBA:
            {
                cpu->x = cpu->sp;

                SET_SIGN(cpu->x);
                SET_ZERO(cpu->x);
                cycles += 2;
                break;
            }


        /* PHA */
        case 0x48:
            {
                PUSH(cpu->acc);

                cycles += 3;
                break;
            }


        /* PLA */
        case 0x68:
            {
                PULL(cpu->acc);

                SET_SIGN(cpu->acc);
                SET_ZERO(cpu->acc);
                cycles += 4;
                break;
            }


        /* PHP */
        case 0x08:
            {
                /* This sets the break flag for some
                 * ungodly reason.
                 * (It's probably a good reason, but
                 * I just don't know it yet.)
                 * (I've always hated PHP...)
                 */
                cpu->ps.flags.b = 1;
                PUSH(cpu->ps.ps);

                cycles += 3;
                break;
            }


        /* PLP */
        case 0x28:
            {
                PULL(cpu->ps.ps);
                /* The UNUSED flag is ALWAYS SET.
                 * IF YOU TRY TO GET AROUND THIS, 
                 * WE WILL REPORT YOU TO THE
                 * AUTHORITIES.
                 *
                 * if(!cpu->ps.flags.UNUSED)
                 * {
                 *     CONTACT_AUTHORITIES(get_country());
                 * }
                 */

                cpu->ps.flags.UNUSED = 1;

                cycles += 4;
                break;
            }


        /* STX */
        case 0x86:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Zero Page */
                src = _read_byte(cpu, cpu->pc);
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->x);
                cycles += 2;
                break;
            }
        case 0x96:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Zero Page Y */
                src = (cpu->y + _read_byte(cpu, cpu->pc)) & 0xFF;
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->x);
                cycles += 2;
                break;
            }
        case 0x8E:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Absolute */
                src = _read_short(cpu, cpu->pc);
                cpu->pc += 2;

                _write_byte(cpu, src, cpu->x);
                cycles += 2;
                break;
            }


        /* STY */
        case 0x84:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Zero Page */
                src = _read_byte(cpu, cpu->pc);
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->y);
                cycles += 2;
                break;
            }
        case 0x94:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Zero Page X */
                src = (cpu->x + _read_byte(cpu, cpu->pc)) & 0xFF;
                cpu->pc += 1;

                _write_byte(cpu, src, cpu->y);
                cycles += 2;
                break;
            }
        case 0x8C:
            {
                /* This will be different because it doesn't
                 * actually read the data it is is writing to.
                 * So we will do things manually, sadly.
                 */
                /* Absolute */
                src = _read_short(cpu, cpu->pc);
                cpu->pc += 2;

                _write_byte(cpu, src, cpu->y);
                cycles += 2;
                break;
            }


        /* TODO: Add "unofficial" opcodes */
        /* UNUSED OPS */
        default:
            {
#if DEBUG
#endif
                printf("BAD OP 0x%02x\n", op);
                getchar();
                cycles += 1;
            }
    }
    return cycles;
}

void dump_rom(CPU_6502 *cpu)
{
    FILE *f = fopen("memdump.rom", "w");
    fwrite(cpu->memory, 1, 0x10000, f);
    fclose(f);
    printf("Dumped ram to memdump.rom\n");
    getchar();
}

int cpu6502_run(CPU_6502 *cpu, int cycles)
{
    int passed = 0;
    while(passed < cycles)
    {
//        if((cpu->pc >> 8) == 0xC6 && cpu->pc > 0xC6A4)
//        {
//            getchar();
//        }
        if(cpu->pc == 0x0801)
        {
            //dump_rom(cpu);
        }
        passed += cpu6502_excecute_opcode(cpu);
#if DEBUG
        cpu6502_print_status(cpu);
#endif
    }
    return passed;
}

void cpu6502_reset(CPU_6502 *cpu)
{
    cpu->pc = _read_short(cpu, CPU6502_RESET);
}

int cpu6502_init(CPU_6502 *cpu, uint8_t *mem)
{
    memset(cpu->read_callbacks, 0, sizeof(cpu->read_callbacks));
    memset(cpu->write_callbacks, 0, sizeof(cpu->write_callbacks));

    if(mem != NULL)
    {
        memcpy(cpu->memory, mem, CPU6502_MEM_SIZE);
    }

    cpu->ps.flags.i = 1;
    cpu->ps.flags.UNUSED = 1;

    return 0;
}

int cpu6502_add_read_memory_map(CPU_6502 *cpu, void *ctx, cpu6502_read_func callback, int page)
{
    if(page < 0x00 || page > 0xFF)
    {
        return 1;
    }

    /* Note to self, stop coding after 1:00 AM */
    cpu6502_ReadMMap this_should_be_obvious;
    this_should_be_obvious.ctx = ctx;
    this_should_be_obvious.callback = callback;

    cpu->read_callbacks[page] = this_should_be_obvious;

    return 0;
}

int cpu6502_add_write_memory_map(CPU_6502 *cpu, void *ctx, cpu6502_write_func callback, int page)
{
    if(page < 0x00 || page > 0xFF)
    {
        return 1;
    }

    /* Note to self, stop coding after 1:00 AM */
    cpu6502_WriteMMap this_should_be_obvious;
    this_should_be_obvious.ctx = ctx;
    this_should_be_obvious.callback = callback;

    cpu->write_callbacks[page] = this_should_be_obvious;

    return 0;
}
