/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Dialog2.h
    Author:  Copyright © 1994 Julian Smith
    Version: 1.04 (17 Apr 2005)
    Purpose: Dialogue box handling
    Mods:    25 Feb 1995 - JPS - Added okfn which is called when program should
                                 read the dialog's contents
             13 Mar 1995 - JPS - Split up source a bit more so that less code
                                 would be linked in.
                                 Bugfix: Event_ReleaseWindow now called when
                                 window isn't deleted.
             10 Apr 1995 - JPS - Modified to support DLLs with the global
                                 variable '*dialog2_menublock'.
*/


#ifndef __dl_dialog2_h
#define __dl_dialog2_h

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
  This header defines ways to handle dialogue boxes - windows that pop
  open to get user choices and then go away again.  This differs from the
  Dialog sublibrary in that it uses function callbacks for you to fill in
  the window contents.

  Like Dialog, these functions make use of the Template and Event
  sublibraries, so these should be initialised properly and a suitable
  template file loaded before using these functions.
*/

typedef enum
{
  dialog2_type_CLOSED = 0,
  dialog2_type_MENULEAF,
  dialog2_type_MENU,
  dialog2_type_STATIC
} dialog2_dialog2type;
/* The various different ways a dialog2 can be opened (or not). */

typedef struct dialog2_block *dialog2_blockptr;

typedef void (*dialog2_openfn)(struct dialog2_block *dialog2);
/*
  A function to be called when a dialog2 window is about to be opened.
*/

typedef void (*dialog2_okfn)(struct dialog2_block *dialog2);
/*
  A function to be called whenever a dialog2 window's contents need acting
  on - whenever OK is clicked or return is pressed in the last writable icon.
*/


typedef struct dialog2_block
{
  void *reference;       /* This is supplied by you. */
  const char *templatename;    /* Name in the Templates file of the window
                                  associated with this dialog2 */
  window_handle window;  /* This is NULL whenever the window has been
                            deleted. */

  dialog2_openfn openfn; /* Your function, called whenever a dialog2
                            is opened. */
  dialog2_okfn okfn;     /* Called when OK is clicked, or
                            return-in-last-icon - ie. whenever the
                            dialog's contents need acting on. */
  union
  {
    int value;
    struct
    {
      unsigned int type : 2; /* A dialog2_dialog2type */
      unsigned int keepwindow :  1;
      unsigned int notifyclose :  1;
      int okbutton : 9;
      int cancelbutton :  9;
      int maxtitlesize : 10;
    } data;
  } flags;
} dialog2_block;
/*
  This structure holds all the information about a particular dialogue box.
  All the fields are all set initially by Dialog2_CreateDialogBlock - you
  shouldn't need to change them normally.

  If you do have to change things yourself, you should *not* change any
  fields directly, as some need to be modified only by internal Dialog2
  functions. There are a few macros and functions defined which give you as
  much access as is good for you!
*/



/* Here come the functions: */

dialog2_block *Dialog2_CreateDialogBlock(const char *templatename,
                                         icon_handle okbutton,
                                         icon_handle cancelbutton,
                                         dialog2_openfn openfn,
                                         dialog2_okfn okfn,
                                         void *reference);
/*
  This creates a dialog2 information block for the window whose name is given
  in templatename.  It returns a pointer to the info block, which can be
  considered a dialog2 handle.  Keep it, use it to open the dialog2 box later.

  Dialog2 dynamically creates the window when you open it, so templatename
  should be a permanent string, eg. Dialog2_CreateDialogBlock("infowindow", ...)

  'okbutton' and 'cancelbutton' are the icon handles of the OK and Cancel
  buttons (or their equivalents) in the window.  Use -1 if these aren't present
  or if you don't want Dialog2 to claim an event_CLICK for them.  Dialog2 uses
  these to close the window when appropriate, and to make escape equivalent to
  a cancel click and return in the last iconbar equivalent to an OK click if
  the validation string is set correctly.

  If 'openfn' is not NULL, it is called whenever the box is opened.  openfn
  should call Event_Claim to claim all events it is interested in, as the
  window will be destroyed when it is closed (unless the keepwindow flag is
  set).  You can choose to have this called when the box is closed, by
  calling Dialog2_SetNotifyClose.

  'okfn' is called whenever OK is clicked (assuming you have set it to non-NULL
  and you have given an okbutton).

  The 'reference' will be stored in the dialog2_block for your later use.
*/

