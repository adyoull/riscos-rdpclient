/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    GFX.gfx.h
    Author:  Copyright © 1992, 1993 Edouard Poor, Jason Williams, Jason Howat
    Version: 0.72 (17 Apr 2005)
    Purpose: Graphics manipulation commands
    Mods:    02 Sep 1995 JH Added GFX_Mode, GFX_SetOrigin.
*/


#ifndef __dl_gfx_h
#define __dl_gfx_h

#ifndef __dl_core_h
#include "Desklib:Core.h"
#endif

#ifndef __dl_wimp_h
#include "Desklib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  A set of fairly low-level graphics manipulation routines and related macros.

  For full details of the plot codes and the subtle differences between them
  you are best off looking for documentation for OS_Plot in the PRMs,
  Stronghelp manuals, or whatever reference you normally use.
*/


/* Plot codes to be used with the GFX_Plot function. */

#define  plot_SOLIDBOTH       0x00
#define  plot_SOLIDEXFINAL    0x08
#define  plot_DOTTEDBOTH      0x10
#define  plot_DOTTEDEXFINAL   0x18
#define  plot_SOLIDEXINIT     0x20
#define  plot_SOLIDEXBOTH     0x28
#define  plot_DOTTEDEXINIT    0x30
#define  plot_DOTTEDEXBOTH    0x38
#define  plot_POINT           0x40
#define  plot_HHORZLINEFILLNB 0x48
#define  plot_TRIANGLEFILL    0x50
#define  plot_HORIZLINEFILLB  0x58
#define  plot_RECTANGLEFILL   0x60
#define  plot_HORIZLINEFILLF  0x68
#define  plot_PARALLELFILL    0x70
#define  plot_HORIZLINEFILLNF 0x78
#define  plot_FLOODTOBACK     0x80
#define  plot_FLOODTOFORE     0x88
#define  plot_CIRCLE          0x90
#define  plot_CIRCLEFILL      0x98
#define  plot_CIRCLEARC       0xA0
#define  plot_SEGMENT         0xA8
#define  plot_SECTOR          0xB0
#define  plot_BLOCK           0xB8
#define  plot_ELLIPSE         0xC0
#define  plot_ELLIPSEFILL     0xC8
#define  plot_GRAPHICSCHAR    0xD0
#define  plot_SPRITE          0xE8

/* Within each block of eight the offset from the base number means: */
#define  plot_MOVECURSORREL   0
#define  plot_DRAWRELFORE     1
#define  plot_DRAWRELINVERSE  2
#define  plot_DRAWRELBACK     3
#define  plot_MOVECURSORABS   4
#define  plot_DRAWABSFORE     5
#define  plot_DRAWABSINVERSE  6
#define  plot_DRAWABSBACK     7


/* The above applies except for plot_BLOCK where the codes are as follows: */
#define  plot_BMOVEREL        0
#define  plot_BMOVERECTREL    1
#define  plot_BCOPYRECTREL    2

#define  plot_BMOVEABS        4
#define  plot_BMOVERECTABS    5
#define  plot_BCOPYRECTABS    6


extern void GFX_Plot(int plotcode, int x, int y);
/*
  An assembler veneer for OS_Plot.

  'x' and 'y' are the screen coordinates to use. 'plotcode' is one of the
  many macros, like plot_SPRITE, plot_POINT and similar, bitwise ORed with
  a modifier to define the exact behaviour.

  Modifiers for everything but plot_BLOCK:

  plot_MOVECURSORREL, plot_DRAWRELFORE, plot_DRAWRELINVERSE, plot_DRAWRELBACK,
  plot_MOVECURSORABS, plot_DRAWABSFORE, plot_DRAWABSINVERSE
  and plot_DRAWABSBACK

  Modifiers for plot_BLOCK:

  plot_BMOVEREL, plot_BMOVERECTREL, plot_BCOPYRECTREL, plot_BMOVEABS,
  plot_BMOVERECTABS and plot_BCOPYRECTABS
*/



/*
  GFX_ miscellaneous drawing macros
  ---------------------------------
  Below are some macros using GFX_Plot to make life easier for you.
  the "By" form (MoveBy, etc) moves/draws relative to the current cursor
  position, while the other form draws in absolute OS coords.

  GFX_Move(X, Y)  Moves the graphics cursor to the OS co-ordinates X, Y
  GFX_Plot(X, Y)  Plots a pixel at the specified position
  GFX_Draw(X, Y)  Draws a line from the current cursor position to x,y
*/

#define GFX_Move(x, y) GFX_Plot(plot_SOLIDBOTH + plot_MOVECURSORABS, x, y)
/*
  MACRO: void GFX_Move(int x, int y);

  This moves the graphics cursor to the OS coordinates (x, y).
*/


#define GFX_MoveBy(x, y) GFX_Plot(plot_SOLIDBOTH + plot_MOVECURSORREL, x, y)
/*
  MACRO: void GFX_MoveBy(int x, int y);

  This moves the graphics cursor by (x, y) OS units relative to its
  current position.
*/


#define GFX_PlotPoint(x, y) GFX_Plot(plot_POINT + plot_DRAWABSFORE, x, y)
/*
  MACRO: void GFX_PlotPoint(int x, int y);

  This plots a pixel at the OS coordinates (x, y).
*/


