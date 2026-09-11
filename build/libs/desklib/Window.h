/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Window.h
    Author:  Copyright © 1992, 1993, 1994, 1995 Jason Williams, Cy Booker and
                                                Sergio Monesi
    Version: 1.14 (17 Apr 2005)
    Purpose: High-level window management functions
    Mods:    1.10 (Dec 1994) Jason Williams
             1.11 (05 Mar 1995) Cy Boooker
                  Window_GetInfo3() now returns os_error *
                  added Window_GainCaret()
             1.12 (14 Jul 1995) Sergio Monesi
                  Added Window_CreateOrig(), Window_DeleteOrig() and
                  Window_ForceWholeRedraw()
             1.13 (26 Jul 1995) Sergio Monesi
                  Added Window_MoveWindow()
             1.14 (17 Apr 2005) John Tytgat
                  Made it usable for C++
*/


#ifndef __dl_window_h
#define __dl_window_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifndef __dl_wimpswis_h
#include "DeskLib:WimpSWIs.h"
#endif

#ifndef __dl_pointer_h
#include "DeskLib:Pointer.h"
#endif

#ifndef __dl_coord_h
#include "DeskLib:Coord.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header declares a number of functions to provide high-level handling
  of windows.  A number of them depend on you having used the Template library
  to load your templates, for example Window_Create.
*/


typedef enum
{
  open_WHEREVER,             /* As defined in the window template file */
  open_CENTERED,             /* center of screen */
  open_CENTEREDUNDERPOINTER, /* Center window on pointer position */
  open_OVERCARET,            /* Over the current caret position. NOTE: If no
                                caret, window is opened open_CENTERED */
  open_UNDERPOINTER,         /* Under the current pointer position (so that
                                the pointer is then in TL corner of window */
  open_NEARLAST              /* Slightly offset from the T.L. pos of the last
                                window to be opened with Window_Show  */
} window_openpos;
/*
  These are various opening positions for use with Window_Show and
  similar functions.

  open_WHEREVER opens it in its last known position, or where it was defined
  in the template file.  open_OVERCARET opens it over the current caret
  position; if there is no caret it is opened centrally on the screen.
  open_UNDERPOINTER opens it so that the pointer is in the top left of the
  window if possible,  open_NEARLAST opens it slightly offset from the top
  left of the last window opened with Window_Show.
*/


extern window_handle Window_Create(const char *windowname, int maxtitlesize);
/*
  This finds the named window template (the file containing it should have
  been loaded with Template_LoadFile) and creates a new copy of the
  window.  It returns the handle of the new window, or NULL if the template
  could not be found.

  'maxtitlesize' should be the maximum length (in bytes) that will be needed
  to hold the title, or else one of template_TITLEMIN or template_TITLEDEFAULT.

  This window may be deleted with Window_Delete, which will free the
  associated memory.
*/


extern window_handle Window_CreateOrig(const char *windowname);
/*
  This finds the named window template (the file containing it should have
  been loaded with Template_LoadFile) and creates a new window directly
  from this template It returns the handle of the new window, or NULL if the
  template could not be found.

  'maxtitlesize' should be the maximum length (in bytes) that will be needed
  to hold the title, or else one of template_TITLEMIN or template_TITLEDEFAULT.

  This window should not be deleted with Window_Delete, as this will mess up
  the memory deallocation system.  It should only be deleted with
  Window_DeleteOrig.
*/


extern void Window_Show(window_handle window, window_openpos openpos);
/*
  This opens the given window on screen in the desired position.  This
  can be used with any window, whether or not it was created with "Window"
  functions.  You should pass one of the values of the enumerated type
  window_openpos to specify the opening position.
*/


extern window_handle Window_CreateAndShow(const char *windowname,
                                          int  maxtitlesize,
                                          window_openpos openpos);
/*
  This simply calls Window_Create and then Window_Show.  You can use it
  to make your programs a little tidier, where appropriate.
*/


#define Window_Hide(handle) Wimp_CloseWindow(handle)
/*
  Macro: void Window_Hide(window_handle handle);

  This closes the specified window (ie. removes it from the screen).
  It does not delete the window, it can be reopened with Window_Show.
*/


extern void Window_Delete(window_handle window);
/*
  This hides the window (closes it), and deletes it.  Before deleting, it
  removes any Event handlers attached to the window.  If the window was
  created with Window_Create, it also frees any memory used by the window.
*/


extern void Window_DeleteOrig(window_handle window);
/*
  This hides and deletes a window created with Window_CreateOrig.  It also
  removes any Event handlers attached to the window.  If the window was
  created with Window_CreateOrig and you wish to delete it, you must use
  this function and not Window_Delete
*/


#define Window_GetInfo3 Window_GetInfo
/*
  Old versions of DeskLib included a RISC OS 2 compatible Window_GetInfo
  function, with Window_GetInfo3 being the RISC OS 3 version. The RISC OS
  2 function is no longer present so the function has been renamed as
  simply Window_GetInfo. This macro is defined in order to allow code
  written using the old naming scheme to still work.
*/


extern os_error *Window_GetInfo(window_handle window, window_info *result);
/*
  This is simply a frontend to the Wimp_GetWindowInfo call. However, it
  returns the Window info block *without* the icon definitions.
  Thus, you can use a window_info structure in your local variables without
  having to allocate enough memory to cope with any icons that the window
  happens to have in it.
*/


