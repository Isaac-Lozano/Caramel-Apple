#include <inttypes.h>

#include "apple_II.h"
#include "diskII.h"
#include "language.h"

int main(int argc, char **argv)
{
    AppleII apple;
    AppleDiskII disk;

    uint8_t rom_buf[APPLE_II_ROM_SIZE];
    FILE *f = fopen(argv[1], "r");
    fread(rom_buf, 1, APPLE_II_ROM_SIZE, f);

    apple_II_init(&apple, rom_buf);

    FILE *img = fopen(argv[2], "r");
    diskII_init(&disk, &apple, img, NULL);

    apple_II_add_card(&apple, &disk, 6, diskII_reference, diskII_prom);

    LanguageCard card;

    language_init(&card, &apple, rom_buf);
    apple_II_add_card(&apple, &card, 0, language_io_switch, NULL);

    apple_II_run(&apple);

    return 0;
}
