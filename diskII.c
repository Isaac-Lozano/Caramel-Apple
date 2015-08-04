#include <stdio.h>

#include "diskII.h"

#define NIB_ODD(x) (((x) >> 1) | 0xAA)
#define NIB_EVEN(x) ((x) | 0xAA)


/**********************************************
 * The tables below were taken from skrenta's *
 * Apple II emulator in C.                    *
 **********************************************/

/* The rom for the Disk2.
 * It will be "copied" into
 * the Apple II's memory.
 * Taken from Apple Win.
 */

char disk2_rom[] =
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

/*  Helps with the bit fiddling necessary to extract the bottom
 *  two bits during the 256 - 342 byte nibblize.
 */

unsigned char tab1[] = {
	0x00, 0x08, 0x04, 0x0C, 0x20, 0x28, 0x24, 0x2C,
	0x10, 0x18, 0x14, 0x1C, 0x30, 0x38, 0x34, 0x3C,
	0x80, 0x88, 0x84, 0x8C, 0xA0, 0xA8, 0xA4, 0xAC,
	0x90, 0x98, 0x94, 0x9C, 0xB0, 0xB8, 0xB4, 0xBC,
	0x40, 0x48, 0x44, 0x4C, 0x60, 0x68, 0x64, 0x6C,
	0x50, 0x58, 0x54, 0x5C, 0x70, 0x78, 0x74, 0x7C,
	0xC0, 0xC8, 0xC4, 0xCC, 0xE0, 0xE8, 0xE4, 0xEC,
	0xD0, 0xD8, 0xD4, 0xDC, 0xF0, 0xF8, 0xF4, 0xFC,
};


/*  Translates to "disk bytes"
 */

unsigned char tab2[] = {
	0x96, 0x97, 0x9A, 0x9B, 0x9D, 0x9E, 0x9F, 0xA6, 
	0xA7, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB2, 0xB3, 
	0xB4, 0xB5, 0xB6, 0xB7, 0xB9, 0xBA, 0xBB, 0xBC, 
	0xBD, 0xBE, 0xBF, 0xCB, 0xCD, 0xCE, 0xCF, 0xD3, 
	0xD6, 0xD7, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 
	0xDF, 0xE5, 0xE6, 0xE7, 0xE9, 0xEA, 0xEB, 0xEC, 
	0xED, 0xEE, 0xEF, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 
	0xF7, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 
};


/*  Dos 3.3 to physical sector conversion
 */

unsigned char phys[] = {
	0x00, 0x0D, 0x0B, 0x09, 0x07, 0x05, 0x03, 0x01,
	0x0E, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x0F,
};

void _step_motor(AppleDiskII *disk, int io_switch)
{
    AppleDrive *drive = &disk->drives[disk->drive_num];
    int phase_switch = 1 << (io_switch >> 1);

    if(io_switch & 1)
    {
        drive->magnets |= phase_switch;
    }
    else
    {
        drive->magnets &= ~phase_switch;
    }

    int direction = 0;
    if(drive->magnets & (1 << ((drive->phase + 1) & 3)))
    {
        direction += 1;
    }
    if(drive->magnets & (1 << ((drive->phase + 3) & 3)))
    {
        direction -= 1;
    }

    if(direction)
    {
        drive->phase += direction;
        if(drive->phase < 0)
        {
            drive->phase = 0;
        }
        else if(drive->phase > 140)
        {
            drive->phase = 140;
        }

        drive->index = NULL;
        drive->sector = 0;

        drive->track = (drive->phase + 1) / 2;
    }

//    printf("Phase switch is now %d\n", drive->phase);
//    printf("Track is %d\n", drive->track);
//    printf("Magnet %d (%d)\n", io_switch >> 1, io_switch % 2);

//    AppleDrive *drive = &disk->drives[disk->drive_num];
//    int phase_switch = io_switch >> 1;
//
//    /* If we're turning a switch on */
//    if(io_switch & 1)
//    {
//        if(drive->pphase == ((phase_switch - 1) & 3))
//        {
//            drive->phase++;
//            if(drive->phase > 140)
//            {
//                drive->phase = 140;
//            }
//
//            drive->index = NULL;
//            drive->sector = 0;
//        }
//        else if(drive->pphase == ((phase_switch + 1) & 3))
//        {
//            drive->phase--;
//            if(drive->phase < 0)
//            {
//                drive->phase = 0;
//            }
//
//            drive->index = NULL;
//            drive->sector = 0;
//        }
//
//        drive->pphase = phase_switch;
//    }
//
//    printf("Phase switch is now %d\n", drive->phase);
//    printf("Magnet %d (%d)\n", phase_switch, io_switch % 2);
//
//    drive->track = (drive->phase + 1) /2;
}

