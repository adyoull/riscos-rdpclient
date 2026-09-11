/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Handler.h
    Author:  Copyright © 1992, 1993, 1994 Jason Williams
    Version: 1.03 (01 April 1994)
    Purpose: Predefined default handlers for you to COPY and use as the
             basis of your own handlers

             NOTE that these are basically EXAMPLE handlers, and so are
             not all designed with peak efficiency in mind.
             PLEASE read the information about these handlers *carefully*
             before use.
             Note that some handlers are for Event, some are for EventMsg
*/


#ifndef __dl_handler_h
#define __dl_handler_h

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  These are default (and example) handler functions for you to use with
  the Event and EventMsg libraries.

  These are essentially examples of how to write handlers and so are not
  designed with peak efficiency in mind.  Read the information about the
  handlers carefully before use.
*/


extern BOOL Handler_CloseWindow(event_pollblock *event, void *reference);
/*
  Simply calls Wimp_CloseWindow()
*/


extern BOOL Handler_DeleteWindow(event_pollblock *event, void *reference);
/*
  Alternate handler to Handler_Close, which calls the higher-level
  Window_Delete() to close and delete the window, release memory, and
  remove all handlers attached to the window.
*/


extern BOOL Handler_OpenWindow(event_pollblock *event, void *reference);
/*
  Simply calls Wimp_OpenWindow()
*/


extern BOOL Handler_NullRedraw(event_pollblock *event, void *reference);
/*
  Supplies a NULL redraw loop (gets rectangles from the WIMP until
  all rectangles have been "redrawn"). Doesn't actually do any drawing
  though.
*/


extern BOOL Handler_ClickOK(event_pollblock *event, void *reference);
/*
  Handler for non-menu clicks on "OK" or "Cancel" (3d) type buttons
  (Indents the button, waits for 1/3 second, and un-indents the button)
*/


extern BOOL Handler_Key(event_pollblock *event, void *reference);
/*
  Handler for unprocessed key events.  Calls Wimp_ProcessKey to pass
  the code on to other tasks.
*/


extern BOOL Handler_ModeChange(event_pollblock *event, void *reference);
/*
  This is a handler for mode change events. It calls Screen_CacheModeInfo()
  every time the screen mode changes, guaranteeing that the screen variables
  are always correct (You must call Screen_CacheModeInfo() once during
  initialisation for this to work).

  If you have called Template_UseOutlineFonts() and the screen resolution
  has changed in such a way as to necessitate it, it also calls
  Window_ModeChange to fix all your icons which use outline fonts
  (otherwise, they often become the wrong size upon a mode change).

  (Note that this won't have a very useful effect unless you use Template
  and Window functions to create all your windows - in fact, such windows
  will no longer 'own' the fonts they are using, and these fonts may
  disappear or change at random in the future - so DON'T use this function
  unless ALL your windows were created with Window_Create.

  eg. EventMsg_Claim(message_MODECHANGE, event_any, Handler_ModeChange, NULL);
*/


extern BOOL Handler_DragNULL(event_pollblock *event, void *reference);
/*
  This is used by the Drag sublibrary.

  DragNULL is called on NULL events to allow drag updates (redraw of a
  selection as it is dragged, for example). It calls the drag handler
  registered with Drag_SetHandlers() on each NULL it recieves.
*/


extern BOOL Handler_DragFinish(event_pollblock *event, void *reference);
/*
  This is used by the Drag sublibrary.

  DragFinish is called on a USERDRAG event (drag has finished) to tidy up
  at the end of a drag (Complete a selection, or save a file, etc).
  This calls the Drag-Completed handler registered with Drag_SetHandlers
  and also resets the drag handlers (as no drag is now in progress)
*/



extern BOOL Handler_HatchRedraw(event_pollblock *event, void *reference);
/*
  Hatch the work area of a window. Useful when testing
  a program before you get round to writing your own redraw
  routines so you can see that something is happening.
*/


#ifdef __cplusplus
}
#endif


#endif
