#ifndef __DISKII_H__
#define __DISKII_H__

#include <inttypes.h>

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
    AppleDrive drives[2];

    uint8_t write_reg;

    int drive_num;

    int mode;
#define DISK_II_MODE_WRITE 0
#define DISK_II_MODE_READ  1

    int write_prot;
} AppleDiskII;

uint8_t diskII_reference(AppleDiskII *disk, int address, int val);

#endif /* __DISKII_H__ */