/* The next few functions return ERROR or NOERROR.	*/
/* See DeskLib:Core.h for #defines of (NO)ERROR.	*/

BOOL Dialog2_OpenDialogMenuLeaf(event_pollblock *event, dialog2_block *dialog2);
/*
  Opens the dialog2 window as the leaf of the current menu. This should be done
  in response  to a message_MENUWARN - the event_pollblock should be the
  one from the message_MENUWARN event.

  Returns ERROR if unable to open the window, NOERROR otherwise.
*/

BOOL Dialog2_OpenDialogMenu( dialog2_block *dialog2, window_openpos openpos);
/*
  Opens the dialog2 as a transient dialogue box - clicking outside it will
  make it disappear. This uses Wimp_CreateMenu.

  Returns ERROR if unable to open the window, NOERROR otherwise.
*/

BOOL Dialog2_OpenDialogStatic( dialog2_block *dialog2, window_openpos openpos);
/*
  Opens the dialog2 as a static dialogue box - a proper window.

  Returns ERROR if unable to open the window, NOERROR otherwise.
*/

BOOL Dialog2_CloseDialog(dialog2_block *dialog2);
/*
  Closes the dialogue box. This can, for example, be called from event_CLICK
  handling code for a 'Cancel' button in the dialog2's window if you choose not
  to use Dialog2's built-in handling of the OK and Cancel buttons.

  If you haven't set the keepwindow flag using Dialog2_KeepWindow, the dialogue
  window will be destroyed and all handlers for the window will be released
  automatically.

  If the dialog2 is part of a menu, the menu will be closed automatically.
*/


BOOL Dialog2_DeleteDialog(dialog2_block *dialog2);
/*
  This closes 'dialog2', if it was open, and then removes the dialog2 block
  completely, so you mustn't use 'dialog2' again after calling this
  function.

  Call this if you will never need to open the dialogue ever again.
*/


/*
********************************************************************
********************************************************************
**Other information which you won't need to know about normally...**
********************************************************************
********************************************************************
*/
/*
dialog2->openfn;
    This is a pointer to a function that is called whenever
    the dialog2 is opened, to allow the application to register
    Event_ handlers for mouse-clicks etc.
    This function will also be called when the dialog2 is closed,
    if you set flags.value.notifyclose with Dialog2_SetNotifyClose.

dialog2->flags.data.type : 2;
    Treat as a dialog2_dialog2type, ie it is one of:
    dialog2_type_CLOSED, dialog2_type_MENU, dialog2_type_MENULEAF
    and dialog2_type_STATIC.
    Useful if, for eg, you are thinking of opening a menu based
    on the dialog2 window.
    You might not want to do this if the dialog2 box is already
    a menu, or part of a menu. Also tells you if the dialog2
    is closed.
    *Don't* change 'type' yourself.

dialog2->flags.data.keepwindow : 1;
    If 0 (the default), Wimp_DeleteWindow will be called whenever
    the dialog2 is closed and 'window' set to NULL. Window_Create
    is called whenever the dialog2 is opened, with 'templatename'.
    Hence 'window' will only be a proper window-handle when the
    dialogue is open.

    If 1, the dialog2's window is left created at all times after
    the first time the dialogue is opened.

    You can change 'keepwindow' at any time you please - use
    the macro 'Dialog2_SetKeepWindow', #defined below.

    You should set 'keepwindow' if you want to have your own
    event handlers permanently attatched to the dialog2 window,
    without having to Event_Claim/Release every time the dialog2
    is opened/closed.

dialog2->flags.data.notifyclose : 1;
    If 1, 'openfn' will also be called whenver the dialog
    is closed. 'openfn' will know whether the dialogue is being
    opened/closed by looking at dialog2->flags.data.type, which
    will be dialog2_type_CLOSED if the dialog2 is closing.

    The reason for this is that you might want to register
    a non-window-specific event handler when the dialog2 is
    open - this won't be released when the dialogue window is
    closed with Window_Delete().

    'notifyclose' is set to 0 initially by Dialog2_CreateDialogBox.
    You can change it whenever you like - use the macro
    'Dialog_SetNotifyClose' #defined below.

dialog2->flags.data.maxtitlesize : 10;
    This is initially set to template_TITLEMIN, and is passed to
    Window_Create whenever the dialog2's window needes to be
    created. If you need to change the title of a Dialog2 window
    to something longer than in the original templates file, you
    should do this before the Dialog2's window is created, with
    Dialog2_ChangeTitleSize.
*/


