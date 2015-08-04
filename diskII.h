#ifndef __DISKII_H__
#define __DISKII_H__

#include <inttypes.h>

#include "apple_II.h"

typedef struct _apple_drive
{
    /* Current sector and track */
    int sector;
    int track;

    /* Encoded sector */
    uint8_t sect_buf[1024];

    /* Current byte in the sector */
    uint8_t *index;

    /* Real file we're reading from */
    FILE *image;

    /* Magnets */
    int magnets;

    /* Current phase */
    int phase;
} AppleDrive;

typedef struct _apple_disk_II
{
    AppleII *parent;

    AppleDrive drives[2];

    uint8_t write_reg;

    int drive_num;

    int mode;
#define DISK_II_MODE_WRITE 0
#define DISK_II_MODE_READ  1

    int write_prot;
} AppleDiskII;

uint8_t diskII_reference(void *vdisk, int address);
uint8_t diskII_prom(CPU_6502 *cpu, void *ctx, int addr);
int diskII_init(AppleDiskII *disk, AppleII *parent, FILE *d1, FILE *d2);

#endif /* __DISKII_H__ */
