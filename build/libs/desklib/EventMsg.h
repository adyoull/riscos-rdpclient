/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    EventMsg.h
    Author:  Copyright © 1992 Jason Williams
    Version: 1.01 (17 Apr 2005)
    Purpose: Augments Event() routines to provide a better way of serving
             WIMP messages to a set of user handler functions
*/


#ifndef __dl_eventmsg_h
#define __dl_eventmsg_h

#include "DeskLib:Event.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  EventMsg is an add-on for the Event system, which provides a layer of
  handlers to handle user messages (reason codes 17-19).

  To use these, you have to call Event_Initialise or an equivalent first.
*/

extern void EventMsg_Initialise(void);
/*
  Initialises the EventMsg system ready for use. Attaches EventMsg event
  handlers to catch all incoming Wimp messages (events 17-19).

  You no longer need to call this function. EventMsg_Claim automatically
  calls EventMsg_Initialise.  This is to support some library functions
  that call EventMsg_Claim when the user may not be aware, and in case
  the programmer forgets for his own code.
*/


extern BOOL EventMsg_Claim(message_action messagetype, window_handle window,
                           event_handler handler, void *reference);
/*
  Much like Event_Claim, but for messages.

  Pass this the type of message (message action) you want to register a
  handler for, the window for which this applies (where appropriate) and
  the handler function to attach.  'messagetype' or 'window' can by
  event_ANY to be called for any message or any window.  'reference' will
  be passed to your handler - it can be anything you want.

  Priorities work like Event_Claim - specific window handlers have
  priority over all-window ones.

  This will call Wimp_AddMessages for any message codes you claim, so that
  your task can be sure of receiving the messages you claim whether you
  ask for them when you initialise the Event library or not.
*/


extern int EventMsg_Release(message_action messagetype, window_handle window,
                            event_handler handler);
/*
  This releases all instances of 'handler' attached to the given message and
  window.

  It returns the number of handlers released.
*/


extern BOOL EventMsg_ReleaseSpecific(message_action messagetype,
                                     window_handle window,
                                     event_handler handler,
                                     void *ref);
/*
  This releases the message handler which was attached with the given type,
  window, etc.  This should be called with same parameters as were passed to
  EventMsg_Claim to claim it.

  It returns TRUE if it released a handler, FALSE otherwise.
*/


extern int EventMsg_ReleaseWindow(window_handle window);
/*
  Finds and removes all message handlers that relate to the given window.
  If you pass in event_ANY as the window handle, all non-window-specific
  handlers will be released.

  It returns the number of handlers released.
*/


extern BOOL EventMsg_ReleaseMessage(message_action messagetype);
/*
  Finds and removes all message handlers that relate to the specified
  message type. If you pass in event_ANY as the type, all handlers that
  were registered as event_ANY will be released.
*/


typedef BOOL (*eventmsg_claimorreleasefn)(message_action messagetype,
                                          window_handle window,
                                          event_handler handler,
                                          void *ref);
/*
  This is the function-type of the EventMsg_Claim and EventMsg_ReleaseSpecific
  functions. It is useful for writing functions which accept a pointer to
  either of these functions, in order to claim or release a set of message
  events consistently.
*/

#ifdef __cplusplus
}
#endif


#endif