/* These next few prototypes are probably not all that much use normally... */



extern dialog2_block *dialog2_menublock;
/*
  This will always point to the dialog2 which is open as part of a menu,
  or else be NULL. If you call Dialog2_CloseDialog(dialog2_menublock) whenever
  you close a menu (ie a menu choice with Select or Menu), then this will
  free the dialog2.
*/


void Dialog2_Window_GetWindowOpenPos(wimp_point *pos, window_openpos openpos,
                                     window_handle window);
/*
  Makes 'pos' be the top-left of the window if it was opened with
  Window_Show. This actually opens the window using Window_Show, reads
  where the window was opened, and closes the window.
*/


void Dialog2_EnsureWindowHandle(dialog2_block *dialog2);
/*
  This creates a window for the dialog2 box if it hasn't got one already.

  This is useful if you have called Dialog2_KeepWindow, so you can set up event
  claims, etc. once and then not bother with it in the open function as your
  window won't be destroyed automatically.
*/


#define Dialog2_ChangeCloseNotification( dialog2, x) (dialog2)->flags.data.notifyclose = (x)
/*
  MACRO: void Dialog2_ChangeCloseNotification(dialog2_block *dialog2, int x);

  Sets whether or not your open function is called when the window is closed
  as well.  Passing x = 1 turns it on, x = 0 turns it off.
*/


#define Dialog2_NotifyClose( dialog2) Dialog2_ChangeCloseNotification( dialog2, 1)
/*
  MACRO: void Dialog2_NotifyClose(dialog2_block *dialog2);

  Turns off the calling of your dialog2 openfn when the dialogue window is
  closed.  This is the default state.
*/


#define Dialog2_DontNotifyClose( dialog2) Dialog2_ChangeCloseNotification( dialog2, 0)
/*
  MACRO: void Dialog2_DontNotifyClose(dialog2_block *dialog2);

  Turns on the calling of your dialog2 openfn when the dialogue window is
  closed.  The default is not to do this.
*/


#define Dialog2_ChangeKeepWindow(dialog2, x) (dialog2)->flags.data.keepwindow = (x)
/*
  MACRO: void Dialog2_ChangeKeepWindow(dialog2_block *dialog2, int x);

  Sets whether or not your the dialogue window is automatically destroyed when
  it is closed.  Passing x = 1 means it isn't, x = 0 means it is.
*/


#define Dialog2_KeepWindow( dialog2) Dialog2_ChangeKeepWindow( dialog2, 1)
/*
  MACRO: void Dialog2_KeepWindow(dialog2_block *dialog2);

  Stops Dialog2 functions from actually deleting the dialogue window when it
  is closed.  This is not the default setting.
*/


#define Dialog2_DontKeepWindow( dialog2) Dialog2_ChangeKeepWindow( dialog2, 0)
/*
  MACRO: void Dialog2_DontKeepWindow(dialog2_block *dialog2);

  Sets the Dialog2 functions to destroy the dialogue window when it is closed.
  This is the default setting.
*/


#define Dialog2_ChangeTitleSize( dialog2, x) (dialog2)->flags.data.maxtitlesize = (x)
/*
  MACRO: void Dialog2_ChangeTitleSize(dialog2_block *dialog2, int maxtitlesize);

  This sets the maxtitlesize to be passed to Window_Create when the window is
  created.  The maxtitlesize is the maximum space to reserve for the title bar
  text of the dialogue box, or one of template_TITLEDEFAULT or
  template_TITLEMIN.

  The default is template_TITLEMIN.
*/

#ifdef __cplusplus
}
#endif


#endif
