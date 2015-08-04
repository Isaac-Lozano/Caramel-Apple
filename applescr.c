#include "applescr.h"

#include <stdio.h>

uint8_t charmap[][8] =
{
    {
        /* @ */
        0x00,
        0x1C,
        0x22,
        0x2A,
        0x2E,
        0x2C,
        0x20,
        0x1E,
    },
    {
        /* A */
        0x00,
        0x08,
        0x14,
        0x22,
        0x22,
        0x3E,
        0x22,
        0x22,
    },
    {
        /* B */
        0x00,
        0x3C,
        0x22,
        0x22,
        0x3C,
        0x22,
        0x22,
        0x3C,
    },
    {
        /* C */
        0x00,
        0x1C,
        0x22,
        0x20,
        0x20,
        0x20,
        0x22,
        0x1C,
    },
    {
        /* D */
        0x00,
        0x3C,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x3C,
    },
    {
        /* E */
        0x00,
        0x3E,
        0x20,
        0x20,
        0x3C,
        0x20,
        0x20,
        0x3E,
    },
    {
        /* F */
        0x00,
        0x3E,
        0x20,
        0x20,
        0x3C,
        0x20,
        0x20,
        0x20,
    },
    {
        /* G */
        0x00,
        0x1E,
        0x20,
        0x20,
        0x20,
        0x26,
        0x22,
        0x1E,
    },
    {
        /* H */
        0x00,
        0x22,
        0x22,
        0x22,
        0x3E,
        0x22,
        0x22,
        0x22,
    },
    {
        /* I */
        0x00,
        0x3E,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
        0x3E,
    },
    {
        /* J */
        0x00,
        0x02,
        0x02,
        0x02,
        0x02,
        0x02,
        0x22,
        0x1C,
    },
    {
        /* K */
        0x00,
        0x22,
        0x24,
        0x28,
        0x30,
        0x28,
        0x24,
        0x22,
    },
    {
        /* L */
        0x00,
        0x20,
        0x20,
        0x20,
        0x20,
        0x20,
        0x20,
        0x3E,
    },
    {
        /* M */
        0x00,
        0x22,
        0x36,
        0x2A,
        0x22,
        0x22,
        0x22,
        0x22,
    },
    {
        /* N */
        0x00,
        0x22,
        0x22,
        0x32,
        0x2A,
        0x26,
        0x22,
        0x22,
    },
    {
        /* O */
        0x00,
        0x1C,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x1C,
    },
    {
        /* P */
        0x00,
        0x3C,
        0x22,
        0x22,
        0x3C,
        0x20,
        0x20,
        0x20,
    },
    {
        /* Q */
        0x00,
        0x1C,
        0x22,
        0x22,
        0x22,
        0x2A,
        0x24,
        0x1A,
    },
    {
        /* R */
        0x00,
        0x3C,
        0x22,
        0x22,
        0x3C,
        0x28,
        0x24,
        0x22,
    },
    {
        /* S */
        0x00,
        0x1C,
        0x22,
        0x20,
        0x1C,
        0x02,
        0x22,
        0x1C,
    },
    {
        /* T */
        0x00,
        0x3E,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
    },
    {
        /* U */
        0x00,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x1C,
    },
    {
        /* V */
        0x00,
        0x22,
        0x22,
        0x22,
        0x22,
        0x22,
        0x14,
        0x08,
    },
    {
        /* W */
        0x00,
        0x22,
        0x22,
        0x22,
        0x22,
        0x2A,
        0x36,
        0x22,
    },
    {
        /* X */
        0x00,
        0x22,
        0x22,
        0x14,
        0x08,
        0x14,
        0x22,
        0x22,
    },
    {
        /* Y */
        0x00,
        0x22,
        0x22,
        0x14,
        0x08,
        0x08,
        0x08,
        0x08,
    },
    {
        /* Z */
        0x00,
        0x3E,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x3E,
    },
    {
        /* [ */
        0x00,
        0x3C,
        0x30,
        0x30,
        0x30,
        0x30,
        0x30,
        0x3C,
    },
    {
        /* \ */
        0x00,
        0x00,
        0x20,
        0x10,
        0x08,
        0x04,
        0x02,
        0x01,
    },
    {
        /* ] */
        0x00,
        0x3E,
        0x06,
        0x06,
        0x06,
        0x06,
        0x06,
        0x3E,
    },
    {
        /* ^ */
        0x00,
        0x00,
        0x00,
        0x08,
        0x14,
        0x22,
        0x00,
        0x00,
    },
    {
        /* _ */
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3E,
    },
    {
        /* ' ' */
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    },
    {
        /* ! */
        0x00,
        0x08,
        0x08,
        0x08,
        0x08,
        0x08,
        0x00,
        0x08,
    },
    {
        /* " */
        0x00,
        0x14,
        0x14,
        0x14,
        0x00,
        0x00,
        0x00,
        0x00,
    },
    {
        /* # */
        0x00,
        0x14,
        0x14,
        0x3E,
        0x14,
        0x3E,
        0x14,
        0x14,
    },
    {
        /* $ */
        0x00,
        0x08,
        0x1E,
        0x28,
        0x1C,
        0x0A,
        0x3C,
        0x08,
    },
    {
        /* % */
        0x00,
        0x30,
        0x32,
        0x04,
        0x08,
        0x10,
        0x26,
        0x06,
    },
    {
        /* & */
        0x00,
        0x10,
        0x28,
        0x28,
        0x10,
        0x2A,
        0x24,
        0x1A,
    },
    {
        /* ' */
        0x00,
        0x00,
        0x08,
        0x08,
        0x08,
        0x00,
        0x00,
        0x00,
    },
    {
        /* ( */
        0x00,
        0x08,
        0x10,
        0x20,
        0x20,
        0x20,
        0x10,
        0x08,
    },
    {
        /* ) */
        0x00,
        0x08,
        0x04,
        0x02,
        0x02,
        0x02,
        0x04,
        0x08,
    },
    {
        /* * */
        0x00,
        0x08,
        0x2A,
        0x1C,
        0x08,
        0x1C,
        0x2A,
        0x08,
    },
    {
        /* + */
        0x00,
        0x00,
        0x08,
        0x08,
        0x3E,
        0x08,
        0x08,
        0x00,
    },
    {
        /* , */
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x08,
        0x10,
    },
    {
        /* - */
        0x00,
        0x00,
        0x00,
        0x00,
        0x3E,
        0x00,
        0x00,
        0x00,
    },
    {
        /* . */
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
    },
    {
        /* / */
        0x00,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x00,
        0x00,
    },
    {
        /* 0 */
        0x00,
        0x1C,
        0x22,
        0x26,
        0x2A,
        0x32,
        0x22,
        0x1C,
    },
    {
        /* 1 */
        0x00,
        0x08,
        0x18,
        0x08,
        0x08,
        0x08,
        0x08,
        0x1C,
    },
    {
        /* 2 */
        0x00,
        0x1C,
        0x22,
        0x02,
        0x0C,
        0x10,
        0x20,
        0x3E,
    },
    {
        /* 3 */
        0x00,
        0x3E,
        0x02,
        0x04,
        0x0C,
        0x02,
        0x22,
        0x1C,
    },
    {
        /* 4 */
        0x00,
        0x04,
        0x0C,
        0x14,
        0x24,
        0x3E,
        0x04,
        0x04,
    },
    {
        /* 5 */
        0x00,
        0x3E,
        0x20,
        0x3C,
        0x02,
        0x02,
        0x22,
        0x1C,
    },
    {
        /* 6 */
        0x00,
        0x0E,
        0x10,
        0x20,
        0x3C,
        0x22,
        0x22,
        0x1C,
    },
    {
        /* 7 */
        0x00,
        0x3E,
        0x02,
        0x04,
        0x08,
        0x10,
        0x10,
        0x10,
    },
    {
        /* 8 */
        0x00,
        0x1C,
        0x22,
        0x22,
        0x1C,
        0x22,
        0x22,
        0x1C,
    },
    {
        /* 9 */
        0x00,
        0x1C,
        0x22,
        0x22,
        0x1E,
        0x02,
        0x04,
        0x38,
    },
    {
        /* : */
        0x00,
        0x00,
        0x00,
        0x08,
        0x00,
        0x08,
        0x00,
        0x00,
    },
    {
        /* ; */
        0x00,
        0x00,
        0x00,
        0x08,
        0x00,
        0x08,
        0x08,
        0x10,
    },
    {
        /* < */
        0x00,
        0x04,
        0x08,
        0x10,
        0x20,
        0x10,
        0x08,
        0x04,
    },
    {
        /* = */
        0x00,
        0x00,
        0x00,
        0x3E,
        0x00,
        0x3E,
        0x00,
        0x00,
    },
    {
        /* > */
        0x00,
        0x10,
        0x08,
        0x04,
        0x02,
        0x04,
        0x08,
        0x10,
    },
    {
        /* ? */
        0x00,
        0x1C,
        0x22,
        0x04,
        0x08,
        0x08,
        0x00,
        0x08,
    }
};