extern void Window_ParentName(window_handle window, char *windowname);
/*
  This takes the handle of a window created with Window_Create, and
  attempts to find the name (8 characters only, truncated if necessary)
  of the template from which it was created.  If the window handle given
  is a negative number, "iconbar" is returned.
*/


extern BOOL Window_AutoHelp(window_handle win, icon_handle icon);
/*
  This adds a handler with the Event system for the given window and icon
  (event_ANY and window_ICONBAR may be used).  Thereafter, every
  HelpRequest message received for the window will be answered with a
  message from the list of messages loaded with Msgs_LoadFile, if one can
  be found.  Note that you will have to ensure that your task receives
  the help requests yourself, as it will be handled through Event and not
  EventMsg.

  The token for the message is constructed with the window's template name
  as the group part and the icon number as the tag part, ie. "window.icon".
  Any part of the window not covered by an icon is covered by a message
  with icon number -1 (eg. "mainwind.-1").  You can also use a message for
  anything not specified individually by using '*' (eg. "mainwind.*").

  All this function does is:
  Event_Claim(event_USERMESSAGERECORDED, win, icon, Window_HelpHandler, NULL);

  An alternative (using EventMsg instead):
  EventMsg_Claim(message_HELPREQUEST, win, Window_HelpHandler, NULL);
*/


extern BOOL Window_HelpHandler(event_pollblock *event, void *reference);
/*
  This is an event handler which provides help on windows and their icons,
  by finding a message from the list loaded by Msgs_LoadFile.  See
  Window_AutoHelp for details of the format for the messages.

  You can use this in combination with a custom help handler by checking
  if the pointer is in a place you want to give customised help about
  (eg. something depending on the file being displayed) and then calling
  this function for all other areas.  Another way is to register this
  function first, and then register a custom handler so that your other
  function gets called first, and this one automatically gets called if
  that one returns FALSE.

  This handler returns TRUE if it finds and sends a help message and FALSE
  otherwise.
*/


extern void Window_ModeChange(void);
/*
  This function will go through all of your templates and the windows
  created with them, and re-find the outline fonts used in them.  This
  should be done after a mode change to keep the fonts up to date, and
  the Handler_ModeChange function is provided to do this for you if you
  wish.  Fonts in window titles are not dealt with, as this would require
  recreating the window, among other things.

  Note that you can only call this function if *all* of your windows with
  outline fonts were created with Window_Create.  Otherwise it will
  release the font, not knowing that another window is still using it,
  which is almost certain to cause problems for your application.
*/


extern void Window_SetTitle(window_handle window, const char *title);
/*
  This changes the title in the given window to that pointed to by 'title'.
  If the title is not indirected the text cannot be changed, but no error is
  generated.  If the string is too long to fit, it is truncated to just
  fit in the available space.

  After setting the text, it redraws the area of the screen corresponding to
  the titlebar (if the window is open).
*/


extern void Window_BringToFront(window_handle window);
/*
  This brings the given window to the front of the window stack, opening
  it if necessary.
*/


#define Window_ConstrainMouse(WND) (Pointer_RestrictToWindow(WND))
/*
  Macro: os_error *Window_ConstrainMouse(window_handle window);

  This restricts the pointer to the bounds of the given window.
*/


extern void Window_SetExtent(window_handle wh, int x0, int y0, int x1, int y1);
/*
  This sets the window's work area extent.  If the window is open on
  screen, Wimp_OpenWindow will be called to make sure the change is
  reflected on the screen.
*/


extern void Window_ForceRedraw(window_handle wh,
                               int x0, int y0, int x1, int y1);
/*
  This marks an area of the screen to be redrawn by the Wimp.  The screen
  area redrawn corresponds to the given rectangle of the given window's
  workarea, with (x0, y0) being the top left corner.

  This is essentially just a veneer for Wimp_ForceRedraw.
*/


extern void Window_GetCoords(window_handle wh, convert_block *coords);
/*
  This fills 'coords' with the coordinates for the given window.
  You use this function to read a convert block for use with the
  Coord library, for instance the Coord_WindowOrigin function.
*/


extern os_error *Window_GainCaret(window_handle window);
/*
  The gives the specified window the input focus, putting the caret in the
  first writable, non-shaded icon in the window,  If you pass zero as the
  window, the window currently with the input focus loses it.  If the
  given window already has the input focus, nothing happens.
*/


extern os_error *Window_ForceWholeRedraw(window_handle window);
/*
  This forces the redraw of the screen area corresponding to the given
  window.
*/


extern BOOL Window_MoveWindow(event_pollblock *event, void *reference);
/*
  This is a handler function to allow you to move a window by dragging
  any (or all) of its icons.  You can either register this function to
  be called when the given icon or window is clicked on, or else call
  it yourself (passing the parameters you received) from your handler
  for the icons that you want to work like the title bar.

  The icon button type should be iconbtype_CLICK (3); if you want the
  window background to be used as well, this should be set to the same
  button type.

  This function *always* returns TRUE.
*/

extern BOOL Window_IsOpen(window_handle window);
/*
  This returns TRUE if the window is currently open.
*/


#ifdef __cplusplus
}
#endif

#endif
