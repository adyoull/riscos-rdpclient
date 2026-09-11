/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Filter.h
    Author:  Copyright © 1993 Shaun Blackmore
    Version: 1.02 (17 Apr 2005)
    Purpose: Assembler veneers for the RISC OS 3 Filter Manager SWIs
*/

#ifndef __dl_filter_h
#define __dl_filter_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header defines some assembler veneers for the RISC OS
  Filter Manager SWIs.
*/


typedef int (*filter_handler)(int event, const event_pollblock *eventdata,
                              const int task);
/*
  This is the type of routine that should be registered to be
  called on a filter event.  It also receives a value in R12 which
  was set at the time the filter was registered.
*/


extern void Filter_RegisterPreFilter(char *filtername,
                                     filter_handler handler, int R12,
                                     task_handle task);
/*
  Registers the 'handler' routine to be called whenever the task with
  handle 'task' polls the Wimp.  If 'task' is zero, then all tasks have
  the filter applied.  'filtername' is name for your filter.

  'R12' holds a value that will be passed to the handler routine in R12.

  A Pre-filter may clear bits in R0 to provide a new event mask for the
  Wimp_Poll.  It must preserve all other registers.
*/


extern void Filter_RegisterPostFilter(char *filtername,
                                      filter_handler handler, int R12,
                                      task_handle task, int eventmask);
/*
  Registers the 'handler' routine to be called whenever the Wimp is about
  to return from Wimp_Poll to the task with handle 'task'.  If 'task' is
  zero, then all tasks have the filter applied.  'filtername' is name for
  your filter.

  The 'eventmask' is like an event mask for Wimp_Poll: 1 bit masks the
  event out.

  'R12' holds a value that will be passed to the handler routine in R12.

  A post-filter may modify to reason code in R0 and the contents of the
  buffer pointed to by R1 to provide a new event for the filtered task.
  If it sets R0 to -1 on exit, the event will not be passed on to the
  task.  It must preserve R1 and R2.
*/


extern void Filter_DeRegisterPreFilter(char *filtername,
                                       filter_handler handler, int R12,
                                       task_handle task);
/*
  This removes a pre-filter from the list of pre-filters.  It should be
  called with the same parameters as were used to register the filter
  with Filter_RegisterPreFilter.
*/


extern void Filter_DeRegisterPostFilter(char *filtername,
                                        filter_handler handler, int R12,
                                        task_handle task, int eventmask);
/*
  This removes a pre-filter from the list of post-filters.  It should be
  called with the same parameters as were used to register the filter
  with Filter_RegisterPostFilter.
*/


#ifdef __cplusplus
}
#endif


#endif