/* TODO: Macro it! (64) */
SDL_Texture *charmap_textures[64] = {0};
SDL_Texture *charmap_inverse_textures[64] = {0};

void _create_surfaces(AppleScr *awin)
{
    int i;

    SDL_SetRenderDrawColor(awin->ren, 0, 255, 0, SDL_ALPHA_OPAQUE);
    for(i = 0; i < 64 /* MACRO */; i++)
    {
        uint8_t ourchar[8];
        int j;
        SDL_Surface *sur;

        /* A hack because I messed up when transcribing the font... Oops. */
        for(j = 0; j < APPLE_SCR_CHAR_HEIGHT; j++)
        {
            charmap[i][j] = charmap[i][j] << 1;
        }

        sur = SDL_CreateRGBSurfaceFrom(charmap[i], APPLE_SCR_CHAR_WIDTH, APPLE_SCR_CHAR_HEIGHT, 1, 1, 0, 0, 0, 0);
        charmap_inverse_textures[i] = SDL_CreateTextureFromSurface(awin->ren, sur);

        /* A hack because the surface is white on 0 for some reason. */
        for(j = 0; j < APPLE_SCR_CHAR_HEIGHT; j++)
        {
            ourchar[j] = ~charmap[i][j];
        }

        sur = SDL_CreateRGBSurfaceFrom(ourchar, APPLE_SCR_CHAR_WIDTH, APPLE_SCR_CHAR_HEIGHT, 1, 1, 0, 0, 0, 0);
        /* Erm... we might have a problem with creating the texture with a non-global renderer. */
        charmap_textures[i] = SDL_CreateTextureFromSurface(awin->ren, sur);
    }
}

