/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Drag.h
    Author:  Copyright © 1994 Jason Williams
    Version: 1.01 (17 Apr 2005)
    Purpose: Automatic handling of drag update and finish events
*/

#ifndef __dl_drag_h
#define __dl_drag_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header provides a way to handle drags using the Event sublibrary.
*/


extern void Drag_Initialise(BOOL attachNULLhandler);
/*
  This function must be called to initialise the drag system for use with
  the Event sublibrary before you use other Drag functions.

  attachNULLhandler should be TRUE if you want to use an update handler to
  do things on Null events.  You usually only need this if you are dragging
  user-redrawn objects, or are using the drag-and-drop protocol between
  applications.
*/


typedef void (*drag_handler) (void *userdata);
/*
  This type is used to define functions called by the Drag library during and
  at the end of a drag.  The userdata is passed to these functions to indicate
  what drag is being updated or finished.
*/


extern drag_handler drag_currentupdate;
/*
  This is used to keep track of which function to call during the current drag.
*/


extern drag_handler drag_currentcomplete;
/*
  This is used to keep track of which function to call when the current drag
  ends (when the mouse button is released).
*/


extern void *drag_currentuserdata;
/*
  This contains the user data pointer for the current drag.
*/


extern void Drag_SetHandlers(drag_handler uproc, drag_handler cproc,
                             void *userdata);
/*
  This sets the functions which will be called during the drag (uproc) and
  after when the drag finishes (cproc).  Either of these may be NULL in which
  case it will not be used.  In any case, uproc will only be called if you
  told Drag_Initialise that you wanted to claim Null events.

  userdata is a pointer that is passed to your handlers for your own use.

  Note that this only sets the handlers, it doesn't actually start the drag -
  you need to call Wimp_DragBox or an equivalent to start the drag.
*/


#ifdef __cplusplus
}
#endif


#endif
