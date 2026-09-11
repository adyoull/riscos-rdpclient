/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Font2.Font2.c
    Author:  Copyright © 1994 Julian Smith
    Version: 1.01 (17 Apr 2005)
    Purpose: Fonts with automatic handling of mode-changing
*/


/*
The RISC OS font manager seems to work in terms of pixels rather than OS
coordinates, and so it doesn't adjust fonts when the user changes to a
screen mode with a different pixel-OS ratio. This results in fonts
changing size over mode changes, unless they are re-calculated.

This causes all sorts of complexity in DeskLib's Template and Window
libraries, because icons with outline fonts contain a font handle in the
icon data, so Window_ModeChange (in Window.c.ModeChange) has to go
through all loaded templates and windows looking for outline fonts, lose
them with Font_LoseFont, recreate them with Font_FindFont and substitute
the new font_handle values into the icon blocks.

For other uses of fonts, a simpler method is to make applications work
with a *pointer* to a font handle rather than a straight font handle,
and have a central agency take care of the actual font handles.

This is what Font2 does. The Font2 library can thus easily keep the list
of font handles (which the application gets pointers into) and change
the handles on mode changes without having to store the location of
every font handle the application uses, and change them when the mode is
changed. Hopefully this should make outline fonts much easier to use.

Note that if the application has many (font2_block *)'s, some will point
to the *same* place if they are the same font. Font2 keeps a count of
how many pointer there are so it knows when a font2_block is finished
with.

Font2 also registers an exit-handler which calls Font_LoseFont for all
fonts registered with Font2 when the application finished (the
font-handling in Window/Template has always done this).

All calls to Font2 functions require the address of the (font2_block *),
ie. a (font2_block **). This is because some of the functions set the
pointer to NULL or a new value.



Use like:

font2_block	*font2;

main()
{
Font2_ClaimFont( &font2, "Trinity.Medium", 16*12, 16*12);
}

Then in a redrawer:

Font_Paint3(
	font2->handle,	|* Always correct for the current mode	*|
	"Some text...",
	font_plot_OSCOORS | font_plot_CURRENTHANDLE,
	x,
	y - font2->info.miny,
	NULL, NULL, 0
	);

Note the use of font2->info. I expect most programs will need to know
about the font_info for all fonts they use, so Font2 provides this
information automatically to a) save calls to Font_ReadInfo or b) save
memory by storing each font_handle's information just once, regardless
of how many times the applcation is using the font.

*/


#ifndef __dl_font2_h
#define __dl_font2_h

#ifndef __dl_font_h
#include "DeskLib:Font.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  The Font Manager doesn't automatically update fonts when you change
  between modes with different pixel to OS unit ratios.  This can lead
  to font handles referring to fonts which do not appear the same size
  as those you claimed.

  The functions declared by this header allow you to claim a font and
  have it automatically updated when the mode changes, so that you always
  have a handle for the font at the size you wanted.

  It works by registering for user messages with Event_Claim and detecting
  a mode change, then updating the handles in its list. Make sure you call
  Event_Initialise (or equivalent) before using this library.

  Font2 also frees all font handles you have claimed through it
  when your application exits, by registering an atexit() handler.
*/


typedef struct font2_block
{
  font_handle handle;  /* Use for all font actions  */
  font_info info;      /* Always up-to-date info    */
} font2_block;
/*
  This is the form in which your program can access always up-to-date
  font handles and font information through Font2.

  The font_info block is supplied as you will usually want the know the
  bounding box at some point, and this cuts down on calls to Font_ReadInfo
  and means it only has to be stored once for all uses of the same font
  in the app.
*/


os_error *Font2_ClaimFont(font2_block **font2, const char *name,
                           int sizex, int sizey);
/*
  'name' should be the name of the font you wish to use (eg. "Trinity.Medium").
  The font size is specified in 1/16ths of a point (eg. 12*16 for 12pt).

  (*font2) will be filled with a pointer to a font2_block containing
  the information about the given font.

  By always referring to the handle and information in this block you will
  always have up-to-date data.

  This function returns NULL if there is no error.
*/


os_error *Font2_ReclaimFont(font2_block **font2, const char *name,
                            int xsize, int ysize);
/*
  This releases a claim on an previously claimed font if *font2 is not NULL,
  and then updates *font2 with a claim on the font with name 'name' and the
  specified size in 1/16ths of a point.
*/


void Font2_ReleaseFont(font2_block **font2);
/*
  This releases a claim on the font claimed with Font2_Claim (if *font2
  is not NULL) and then sets *font2 to NULL.
*/


void Font2_Initialise(void);
/*
  This sets up the Font2 system and registers a claim with Event_Claim
  so that it can detect mode changes.

  It is called automatically as necessary by Font2, so you never have to
  call this yourself.
*/


#ifdef __cplusplus
}
#endif


#endif
