/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane.h
    Author:  Copyright © 1994 Ainsley Pereira. Complete revision by Keith Hall
    Version: 1.12 (17 Apr 2005)
    Purpose: Handles windows with panes.
*/


/* Abstract */
/*
  The functions declared in this header are intended to make handling panes
  relatively easy.  It works by allowing you to attach a pane to to a
  "master window".  You can then refer to the linked pair by the handle of
  that master window.

  It attaches panes relative to the top-left corner of the visible area of
  the window - if you want to attach to the bottom edge of a resizable window,
  you will have to update the offsets yourself as the window is resized, or
  else alter the Pane code to attach to the bottom edge instead of the top.

  The default offset between the windows, and the default size of the pane
  are read from the template file.
*/


#ifndef __dl_pane_h
#define __dl_pane_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifndef __dl_window_h
#include "DeskLib:Window.h"
#endif

#ifndef __dl_wimpswis_h
#include "DeskLib:WimpSWIs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  window_handle master;
  window_handle pane;
  wimp_point    offset;
  wimp_point    size;
  union
  {
    int value;
    struct
    {
      unsigned int isopen :1; /* master/pane pair currently displaying */
      unsigned int fixed  :1; /* pane sticks to position relative
                                 to top of master */
      unsigned int resize :1; /* pane shrinks with relation to size of master */
      unsigned int horiz  :1; /* pane is horizontal (shrinks to left when
                                 resize flag set and window gets resized
                                 (like !StrongEd toolbar) */
      unsigned int vert   :1; /* pane is vertical (shrinks to top when resize
                                 flag set and window get resized */
      unsigned int dummy  :27;
    } data;
  } flags;
} pane_data;
/*
  This holds information about a 'master' window and its associated pane.
  It is set up by Pane_Link or Pane_CreateAndLink.  The offset is the offset
  of the top-left of the visible area of the pane from the top-left of the
  visible area of the window.  If you want to keep a pane attached to the
  bottom of a resizable window you will have to alter this offset whenever
  the window is resized.

  This 'isopen' flag is set when the pane pair is currently displaying.  The
  'fixed' flag means that the pane keeps the same position relative to the
  master window.  If you set 'resize', then the pane will be shrunk if the
  window is smaller than the maximum pane size, as the main window is resized.
  You decide how the pane should shrink by setting 'horiz' or 'vert'.  'horiz'
  means that the pane is horizontal, and should be shrunk to the width of the
  main window.  'vert' means that the pane is vertical and should be shrunk
  to the height of the main window.
*/


#define pane_OPEN   0x0001
/*
  This flag means that the window/pane pair is currently open.
*/

#define pane_FIXED  0x0002
/*
  This flag means that the position of the pane is fixed relative to the
  position of the main window.
*/

#define pane_RESIZE 0x0004
/*
  This flag means the the pane will be resized appropriately when the
  main window is resized.
*/

#define pane_HORIZ  0x0008
/*
  This means that the pane is horizontal, and that if pane_RESIZE is set,
  the window will be resized from its right end, with its left end remaining
  fixed.
*/

#define pane_VERT   0x0010
/*
  This means that the pane is vertical, and that if pane_RESIZE is set,
  the window will be resized from the bottom, with the top remaining fixed.
*/


extern BOOL Pane_OpenEventHandler(event_pollblock *event, void *reference);
/*
  This can be used as an Event handler for a window with a pane, to open
  the pane at the correct offset from the main window and at the correct
  size.  Simply attach it to event_OPEN for the main window.
*/


extern pane_data *Pane_GetSysHandle(window_handle master);
/*
  This returns a pointer to the pane_data associated with the given
  'master' window.

  It returns NULL if the the master window isn't linked to a pane.
*/


extern int Pane_SetFlags(window_handle master, int flags);
/*
  This sets or reads the flags for the pane/master window relationship.
  Pass in the new flags to set, or -1 to read.

  It returns NULL if the the master window isn't linked to a pane.
*/


extern void Pane_Link(window_handle mast, window_handle pane,
                      wimp_point *offset, wimp_point *size, int flags);
/*
  This links the master and pane windows whose window handles are given.
  'offset' specifies the offset between the top left of the visible area
  of each window, and 'size' specifies the size of the pane.

  If either 'offset' or 'size' is NULL, the information is taken from the
  window definitions themselves (normally however you set them up in the
  template file).
*/


extern window_handle Pane_CreateAndLink(const char *mastname, const char *panename,
                                        int mastmaxsize, int panemaxsize,
                                        wimp_point *offset, wimp_point *size,
                                        int flags);
/*
  This creates the master and pane windows given in 'mastname' and 'panename',
  and links them together.  'offset' specifies the offset between the top left
  of the visible area of each window, and 'size' specifies the size of the
  pane.  If either 'offset' or 'size' is NULL, the information is taken from the
  template file.

  The 'mastmaxsize' and 'panemaxsize' are the maximum space to reserve
  for the text of each window's title bar, or else template_TITLEDEFAULT or
  template_TITLEMIN.
*/


extern void Pane_Show(window_handle window, window_openpos openpos);
/*
  This calls Window_Show for the master window and the associated pane.
*/


extern void Pane_Delete(window_handle window);
/*
  This delinks the master window and its pane, and deletes both with
  Window_Delete, so any events claimed for either window will be
  automatically released for you.
*/


extern void Pane_Hide(window_handle window);
/*
  This hides (closes) the given master window and associated pane with
  Window_Hide.
*/


#ifdef __cplusplus
}
#endif


#endif