void _awin_update_hires_line(AppleScr *awin, int y, uint8_t *lbuf)
{
    int byt, bit, x = 0, prev = 0, curr = 0, next = 0;

    for(byt = 0; byt < 40; byt++)
    {
        int data = lbuf[byt];
        int hbit = data & 0x80;

        for(bit = 0; bit < 7; bit++)
        {
            prev = curr;
            curr = next;
            next = data & 0x01;

            data >>= 1;

            if(x == 0)
            {
            }
            else
            {
                if(curr)
                {
                    if(prev || next)
                    {
                        SDL_SetRenderDrawColor(awin->ren, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
                    }
                    else if(hbit)
                    {
                        if(x & 1)
                        {
                            /* Blue */
                            SDL_SetRenderDrawColor(awin->ren, 0x00, 0x00, 0xD0, SDL_ALPHA_OPAQUE);
                        }
                        else
                        {
                            /* Red */
                            SDL_SetRenderDrawColor(awin->ren, 0xD0, 0x00, 0x00, SDL_ALPHA_OPAQUE);
                        }
                    }
                    else
                    {
                        if(x & 1)
                        {
                            /* Violet */
                            SDL_SetRenderDrawColor(awin->ren, 0x80, 0x00, 0x80, SDL_ALPHA_OPAQUE);
                        }
                        else
                        {
                            /* Green */
                            SDL_SetRenderDrawColor(awin->ren, 0x00, 0x80, 0x00, SDL_ALPHA_OPAQUE);
                        }
                    }
                    SDL_RenderDrawPoint(awin->ren, x, y);
                }
            }
            x++;
        }
    }
}

void _awin_update_hires(AppleScr *awin, uint8_t *gbuf)
{
    int scan;

    /* Translates scan to physical line on the monitor */
    int translate[] = {0, 8, 16, 24, 32, 40, 48, 56, 1, 9, 17, 25, 33, 41, 49, 57, 2, 10, 18, 26, 34, 42, 50, 58, 3, 11, 19, 27, 35, 43, 51, 59, 4, 12, 20, 28, 36, 44, 52, 60, 5, 13, 21, 29, 37, 45, 53, 61, 6, 14, 22, 30, 38, 46, 54, 62, 7, 15, 23, 31, 39, 47, 55, 63};

    SDL_SetRenderDrawColor(awin->ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(awin->ren);

    for(scan = 0; scan < 64 /* TODO: MACRO */; scan++)
    {
        _awin_update_hires_line(awin, translate[scan], gbuf + (0x80 * scan));
        _awin_update_hires_line(awin, translate[scan] + 64, gbuf + (0x80 * scan) + 0x28);
        _awin_update_hires_line(awin, translate[scan] + 128, gbuf + (0x80 * scan) + 0x50);
    }

    SDL_RenderPresent(awin->ren);
}

//void _awin_update_hires(AppleScr *awin, uint8_t *gbuf)
//{
//    int pass, mpass, byt, i, ind = 0;
//
//    SDL_SetRenderDrawColor(awin->ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
//    SDL_RenderClear(awin->ren);
//    SDL_SetRenderDrawColor(awin->ren, 255, 255, 255, SDL_ALPHA_OPAQUE);
//
//    for(mpass = 0; mpass < 8; mpass++) /* Minor pass */
//    {
//        for(pass = 0; pass < 8; pass++) /* Major pass */
//        {
//            int x = 0;
//            ind = (pass * 0x80) + (mpass * 0x400);
//            for(byt = 0; byt < 40; byt++)
//            {
//                int y = pass * 8 + mpass;
//                uint8_t b = gbuf[ind++];
//                int other_colors = b & 0x80;
//
//                for(i = 0; i < 7; i++)
//                {
//                    if(b & 1)
//                    {
//                        if(other_colors)
//                        {
//                            if(x & 1)
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0xD0, 0x00, 0x00, SDL_ALPHA_OPAQUE);
//                            }
//                            else
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0x00, 0x00, 0xD0, SDL_ALPHA_OPAQUE);
//                            }
//                        }
//                        else
//                        {
//                            if(x & 1)
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0x00, 0x80, 0x00, SDL_ALPHA_OPAQUE);
//                            }
//                            else
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0xD0, 0x00, 0xD0, SDL_ALPHA_OPAQUE);
//                            }
//                        }
//                        SDL_RenderDrawPoint(awin->ren, x, y);
//                    }
//                    b >>= 1;
//                    x++;
//                }
//            }
//            x = 0;
//            for(byt = 0; byt < 40; byt++)
//            {
//                int y = pass * 8 + mpass + 64;
//                uint8_t b = gbuf[ind++];
//                int other_colors = b & 0x80;
//
//                for(i = 0; i < 7; i++)
//                {
//                    if(b & 1)
//                    {
//                        if(other_colors)
//                        {
//                            if(x & 1)
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0xD0, 0x00, 0x00, SDL_ALPHA_OPAQUE);
//                            }
//                            else
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0x00, 0x00, 0xD0, SDL_ALPHA_OPAQUE);
//                            }
//                        }
//                        else
//                        {
//                            if(x & 1)
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0x00, 0x80, 0x00, SDL_ALPHA_OPAQUE);
//                            }
//                            else
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0xD0, 0x00, 0xD0, SDL_ALPHA_OPAQUE);
//                            }
//                        }
//                        SDL_RenderDrawPoint(awin->ren, x, y);
//                    }
//                    b >>= 1;
//                    x++;
//                }
//            }
//            x = 0;
//            for(byt = 0; byt < 40; byt++)
//            {
//                int y = pass * 8 + mpass + 128;
//                uint8_t b = gbuf[ind++];
//                int other_colors = b & 0x80;
//
//                for(i = 0; i < 7; i++)
//                {
//                    if(b & 1)
//                    {
//                        if(other_colors)
//                        {
//                            if(x & 1)
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0xD0, 0x00, 0x00, SDL_ALPHA_OPAQUE);
//                            }
//                            else
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0x00, 0x00, 0xD0, SDL_ALPHA_OPAQUE);
//                            }
//                        }
//                        else
//                        {
//                            if(x & 1)
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0x00, 0x80, 0x00, SDL_ALPHA_OPAQUE);
//                            }
//                            else
//                            {
//                                SDL_SetRenderDrawColor(awin->ren, 0xD0, 0x00, 0xD0, SDL_ALPHA_OPAQUE);
//                            }
//                        }
//                        SDL_RenderDrawPoint(awin->ren, x, y);
//                    }
//                    b >>= 1;
//                    x++;
//                }
//            }
//        }
//    }
//    SDL_RenderPresent(awin->ren);
//}

