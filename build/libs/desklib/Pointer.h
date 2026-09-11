/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pointer.h
    Author:  Copyright © 1994 Lee Atkinson
    Version: 1.01 (17 Apr 2005)
    Purpose: high level pointer handling routines
*/

/* Abstract */
/*
  This header declares various routines for pointer handling, which at the
  moment consists of routines to restrict the pointer to certain areas of
  the screen, and removing that restriction.
*/

#ifndef __dl_pointer_h
#define __dl_pointer_h

#include "DeskLib:Core.h"
#include "DeskLib:Icon.h"
#include "DeskLib:Window.h"


#ifdef __cplusplus
extern "C" {
#endif


extern os_error *Pointer_RestrictToWindow(window_handle window);
/*
  This restricts the pointer to the rectangle which is the window specified
  in 'window'.  It returns NULL, or a pointer to any error which occurs.
*/


extern os_error *Pointer_RestrictToIcon(window_handle win,icon_handle icon);
/*
  This restricts the pointer to the rectangle which is the bounding box
  of the icon 'icon' in the window 'win'.  It returns NULL, or a pointer
  to any error which occurs.
*/


extern os_error *Pointer_RestrictToRect(wimp_rect rect);
/*
  This restricts the pointer to the screen rectangle which is given in
  'rect'.  It returns NULL, or a pointer to any error which occurs.
*/


extern os_error *Pointer_Unrestrict(void);
/*
  This removes any restriction on the pointer.
*/


#ifdef __cplusplus
}
#endif


#endif