void write_log(uint8_t dat)
{
    static FILE *log;
    char buf[256];
    if(log == NULL)
    {
        log = fopen("rawdisk", "w");
    }

//    sprintf(buf, "\nTRACK %d SECTOR %d\n", drive->track, drive->sector);
//
//    fwrite(buf, 1, strlen(buf), log);
//    fwrite(drive->sect_buf, 1, strlen(drive->sect_buf), log);
    fwrite(&dat, 1, 1, log);
}

uint8_t *_encode_data(AppleDrive *drive, uint8_t *iter)
{
//    unsigned char buf[344];
//    unsigned char *one;
//    unsigned char *bump;
//    unsigned char *two;
//    unsigned char *three;
//    unsigned char *dest;
//    int i;
//
//    read_disk(track, sector, &buf[86]);
//    buf[342] = 0;
//    buf[343] = 0;
//
//    dest = buf;
//    one = &buf[86];
//    two = &buf[86 + 0x56];
//    bump = two;
//    three = &buf[86 + 0xAC];
//
//    do {
//        i = (*one++ & 0x03) |
//            ((*two++ & 0x03) << 2) | ((*three++ & 0x03) << 4);
//
//        *dest++ = tab1[i];
//    } while (one != bump);
//
//    sectp[0] = buf[0];
//    for (i = 1; i <= 342; i++)
//        sectp[i] = buf[i - 1] ^ buf[i];
//
//    for (i = 0; i <= 342; i++)
//        sectp[i] = tab2[ sectp[i] >> 2 ];
//
//    sectp = &sectp[343];
    unsigned char buf[344];
    unsigned char *one;
    unsigned char *bump;
    unsigned char *two;
    unsigned char *three;
    unsigned char *dest;
    int i;

    fseek(drive->image, (drive->track * 16 + drive->sector) * 256, 0);
    if(fread(&buf[86], sizeof(uint8_t), 256, drive->image) != 256)
    {
        printf("NMASDFASDF\n");
    }

    /* ? */
    buf[342] = buf[343] = 0;

    dest = buf;
    one = &buf[86];
    two = &buf[86 + 0x56];
    bump = two;
    three = &buf[86 + 0xAC];

    do {
        i = (*one++ & 0x03) |
            ((*two++ & 0x03) << 2) | ((*three++ & 0x03) << 4);

        *dest++ = tab1[i];
    } while (one != bump);

    iter[0] = buf[0];
    for (i = 1; i <= 342; i++)
        iter[i] = buf[i - 1] ^ buf[i];

    for (i = 0; i <= 342; i++)
        iter[i] = tab2[ iter[i] >> 2 ];

    iter = &iter[343];

//    write_log(drive);
    return iter;
}

