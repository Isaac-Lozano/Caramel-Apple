#include <SDL2/SDL.h>
#include <inttypes.h>

#define APPLE_SCR_WIDTH 280
#define APPLE_SCR_HEIGHT 192
#define APPLE_SCR_TEXT_WIDTH 40
#define APPLE_SCR_TEXT_HEIGHT 24
#define APPLE_SCR_CHAR_WIDTH 7
#define APPLE_SCR_CHAR_HEIGHT 8

typedef struct _apple_scr
{
    SDL_Window *win;
    SDL_Renderer *ren;

    /* Text mode primary and secondary */
    SDL_Surface *textp;
    SDL_Surface *texts;

    /* Low Graphics primary and secondary */
    SDL_Surface *lowgp;
    SDL_Surface *lowgs;

    /* High Graphics primary and secondary */
    SDL_Surface *highgp;
    SDL_Surface *highgs;

    int display_flags;
#define APPLE_SCR_MODE_GRAPHICS  0x01
#define APPLE_SCR_MODE_MIXED     0x02
#define APPLE_SCR_MODE_SECONDARY 0x04
#define APPLE_SCR_MODE_HIGH_RES  0x08

    uint8_t *text_buffer;
    uint8_t *graphics_buffer;
} AppleScr;

int apple_scr_init(AppleScr *awin, uint8_t *text_buffer, uint8_t *graphics_buffer);
void apple_scr_update(AppleScr *awin);
void apple_scr_redraw(AppleScr *awin);
void apple_scr_close(AppleScr *awin);