#define GFX_PlotPointBy(x, y) GFX_Plot(plot_POINT + plot_DRAWRELFORE, x, y)
/*
  MACRO: void GFX_PlotPointBy(int x, int y);

  This plots a pixel at the point (x, y) OS units from the current
  graphics cursor position.
*/


#define GFX_Draw(x, y) GFX_Plot(plot_SOLIDBOTH + plot_DRAWABSFORE, x, y)
/*
  MACRO: void GFX_Draw(int x, int y);

  This draws a line from the current graphics position to the
  OS coordinates (x, y).
*/


#define GFX_DrawBy(x, y) GFX_Plot(plot_SOLIDBOTH + plot_DRAWRELFORE, x, y)
/*
  MACRO: void GFX_DrawBy(int x, int y);

  This draws a line from the current graphics position to a point
  (x, y) OS units away.
*/


extern void GFX_Rectangle(int x, int y, int w, int h);
/*
  This draws a rectangle of width w and height h with the bottom-left
  hand corner at (x, y).

  All values are in OS units.
*/


#define GFX_RectangleFill(x, y, w, h)                       \
  {                                                         \
    GFX_Move(x, y);                                         \
    GFX_Plot(plot_RECTANGLEFILL + plot_DRAWRELFORE, w, h);  \
  }
/*
  MACRO: void GFX_RectangleFill(int x, int y, int w, int h);

  This draws a filled rectangle of width w and height h
  with the bottom-left hand corner at (x, y).

  All values are in OS units.
*/


#define GFX_Circle(x, y, r)                                 \
  {                                                         \
    GFX_Move(x, y);                                         \
    GFX_Plot(plot_CIRCLE + plot_DRAWRELFORE, (r), 0);       \
  }
/*
  MACRO: void GFX_Circle(int x, int y, int r);

  This draws a circle outline of radius r, with its centre at (x, y).

  All values are in OS units.
*/


#define GFX_CircleFill(x, y, r)                             \
  {                                                         \
    GFX_Move(x, y);                                         \
    GFX_Plot(plot_CIRCLEFILL + plot_DRAWRELFORE, (r), 0);   \
  }
/*
  MACRO: void GFX_Circle(int x, int y, int r);

  This draws a filled circle of radius r, with its centre at (x, y).

  All values are in OS units.
*/


extern void GFX_CLG(void);
/*
  This clears the current graphics window to the current background colour.
*/


extern void GFX_VDU(char ch);
/*
  This is a veneer to OS_WriteC.  It writes the given ASCII character to
  the VDU stream.
*/


#define VDU(C) GFX_VDU(C)
/*
  MACRO: void VDU(char ch);

  This is an alias for GFX_VDU, which is a veneer to OS_WriteC.
  It writes the given ASCII character to the VDU stream.
*/


#define GFX_GCOL(A,C) {GFX_VDU(18); GFX_VDU(A); GFX_VDU(C);}
/*
  MACRO: void GFX_GCOL(int action, int colour);

  This sets the current graphics colour and action through a VDU sequence.
*/


#define GFX_Mode(M) {GFX_VDU(22); GFX_VDU(M);}
/*
  MACRO: void GFX_Mode(int mode);

  Sets the screen mode to the given mode number.

  Don't do this in the desktop.
*/


#define GFX_SetOrigin(x,y) {short a=(x),b=(y); GFX_VDU(29); \
                            GFX_VDU(a & 0xff); GFX_VDU((a>>8) & 0xff); \
                            GFX_VDU(b & 0xff); GFX_VDU((b>>8) & 0xff);}
/*
  MACRO: void GFX_SetOrigin(short x, short y);

  This sets the graphics origin to (x, y) in OS coordinates.
*/


extern void GFX_Write0(const char *string);
/*
  This is a veneer for OS_Write0, which writes the given zero-terminated
  string to the VDU stream.
*/


extern void GFX_WriteN(const char *string, int numchars);
/*
  This is a veneer for OS_WriteN which writes 'numchars' characters from
  the given string to the VDU stream.
*/


extern void GFX_Wait(void);
/*
  This call does not return until the vertical refresh has finished.

  This means that you get control back after the hardware has
  finshed updating the screen so if you do any drawing immediately
  after this call it will be done while the screen is not being
  drawn. This helps reduce flicker.

  Calling this procedure in Wimp applications should be used sparingly
  as overuse will slow down the machine to an unacceptable level.  Generally
  only use by experimentation - write your graphics code, if it flickers too
  much try putting in a GFX_Wait() and see if it helps. If it does and the
  update speed isn't adversly affected then leave it in.
*/


extern BOOL GFX_ReadPoint(int x, int y, int *colour, int *tint);
/*
  This reads the colour and tint at the screen co-ordinate (x,y).
  If the point is not on the screen it returns FALSE, otherwise
  TRUE.

  Note that 'colour' and 'tint' may be changed even if this function
  returns FALSE.
*/


extern void GFX_GetClip(wimp_rect *rect);
/*
  Get the current graphics clip rectangle
*/


extern void GFX_SetClip(wimp_rect *rect);
/*
  Set the current graphics clip rectangle
*/


extern void GFX_SetColour(unsigned int flags, unsigned int colour);
/*
  Set foreground or background colour.  This is an interface to OS_SetColour
*/


#ifdef __cplusplus
}
#endif


#endif