void _setup_sector(AppleDrive *drive)
{
    int i;
    uint8_t *iter = drive->sect_buf;

    int phys_sector = phys[drive->sector];

//    printf("Translating %d to %d\n", drive->sector, phys_sector);

    for(i = 0; i < 16; i++)
    {
        *iter++ = 0xFF; /* GAP */
    }

    *iter++ = 0xD5;			/* address header */
    *iter++ = 0xAA;
    *iter++ = 0x96;

    *iter++ = 0xFF;			/* disk volume 254 */
    *iter++ = 0xFE;

    *iter++ = NIB_ODD(drive->track);
    *iter++ = NIB_EVEN(drive->track);

    *iter++ = NIB_ODD(phys_sector);
    *iter++ = NIB_EVEN(phys_sector);

    int checksum = 254 ^ (drive->track) ^ phys_sector;
    *iter++ = NIB_ODD(checksum);
    *iter++ = NIB_EVEN(checksum);

    *iter++ = 0xDE;			/* address trailer */
    *iter++ = 0xAA;
    *iter++ = 0xEB;

    for (i = 0; i < 8; i++)
    {
        *iter++ = 0xFF;
    }

    *iter++ = 0xD5;			/* data header */
    *iter++ = 0xAA;
    *iter++ = 0xAD;

    iter = _encode_data(drive, iter);		/* nibblized data */

    *iter++ = 0xDE;			/* data trailer */
    *iter++ = 0xAA;
    *iter++ = 0xEB;

    *iter = '\0';				/* ending mark for our use */
    drive->index = drive->sect_buf;
}

uint8_t diskII_reference(void *vdisk, int address)
{
    AppleDiskII *disk = (AppleDiskII *) vdisk;

    int io_switch = address & 0x0F;
    switch(io_switch)
    {
        /* Phase control */
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            _step_motor(disk, io_switch);


        /* Motor control. */
        case 0x08: /* Motor off */
        case 0x09: /* Motor on */
            /* We don't care about the motor */
            break;


        /* Do we care about keeping the head
         * in the same spot between drives?
         */
        case 0x0A:
            printf("Drive is now 0\n");
            disk->drive_num = 0;
            break;

        case 0x0B:
            printf("Drive is now 1\n");
            disk->drive_num = 1;
            break;

        /* Q switches */
        case 0x0C: /* Data shift register */
            if(disk->mode == DISK_II_MODE_WRITE)
            {
                printf("Trying to write 0x%02X to disk %d\n", disk->write_reg, disk->drive_num);
            }
            else
            {
                if(disk->drives[disk->drive_num].index == NULL || *disk->drives[disk->drive_num].index == '\0')
                {
                    printf("Making new sector.\n");
                    disk->drives[disk->drive_num].sector--;
                    if(disk->drives[disk->drive_num].sector < 0)
                    {
                        disk->drives[disk->drive_num].sector = 15;
                    }
                    _setup_sector(&disk->drives[disk->drive_num]);
                } 

//                printf("Trying to read from disk %d, track %d, sector %d\n", disk->drive_num, disk->drives[disk->drive_num].track, disk->drives[disk->drive_num].sector);
//                printf("read 0x%02X\n", *disk->drives[disk->drive_num].index);

//                write_log(*disk->drives[disk->drive_num].index);
                return *disk->drives[disk->drive_num].index++;
            }

            break;

        case 0x0D: /* Load data registar */
            {
                uint8_t val = disk->parent->cpu.memory[address];
                printf("loading 0x%02X to data register\n", val);
                disk->write_reg = val;
                break;
            }

        case 0x0E: /* Read mode */
            printf("Read Mode\n");
            disk->mode = DISK_II_MODE_READ;
            if(disk->write_prot)
            {
                return 0xFF;
            }
            break;

        case 0x0F: /* Write mode */
            printf("Write Mode\n");
            disk->mode = DISK_II_MODE_WRITE;
            break;
    }

    return 0;
}

uint8_t diskII_prom(CPU_6502 *cpu, void *ctx, int addr)
{
    return (uint8_t) disk2_rom[addr & 0xFF];
}

int diskII_init(AppleDiskII *disk, AppleII *parent, FILE *d1, FILE *d2)
{
    memset(disk, 0, sizeof(AppleDiskII));
    disk->parent = parent;
    disk->drives[0].image = d1;
    disk->drives[1].image = d2;

    /* XXX ROM HACK */
    /* This removes some "ugly" delay functions */
    disk2_rom[0x4C] = 0xA9;
    disk2_rom[0x4D] = 0x00;
    disk2_rom[0x4E] = 0xEA;

    return 0;
}
