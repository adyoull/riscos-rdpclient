/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    DragASpr.h
    Author:  Copyright © 1992 Shaun Blackmore and Jason Williams
    Version: 1.02 (17 Apr 2005)
    Purpose: Function veneers for DragASpr SWIs
*/

#ifndef __dl_dragaspr_h
#define __dl_dragaspr_h

#ifndef __dl_wimp_h
#include "DeskLib:wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header defines some veneers to the DragASprite SWIs.
*/


extern os_error *DragASprite_Start(int flags, void *spritearea, const char *name,
                                   wimp_rect *bound, wimp_rect *parent);
/*
  This function calls the DragASprite_Start SWI with the arguments places in
  the appropriate registers.  This should cause a sprite drag to start.

  spritearea is a pointer to the sprite area holding the sprite you want to
  drag, or 1 for the Wimp sprite pool.  name is the name of the sprite.

  bbox is the bounding box of the sprite, and parent is the bounding box of the
  drag (the mouse pointer will be constrained to this box during the drag) if
  the appropriate flag is set.

  See the PRMs for full details of the flags.  Bits 4 & 5 of the flags decide
  what defines the bounding box:  00 means the whole-screen, 01 means the
  display area of the window the pointer is over, and 10 means it is user
  specified (by passing a parent rectangle).  Set bit 7 to use a drop-shadow
  and bit 8 to make the drag solid rather than transparent on RISC OS 3.5
  and later.
*/


extern os_error *DragASprite_Stop(void);
/*
  This terminates any sprite drag begun with DragASprite_Start.
*/


extern os_error *DragASprite_DragIcon(window_handle window, icon_handle icon);
/*
  This is a simple interface for DragASpr. Simply call it when you want to
  start a drag operation of a sprite icon, and it starts a DragASprite
  drag if possible, or a normal Wimp_DragBox if not.
*/

#ifdef __cplusplus
}
#endif


#endif
