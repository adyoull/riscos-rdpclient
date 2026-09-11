/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Tinct.h
    Author:  Copyright © 2005 Peter Naulls
    Version: 1.00 (27 May 2005)
    Purpose: Veneers the Tinct module.
             See http://www.tinct.net/
*/

#ifndef __dl_tinct_h
#define __dl_tinct_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#include "DeskLib:Sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TINCT_SPRITETYPE 0x301680B5


typedef union
{
  struct
  {
      unsigned int read_screenbase : 1;
      unsigned int bilinear        : 1;
      unsigned int dither          : 1;
      unsigned int diffusion       : 1;
      unsigned int horz_fill       : 1;
      unsigned int vert_fill       : 1;
      unsigned int read_palette    : 1;
      unsigned int use_osspriteop  : 1;
      unsigned int background      : 24;
  } flags;
  unsigned int val;
} tinct_flags;


typedef struct
{
    unsigned int next;
    char name[12];
    unsigned int width;
    unsigned int height;
    unsigned int firstbit;
    unsigned int lastbit;
    unsigned int offset1;
    unsigned int offset2;
    unsigned int type;
} tinct_sprite;


typedef struct
{
    unsigned int width;
    unsigned int height;
    char name[12];
    unsigned int flags;
    unsigned int size;
    unsigned char data[0];
} tinct_compressed;


extern os_error *Tinct_PlotAlpha(tinct_sprite *sprite, int x, int y,
                                 tinct_flags flags);


extern os_error *Tinct_PlotScaledAlpha(tinct_sprite *sprite, int x, int y,
                                       unsigned int width, unsigned int height,
                                       tinct_flags flags);

extern os_error *Tinct_Plot(tinct_sprite *sprite, int x, int y,
                            tinct_flags flags);


extern os_error *Tinct_PlotScaled(tinct_sprite *sprite, int x, int y,
                                  unsigned int width, unsigned int height,
                                  tinct_flags flags);

extern os_error *Tinct_ConvertSprite(sprite_info *source, tinct_sprite *dest);

unsigned int Tinct_AvailableFeatures(unsigned int feature_test);

extern os_error *Tinct_Compress(tinct_sprite *sprite, unsigned char *output,
                                unsigned int available, unsigned int flag,
                                unsigned int *size);

extern os_error *Tinct_Decompress(unsigned char *input, unsigned char *output,
                                  unsigned int flag, unsigned int *size);



#ifdef __cplusplus
}
#endif

#endif
