/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Event.h
    Author:  Copyright © 1992 Jason Williams (for code by John Winters)
    Version: 1.02 (17 Apr 2005)
    Purpose: High-level WIMP event dispatch to a hierarchy of user event
             handling functions.
    Mods:    14 July 1993 - Added Event_Initialise3
             20 Mar 1995  - JPS Added veneers for global vars for use with DLLs
*/


#ifndef __dl_event_h
#define __dl_event_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  Event supplies a set of high-level function for proper handling of Wimp
  events.

  It polls the Wimp for you, and then passes the resulting event to one
  or more of your handler routines based on the event which occurred and the
  window and icon the event occurred in.  Events are cascaded down through
  your handlers until one of them returns TRUE, indicating that it has been
  dealt with succesfully.  This allows you to have application-wide
  defaults for certain event types, with occasional overrides of that
  default for specific windows or icons.

  Any event which has no handlers attached is automatically masked out in
  subsequent Wimp_Polls (except for quit messages).  A set of default (and
  example) handlers for some events is provided in the Handlers library.
*/


typedef BOOL (*event_handler) (event_pollblock *poll_block, void *reference);
/*
  This type is for functions which can be used to process events claimed using
  Event_Claim.  It should return TRUE if it has processed the event passed
  to it, or FALSE to let it "fall through" for any other handlers to try and
  deal with it.
*/

extern event_pollmask  event_mask;
/* Event mask used by the Event FNs for Wimp_Poll and Wimp_PollIdle calls */

extern int event_taskhandle;
/*
   Wimp task-handle of this application, set by Event_Initialise
   or equivalent.
*/

extern unsigned int    event_wimpversion;
/*
  This is the Wimp version number * 100, as set up when the Event
  library is initialised.
*/

extern char event_taskname[40];
/* Your application name, set by Event_Initialise or equivalent.  */

extern event_pollblock event_lastevent;
/* This contains the last event received by the Event system. */



extern BOOL Event_Claim(event_type eventtype,
                        window_handle window,  icon_handle icon,
                        event_handler handler, void *reference);
/*
  This function attaches an event handler to a a particular 'eventtype',
  'window' and 'icon'.  Any of these can be event_ANY to be non-specific.
  'handler' is your handler function, and 'reference' is passed directly to
  your handler function and can be anything you like.

  You can only attach events to a specific icon if you also specify a window.

  The more-specific events are called first, so you can have a general routine
  handling all icon clicks, and then several ones for specific icons which will
  be called first if those icons are clicked.  If two events are of identical
  priority (ie. you attach the same event to the same window and icon), the
  one most recently attached will be called first.

  Note that Null reason events are not enabled unless you specifically ask for
  them, whereas all other events will be enabled by you registering a handler
  for event_ANY.
*/


extern BOOL Event_Release(event_type event,
                          window_handle  window, icon_handle icon,
                          event_handler handler, void *reference);
/*
  This releases an event handler. This should be called with same parameters
  as were passed to Event_Claim to claim it. It will only release one claim
  each time it is called, so make sure you haven't mistakenly made multiple
  identical claims.
*/


extern void Event_ReleaseWindow(window_handle window);
/*
  This function releases all handlers which were attached to a specific window.
  This should called by you if you delete a window with low-level routines,
  but it is done for you if you use Window_Delete.
*/


extern void Event_Process(event_pollblock *event);
/*
  Call this to process a Wimp_Poll event. It will cascade the event down
  through the event-handler hierarchy until one of your event handlers
  returns TRUE to indicate succesful handling of the message.

  Note that you only need to call this if you have "manually" polled the
  Wimp using a SWI rather than using Event_Poll or Event_PollIdle, as these
  functions call it for you.
*/


extern void Event_Poll(void);
/*
  This is a simple event poll routine to be called by your main poll loop.
  It simply calls Wimp_Poll and then Event_Process for you.

  eg. while (TRUE) Event_Poll();
*/


extern void Event_PollIdle(unsigned int delay);
/*
  This is a simple event poll routine to be called by your main poll loop.
  It simply calls Wimp_PollIdle and then Event_Process for you.

  eg. while (TRUE) Event_PollIdle(100);

  This would mean that the earliest you would want to receive a Null reason
  event was 100 centiseconds after this call.  Only really useful if you
  have claimed a Null reason event.
*/


extern void Event_Initialise3(const char *taskname, int version, int *messages);
/*
  This initialises the Wimp and the Event system for your task.

  taskname should be your task name, version should be the Wimp version * 100
  required for your program to run. and messages should point to a
  zero-terminated list of user message numbers you are interested in.

  A messages list with just zero means you are interested in all messages,
  whereas passing NULL means you aren't interested in any messages (except
  quit, which you will receive anyway).

  Note: All the above is true for Wimp versions >= 310 which are the only
  versions now supported by DeskLib.
*/

#define Event_Initialise(taskname) \
        Event_Initialise3(taskname, 310, NULL)
/*
  MACRO: void Event_Initialise(const char *taskname);

  This calls Event_Initialise3 for you, with version set to 310 (the
  earliest version supported by DeskLib), and expressing an interest in
  *no* messages.

  Note: in versions of DeskLib up to and including 2.40, this set the wimp
  version to 200 and got notified of *all* messages.
*/


extern void Event_InitNested(const char *taskname);
/*
  Version of Event_Initialise to pass default values for a
  task under a Nested Wimp with no messages.
*/


extern void Event_CloseDown(void);
/*
  Call this to Wimp_CloseDown your application.  This calls exit(), so nothing
  after this in your application will be called (you shouldn't do anything
  after calling Wimp_CloseDown anyway).
*/


typedef BOOL (*event_claimorreleasefn)(event_type eventtype,
                                       window_handle window,
                                       icon_handle icon,
                                       event_handler handler,
                                       void *reference);
/*
  This is the function-type of the Event_Claim and Event_Release
  functions. It is useful for writing functions which accept a pointer to
  either of these functions, in order to claim or release a set of events
  consistently.
*/


#ifdef __cplusplus
}
#endif


#endif
