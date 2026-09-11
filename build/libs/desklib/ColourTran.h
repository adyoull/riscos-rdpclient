/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    ColourTran.h
    Author:  Copyright © 1993 Shaun Blackmore
    Version: 1.01 (17 Apr 2005)
    Purpose: Function veneer for ColourTrans SWIs
*/



#ifndef __dl_colourtran_h
#define __dl_colourtran_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#include "DeskLib:Font.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header defines several functions which are veneers for some of
  the SWIs provided by the ColourTrans module.
*/

extern os_error *ColourTrans_SetGCOL(int palette, int flag, int gcol);
/*
  This works out the closest available GCOL to a given palette entry,
  and sets it.

  The palette entry (passed in 'palette') is in the form 0xBBGGRR00,
  where B is the blue component, G green, and R red.  The flags you
  are allowed to set are bit 7 to set the background colour, and bit
  8 to use dithering.  'gcol' is the GCOL action to set.
*/

extern os_error *ColourTrans_InvalidateCache(void);
/*
  This function informs the ColourTrans module that the palette has
  changed since the last ColourTrans call.  Colour changes due to
  mode changes are automatically dealt with.
*/

extern os_error *ColourTrans_SelectTable(int src_mode, int *src_pal,
                                         int dst_mode, int *dst_pal,
                                         char *table);
/*
  This function sets up a translation table in a buffer for the
  given source and destination modes and palettes.

  Mode and palette parameters may be -1 to use the current values.
*/

typedef struct
{
  int font;
  int back;
  int fore;
  int max;
} colourtrans_fontblock;
/*
  Holds the information needed for the ColourTrans_ReturnFontColours
  function, namely the font handle, background colour, foreground
  colour and the maximum colour offset.
*/

extern os_error *ColourTrans_ReturnFontColours(colourtrans_fontblock *fontcols);
/*
  This function finds the maximum range of colours which can be
  sensibly used for anti-aliasing a font.  For the given palette
  entries, it finds the best fit in the current palette and then
  determines the maximum amount of anti-aliasing up to the limit
  passed in fontcols->max.

  If anti-aliasing is desired set fontcols->max to 14, otherwise set
  it to 0.  fontcols->font should be set to the font handle, or the
  current font.
*/


extern int ColourTrans_ReturnColourNumberForMode(int palette, int mode,
                                                 int *pal);
/*
  This function returns the closest possible colour number for the
  given palette entry 'palette', mode 'mode' and palette 'pal'.

  'mode' may be -1 for the current mode and 'pal' may be -1 for the
  current palette or 0 for the default palette.

  It returns -1 to indicate an error.
*/


extern os_error *ColourTrans_SetFontColours(font_handle font,
                                            unsigned int background,
                                            unsigned int foreground,
                                            int offset);

#ifdef __cplusplus
}
#endif


#endif
