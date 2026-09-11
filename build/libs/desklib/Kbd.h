/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Kbd.h
    Author:  Copyright © 1993 Jason Williams
    Version: 1.02 (17 Apr 2005)
    Purpose: Reading from the keyboard

    Mods:    24-07-93  JW  Added Kbd_GET
*/


#ifndef __dl_kbd_h
#define __dl_kbd_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This defines routines for relatively low-level reading from the keyboard.
*/


extern BOOL Kbd_KeyDown(int keynum);
/*
  This checks to see if the given key is currently being pressed.
  'keynum' is a negative INKEY number.

  The only values you should ever have to use with this in the Desktop
  are defined by the kbd_neginkey enum.
*/


typedef enum
{
  inkey_ADJUST       = -12,                       /* MOUSE 'Adjust' button   */
  inkey_MENU         = -11,                       /* MOUSE 'Menu' button     */
  inkey_SELECT       = -10,                       /* MOUSE 'Select' button   */

  inkey_RALT         = -9,                        /* Right ALT key           */
  inkey_LALT         = -6,                        /* Left ALT key            */
  inkey_ALT          = -3,                        /* Either/Both ALT keys    */

  inkey_RCTRL        = -8,                        /* Right CTRL key          */
  inkey_LCTRL        = -5,                        /* Left CTRL key           */
  inkey_CTRL         = -2,                        /* Either/Both CTRL keys   */

  inkey_RSHIFT       = -7,                        /* Right SHIFT key         */
  inkey_LSHIFT       = -4,                        /* Left SHIFT key          */
  inkey_SHIFT        = -1                         /* Either/Both SHIFT keys  */
} kbd_neginkey;
/*
  These are the only keys you should actually have to test for in the
  Desktop environment.  All other keys you are interested in should come
  in via Wimp events.

  The names are self-explanatory, with the first three corresponding to
  mouse buttons and the rest giving right-only, left-only or either left
  or right tests.

  If you need other values, the negative inkey code corresponds to the
  -1-(internal key code).  The key codes can be found in the PRMs or in
  the "OS" Stronghelp manual at the time of writing.
*/


extern char Kbd_GET(void);
/*
  This is an OS_ReadC veneer.  It returns the ASCII code of the next key
  pressed (which may already be in the keyboard buffer).  This is
  effectively a C version of BASIC's GET command, hence the name.
*/


typedef struct
{
  unsigned alt         : 1;
  unsigned ctrl        : 1;
  unsigned shift       : 1;
  unsigned left_alt    : 1;
  unsigned left_ctrl   : 1;
  unsigned left_shift  : 1;
  unsigned right_alt   : 1;
  unsigned right_ctrl  : 1;
  unsigned right_shift : 1;

} kbd_modifiers;
/*
  For each modifier key, when the flag is TRUE it means that the key
  is held down.
*/


extern kbd_modifiers Kbd_GetModifiers(BOOL detailed);
/*
  This returns the current state of the modifier keys (Alt, Shift and Ctrl).
  If you set detailed to TRUE it checks and fills in the state of the
  left and right keys as well.
*/


#ifdef __cplusplus
}
#endif


#endif
