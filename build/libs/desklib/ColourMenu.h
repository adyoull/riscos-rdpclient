/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    ColourMenu.ColourMenu.c
    Author:  Copyright © 1994 Craig Beech
    Version: 1.01 (17 Apr 2005)
    Purpose: Equivalent of the RISC_OSLib colourmenu function.
*/

#ifndef __dl_colourmenu_h
#define __dl_colourmenu_h

#ifndef __dl_menu_h
#include "DeskLib:Menu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header declares a single function to create a simple menu of
  the sixteen Wimp colours.
 */

extern menu_ptr ColourMenu_Create(const char *name);
/*
  This creates a menu containing the sixteen Wimp colours, with the title
  for the menu given by 'name'.  It returns a pointer to the new menu.
*/

#ifdef __cplusplus
}
#endif


#endif
