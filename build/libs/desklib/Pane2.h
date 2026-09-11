/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.h
    Author:  Copyright © 1995 Andrew Sellors.
    Version: 1.05 (17 Apr 2005)
    Purpose: Handles windows with panes.
    Mods:    1.02 -> 1.04
             Added pane2_MAINRIGHT, Pane2_GetMainHandle, Pane2_GetPaneNumber,
             Pane2_OpenWindow, Pane2_ReadVars, Pane2_SetVars.
*/


/* Abstract */
/*
  This aims to be a more flexible pane-handling library than that supplied
  by Pane, allowing multiple panes attached to one window, and allowing more
  options as to how the positions and sizes of the panes are calculated
  relative to the main window.
*/


#ifndef __dl_pane2_h
#define __dl_pane2_h

#ifndef __dl_wimpswis_h
#include "DeskLib:WimpSWIs.h"
#endif

#ifndef __dl_window_h
#include "DeskLib:Window.h"
#endif

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef union
{
  int value;
  struct
  {
    unsigned int panetop   :1; /* pane is linked using the top of the pane
                                  window, otherwise the bottom is used */
    unsigned int maintop   :1; /* pane is fixed to top of main else
                                  fixed to bottom */
    unsigned int fixed     :1; /* pane cannot be moved relative to
                                  main window */
    unsigned int hresize   :1; /* pane is resized to match width of
                                  main window up to pane width */
    unsigned int hstretch  :1; /* pane width is ignored when resizing */
    unsigned int vresize   :1; /* pane is resized to match height of
                                  main window up to pane height */
    unsigned int vstretch  :1; /* pane height is ignored when resizing */
    unsigned int hscroll   :1; /* pane scrolls horizontally with
                                  the main window */
    unsigned int vscroll   :1; /* pane scrolls vertically with the
                                  main window */
    unsigned int mainright :1; /* pane is fixed to right of main
                                  instead of left */
    unsigned int dummy     :22;
  } data;
} pane2_flags;

#define pane2_PANETOP   0x0001
/*
  This means that the pane is "attached" to the main window using the top
  left of the pane window rather than the bottom left corner, which is the
  default.
*/

#define pane2_MAINTOP   0x0002
/*
  This means that the pane moves relative to the top of the main window
  rather than the bottom corner, which is the default.
*/

#define pane2_FIXED     0x0004
/*
  This forces the offset between the pane and the main windows to be constant,
  the pane is not allowed to move relative.  With it off you get ArtWorks
  style pane windows.
*/

#define pane2_HRESIZE   0x0008
/*
  This makes the right hand side of the pane be clipped the right hand
  sie of the main window if the main window is smaller than the pane.
*/

#define pane2_HSTRETCH  0x0010
/*
  When used in conjunction with pane2_HRESIZE this makes the right hand
  side of the pane always follow the right hand side of the main window.
*/

#define pane2_VRESIZE   0x0020
/*
  This makes the pane be clipped to the height of the main window if
  the main window is smaller than the pane.
*/

#define pane2_VSTRETCH  0x0040
/*
  When used in conjunction with pane2_VRESIZE this makes the pane
  stretch to the vertical size of the window.
*/

#define pane2_HSCROLL   0x0080
/*
  This makes the pane scroll with the main window in the horizontal direction.
*/

#define pane2_VSCROLL   0x0100
/*
  This makes the pane scroll with the main window in the vertical direction.
*/

#define pane2_MAINRIGHT 0x0200
/*
  This makes the pane move relative to the right of the main window rather
  than the left.
*/


extern BOOL Pane2_AddMain(window_handle mainwindow);
/*
  This intitialises 'mainwindow' with the library as a main window that
  can have panes attached to it.
*/


extern window_handle Pane2_CreateAndAddMain(const char *windowname, int maxtitlesize);
/*
  This creates the given window is the same manner as Window_Create, and
  initialises it with the Pane2 library so that it can have panes attached
  to it.

  The window handle is returned, or zero if the window could not be created
  for some reason.
*/


extern BOOL Pane2_AddPane(window_handle mainwindow, window_handle panewindow,
                          wimp_point *paneoffset, wimp_point *panesize,
                          int flags);
/*
  Adds the pane window 'panewindow' to the main window 'mainwindow'.
  If 'paneoffset' is not NULL then this is taken as the offset between the
  pane and the main window instead of the positions in the template.
  If 'panesize' is not NULL then this is taken as the size of the pane
  instead of the size in the template.

  It returns FALSE if it fails for some reason, TRUE otherwise.
*/


