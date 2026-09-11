/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Dialog.h
    Author:  Copyright © 1993 Tim Browse, with modifications by JW
    Version: 1.02 (17 Apr 2005)
    Purpose: Very high-level window (dialogue box) handling
*/

#ifndef __dl_dialog_h
#define __dl_dialog_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifndef __dl_window_h
#include "DeskLib:Window.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  A dialogue box is a window that is popped open to get user choices and
  then goes away - they can act like menus (go away when you click outside
  them) if opened with Dialog_Show, or can be permanent windows if
  you open them using Dialog_ShowStatic.

  This code is intended to simplify popping up such dialogs, as it handles
  opening, closing and simple processing of the window for you.

  Note that the Dialog functions make use of the Template and Event
  sublibraries, so these should be initialised properly and a suitable
  template file loaded before using these functions.
*/


/*
  To use Dialog, you do something like the following:
  (Assuming a window where icon 0 is the 'OK' button, and icon 1 is the
   'Cancel' button)

  {
    dialog dlog;

    dlog = Dialog_Create("template", 0);           // Create the window
    Dialog_ShowStatic(dlog, open_UNDERPOINTER);    // Pop up under pointer

    while (Dialog_WaitForClick(dlog) > 1 || Dialog_Persist(dlog))
      |* Busy wait loop *| ;                       // Wait for OK/Cancel

    Window_Hide(dlog);      // Hide the window before we start processing

    if (Dialog_LastClicked(dlog) == 0)
      ProcessTheData();     // OK was clicked, so go ahead with processing

    Dialog_Destroy(dlog);
  }


  SeeAlso:  dialog_record; dialog
*/



typedef struct
{
  window_handle window;         /* The window handle of this dialog      */

  icon_handle   lastclicked;    /* The icon handle of the last icon that
                                 * was clicked, or dialog_NOCHOICE if no
                                 * icons have been clicked yet. */

  button_state  button;         /* The button state for the last
                                 * click event. */
  struct
  {
    unsigned int stillopen : 1; /* The dialogue window is still open     */

    unsigned int persist   : 1; /* It should persist (adjust was clicked)*/

    unsigned int isstatic  : 1; /* It is a static (permanent) dialogue   */
  } state;
} dialog_record;
/*
  Holds information about a particular dialogue box.

  'window' is the window_handle of the dialogue box.  'lastclicked' is the icon
  handle of the last icon that was clicked, or dialog_NOCHOICE if no icons
  have been clicked.  'button' is the button state for the last click event.

  The 'state' struct holds the state of the dialogue - whether it is stil open,
  if it should persist because adjust was clicked and whether it is a static
  (permanent) dialogue.
*/


typedef dialog_record *dialog;
/*
  The usual type passed to Dialog functions - a pointer to information
  about a particular dialogue box.
 */


#define dialog_CLOSE    ((icon_handle) -1)
/*
  This is returned by Dialog_WaitForClick when the dialogue box is closed
  by clicking outside the window or clicking on a close icon.
*/


#define dialog_NOCHOICE ((icon_handle) -2)
/*
  This is returned by Dialog_LastClicked when the user has not yet done
  anything (ie. the window is open but no icons have been clicked)
*/


extern dialog Dialog_Create(const char *template_name, int maxtitlesize);
/*
  This function creates (but does not show) a dialogue box from the template
  whose name is given.  The maxtitlesize is the maximum space to reserve
  for the title bar text of the dialogue box, or template_TITLEDEFAULT or
  template_TITLEMIN.

  It returns the newly created dialogue box, or NULL if it was not possible
  to create the dialogue.

  Possible errors are: Out of memory; Could not find window template
*/


extern void Dialog_Destroy(dialog d);
/*
  This function removes the dialogue box whose handle is given, both from
  display and memory.  The handle is then completely invalid, and must not
  be used with Dialog functions (except Dialog_Create of course).
*/


extern void Dialog_Show(dialog d);
/*
  This function shows the dialogue box whose handle is given in the centre
  of the screen.  It is opened as a transient dialogue box, ie. it will
  disappear if you click outside it or press Escape.
*/


extern void Dialog_ShowAt(dialog d, int x, int y);
/*
  This function shows the dialogue box whose handle is given with the top
  left of the window at the position specified. It is opened as a transient
  dialogue box, ie. it will disappear if you click outside it or press Escape.
*/


extern void Dialog_ShowStatic(dialog d, window_openpos openpos);
/*
  This function shows the dialogue box whose handle is given as a static
  dialogue box - it only closes if the close icon is clicked or if the
  application closes it.  The opening position is specified by 'openpos'
  (see Window.h for window_openpos information)
*/


extern void Dialog_Hide(dialog d);
/*
  This hides the given dialogue box by closing the window, but does not
  destroy it.  It can be reopened with Dialog_Show, for instance.
*/


extern int Dialog_WaitForClick(dialog d);
/*
  This waits for an icon to be clicked in the dialogue box, returning the icon
  handle of the icon clicked, or dialog_CLOSE if the user closed the window.

  It continues to process events while it waits using Event_Poll.
*/


#define Dialog_WindowHandle(d) ((d)->window)
/*
  MACRO:    window_handle Dialog_WindowHandle(dialog d)

  This returns the window handle for the dialogue box window.
  This allows filling in of fields using, eg. DeskLib's 'Icon' sublibrary.
*/


#define Dialog_Persist(D) ((D)->state.persist && (D)->lastclicked >= 0)
/*
  MACRO: BOOL Dialog_Persist(dialog d);

  This checks to see if the user wants the dialogue box to remain open after
  a click, ie. if they clicked with the adjust button.
*/


#define Dialog_LastClicked(D) ((D)->lastclicked)
/*
  MACRO: icon_handle Dialog_LastClicked(dialog d);

  This returns the icon number of the last icon that was clicked in the given
  dialogue box.  If no icons have been clicked, it returns dialog_NOCHOICE.
*/


#define Dialog_StillOpen(D) ((D)->state.stillopen)
/*
  MACRO: BOOL Dialog_StillOpen(dialog d)

  This returns TRUE if the dialogue box is open, FALSE otherwise.
*/


#ifdef __cplusplus
}
#endif


#endif