void _awin_update_text(AppleScr *awin, uint8_t *tbuf)
{
    int passx, pass, ind = 0;

    SDL_SetRenderDrawColor(awin->ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(awin->ren);

    for(pass = 0; pass < 8; pass += 1)
    {
        int x = 0, y = pass;
        ind = (0x80 * pass);
        for(passx = 0; passx < APPLE_SCR_TEXT_WIDTH * 3; passx += 1)
        {
           // printf("Printing index 0x%02X at (%d, %d)\n", ind, x, y);
            SDL_Texture *char_tex;
            uint8_t char_code = tbuf[ind++];

            SDL_Rect target;
            target.x = x * APPLE_SCR_CHAR_WIDTH;
            target.y = y * APPLE_SCR_CHAR_HEIGHT;
            target.w = APPLE_SCR_CHAR_WIDTH;
            target.h = APPLE_SCR_CHAR_HEIGHT;

            if(char_code >> 6 == 0) /* INVERSE -- if 0x00 <= charcode < 0x40 */
            {
                char_tex = charmap_inverse_textures[char_code % 64];
            }
            else if(char_code >> 6 == 1) /* FLASHING -- if 0x40 <= charcode < 0x80 */
            {
                if((SDL_GetTicks() >> 9) % 2)
                {
                    char_tex = charmap_inverse_textures[char_code % 64];
                }
                else
                {
                    char_tex = charmap_textures[char_code % 64];
                }
            }
            else /* Normal */
            {
                char_tex = charmap_textures[char_code % 64];
            }

            SDL_RenderCopy(awin->ren, char_tex, NULL, &target);

            x++;
            if(x == 40)
            {
                x = 0;
                y += 8;
            }

//            memcpy(chr, charmap[tbuf[ind++] % 64], sizeof(uint8_t) * 8);
//
//            /* TODO: Maybe unroll a bit? */
//            /* TODO: Remove magic numbers */
//            for(row = 0; row < 8; row++)
//            {
//                for(col = 0; col < 7; col++) /* col 0 == 0x01 */
//                {
//                    if((chr[row] >> col) & 0x01)
//                    {
//                        SDL_RenderDrawPoint(awin->ren, ch_x + (7 - col - 1), ch_y + row);
//                    }
//                }
//            }
        }
    }

    SDL_RenderPresent(awin->ren);
}

int apple_scr_init(AppleScr *awin, uint8_t *text_buffer, uint8_t *graphics_buffer)
{
    /* TODO: What if we already initialized this? */
    /* Won't happen, though... */
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        printf("1\n");
        return 1;
    }

    /* TODO: Maybe multiply size by some constant? */
    SDL_Window *win = SDL_CreateWindow("Apple ][", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, APPLE_SCR_WIDTH, APPLE_SCR_HEIGHT, SDL_WINDOW_SHOWN);
    if(win == NULL)
    {
        printf("3\n");
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(ren == NULL)
    {
        printf("5\n");
        return 1;
    }

    awin->win = win;
    awin->ren = ren;
    awin->display_flags = 0;
    awin->text_buffer = text_buffer;
    awin->graphics_buffer = graphics_buffer;
    _create_surfaces(awin);
    apple_scr_update(awin);

    return 0;
}

void apple_scr_update(AppleScr *awin)
{
}

void apple_scr_redraw(AppleScr *awin)
{
    if(awin->display_flags & APPLE_SCR_MODE_GRAPHICS)
    {
        if(awin->display_flags & APPLE_SCR_MODE_HIGH_RES)
        {
            if(awin->display_flags & APPLE_SCR_MODE_SECONDARY)
            {
                _awin_update_hires(awin, awin->graphics_buffer + 0x2000);
            }
            else /* Primary screen */
            {
                _awin_update_hires(awin, awin->graphics_buffer);
            }
        }
        else /* Low res */
        {
            if(awin->display_flags & APPLE_SCR_MODE_SECONDARY)
            {
                //_awin_update_lowres(awin, awin->graphics_buffer + 0x400);
            }
            else /* Primary screen */
            {
                //_awin_update_lowres(awin, awin->graphics_buffer);
            }
        }
    }
    else /* Text mode */
    {
        if(awin->display_flags & APPLE_SCR_MODE_SECONDARY)
        {
            _awin_update_text(awin, awin->text_buffer + 0x400);
        }
        else /* Primary screen */
        {
            _awin_update_text(awin, awin->text_buffer);
        }
    }
}

//int pwin_init(struct pixel_window *pwin, int w, int h, char *buf)
//{
//    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) /* I am ashamed of this line of code. */
//    {
//        return 1;
//    }
//
//    SDL_Window *win = SDL_CreateWindow("Pixel Window", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
//    if(win == NULL)
//    {
//        return 2;
//    }
//
//    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//    if(ren == NULL)
//    {
//        return 3;
//    }
//
//    pwin->win = win;
//    pwin->ren = ren;
//    pwin->buf = buf;
//    pwin->w = w;
//    pwin->h = h;
//    pwin->x = 0;
//    pwin->y = 0;
//
//    SDL_AddEventWatch(&_pwin_event_callback, pwin);
//
//    _pwin_draw_image(pwin);
//    return 0;
//}
//
//int _pwin_event_callback(void *void_pwin, SDL_Event *e)
//{
//    struct pixel_window *pwin = (struct pixel_window *) void_pwin;
//    char *last_char, temp;
//
//    if(e->type == SDL_KEYDOWN)
//    {
//        last_char = &pwin->buf[(pwin->y * pwin->w) + pwin->x];
//        switch(e->key.keysym.sym)
//        {
//            case SDLK_UP:
//                pwin->y--;
//                if(pwin->y < 0) pwin->y += pwin->h;
//                break;
//            case SDLK_DOWN:
//                pwin->y++;
//                pwin->y %= pwin->h;
//                break;
//            case SDLK_LEFT:
//                pwin->x--;
//                if(pwin->x < 0) pwin->x += pwin->w;
//                break;
//            case SDLK_RIGHT:
//                pwin->x++;
//                pwin->x %= pwin->w;
//                break;
//            case SDLK_ESCAPE:
//                pwin_close(pwin);
//                return 0;
//        }
//
//        temp = pwin->buf[(pwin->y * pwin->w) + pwin->x];
//        pwin->buf[(pwin->y * pwin->w) + pwin->x] = *last_char;
//        *last_char = temp;
//
//
//        _pwin_draw_image(pwin);
//    }
//}
//
//void _pwin_draw_image(struct pixel_window *pwin)
//{
//    int s_width, s_height, x, y;
//
//    SDL_GetWindowSize(pwin->win, &s_width, &s_height);
//    SDL_RenderClear(pwin->ren);
//
//    SDL_Rect pixel = {0, 0, s_width / pwin->w, s_height / pwin->h};
//
//    for(y = 0; y < pwin->h; y++)
//    {
//        for(x = 0; x < pwin->w; x++)
//        {
//            SDL_SetRenderDrawColor(pwin->ren, GET_RED(pwin->buf[(y * pwin->w) + x]),
//                                           GET_GREEN(pwin->buf[(y * pwin->w) + x]),
//                                           GET_BLUE(pwin->buf[(y * pwin->w) + x]),
//                                           0xFF);
//            SDL_RenderFillRect(pwin->ren, &pixel);
//            pixel.x += s_width / pwin->w;
//        }
//        pixel.x = 0;
//        pixel.y += s_height / pwin->h;
//    }
//
//    SDL_RenderPresent(pwin->ren);
//}
//
//void pwin_wait(int len)
//{
//    SDL_Delay(len);
//}
//
//void pwin_close(struct pixel_window *pwin)
//{
//    SDL_DestroyRenderer(pwin->ren);
//    SDL_DestroyWindow(pwin->win);
//    SDL_Quit();
//}