extern window_handle Pane2_CreateAndAddPane(const char *windowname, int maxtitlesize,
                                            window_handle mainwindow,
                                            wimp_point *paneoffset,
                                            wimp_point *panesize,
                                            int flags);
/*
  This creates the pane window (as it would be done by Window_Create), and
  adds it as a pane to the main window 'mainwindow'.
  If 'paneoffset' is not NULL then this is taken as the offset between the
  pane and the main window instead of the positions in the template.
  If 'panesize' is not NULL then this is taken as the size of the pane
  instead of the size in the template.

  It returns the window handle of the created pane, or NULL if it cannot
  create it for some reason.
*/


extern void Pane2_Show(window_handle mainwindow, window_openpos openpos);
/*
  This opens the main window 'mainwindow' and all its attached pane windows
  according to the position given by openpos.
*/


extern void Pane2_Hide(window_handle mainwindow);
/*
  This hides (closes) the given 'mainwindow' and all associated panes.
*/


extern void Pane2_Delete(window_handle mainwindow);
/*
  This hides (closes) and deletes  the given 'mainwindow' and all
  associated panes.  Before deleting, it releases all events claimed
  with Event_Claim for the windows.
*/


extern void Pane2_ReadIcon(window_handle window, icon_handle icon,
                           const char *panewindow, wimp_point *paneoffset,
                           wimp_point *panesize);
/*
  This calculates the pane offset and size so that the pane will be in the
  same position and size as the given icon in the given window, and fills
  in 'paneoffset' and 'panewindow' with the appropriate values.  You need
  to use the pane2_MAINTOP and pane2_PANETOP flags when you attach the
  pane to the window for these values to work.

  If 'panewindow' is the name of the pane window in the template file then
  the presence of scroll bars or window titles is taken into account.  If it
  is NULL no such calculation is used.  Note that for this purpose the
  scrollbars and the titlebar are assumed to be 40 OS units wide (or high,
  depending).
*/


extern int Pane2_SetFlags(window_handle mainwindow, int panenumber, int flags);
/*
  This sets the flags of the 'panenumber'th pane attached to the given
  window to the given value.  Pane numbers start from 1. If the pane number
  is invalid, no action is taken and NULL is returned, otherwise the new value
  of the flags is returned.
*/


extern window_handle Pane2_GetPaneHandle(window_handle mainwindow,
                                         int panenumber);
/*
  This returns the window handle of the 'panenumber'th pane attached to the
  given window 'main', or zero if the pane number is invalid.  Pane numbers
  start at 1.
*/


extern window_handle Pane2_GetMainHandle(window_handle panewindow);
/*
  This returns the window handle of the main window to which the given pane
  is attached, or zero if it could not be found.
*/


extern int Pane2_GetPaneNumber(window_handle mainwindow,
                               window_handle panewindow);
/*
  This returns the pane number of 'panewindow' attached to 'mainwindow', or
  zero if the pane was not found.  This number can be used with some of the
  other Pane2 functions to identify the pane.
*/


extern void Pane2_DeletePane(window_handle mainwindow, int panenumber);
/*
  This hides and deletes the 'panenumber'th pane attached to the window
  'mainwindow'.  Before deleting, it releases all events claimed with
  Event_Claim for the window.
*/


extern BOOL Pane2_OpenWindow(window_handle mainwindow,
                             window_openblock *openblock);
/*
  This opens the main window and all its panes as if it was sent an
  openwindow request.  If 'openblock' is NULL then the current state of the
  main window is used instead.  Supplying an openblock allows the main window
  to be changed and the panes to be altered to match.  This can be used to
  make a pane window appear that has been added to an open mainwindow.
*/


extern BOOL Pane2_ReadVars(window_handle mainwindow, int panenumber,
                           wimp_point *paneoffset, wimp_point *panesize);
/*
  This read and fills in 'paneoffset' and 'panesize' for the 'panenumber'th
  pane attached to 'mainwindow'.  If you pass NULL for either of these they
  will not be filled.  This function returns TRUE for sucess, and FALSE if
  the position could not be read, for instance because of an invalid panenumber.
*/


extern BOOL Pane2_SetVars(window_handle mainwindow, int panenumber,
                          wimp_point *paneoffset, wimp_point *panesize);
/*
  This sets the paneoffset and panesize for the given pane attached to
  the given window.  The value is only set if 'paneoffset' or 'panesize' is
  non-NULL, respectively.  This function returns TRUE for sucess, and FALSE if
  the position could not be read, for instance because of an invalid panenumber.

  After changing the variables, Pane2_OpenWindow must be used to make the
  windows on screen exhibit the changes.
*/

#ifdef __cplusplus
}
#endif


#endif
