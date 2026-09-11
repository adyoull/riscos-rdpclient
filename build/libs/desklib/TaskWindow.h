/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    TaskWindow.h
    Author:  Copyright © 2003 Antony Sidwell
    Version: 1.01 (17 Apr 2005)
    Purpose: TaskWindow handling using Event/EventMsg
*/

#ifndef __dl_taskwindow_h
#define __dl_taskwindow_h

#include "DeskLib:Wimp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void tw_output_fn(char *data, size_t datalen, void *ref);
/*
  This type of function is called with any data output from the
  taskwindow task.  Note that the data is not (necessarily) a
  C string - it may contain embedded zeroes and is likely to
  be unterminated.

  It is also passed the "ref" registered with TaskWindow_Start,
  to enable you to identify the taskwindow task to which it
  refers.
*/



typedef void tw_morio_fn(void *ref);
/*
  This type of function is called when the taskwindow task
  quits, so that you can stop taking input, close a display, or
  whatever you feel like.

  It is passed the "ref" registered with TaskWindow_Start, to
  enable you to identify the taskwindow task which has quit.
*/



os_error *TaskWindow_Start(const char *command, const char *taskname,
                           size_t wimpslot_k, tw_output_fn output,
                           tw_morio_fn died, void *ref,
                           task_handle *handle);
/*
  Takes a command to start in a taskwindow, a name for that task and a
  wimpslot (or zero to not specify a wimpslot).  Wimpslot sizes are in
  kilobytes, not bytes.  If the task is started successfully (and 'handle'
  is not NULL), then the task handle of the started task is returned in
  handle.  If there is a problem, an error block is returned, otherwise
  it returns NULL.

  If there is any output from the task, it is passed to the "output"
  function for the parent task to process or display.  When the task
  exits, the "died" function is called, and all the handlers attached
  to events by TaskWindow_Start are released.

  "ref" is a value of your choosing, passed to both the output and the
  died function so that you can identify the task being executed.
*/



void TaskWindow_Kill(task_handle handle);
/*
  This sends a message to the specified taskwindow task in
  order to kill it.
*/



void TaskWindow_Resume(task_handle handle);
/*
  This sends a message telling a previously suspended taskwindow
  task to resume.
*/



void TaskWindow_SendInput(task_handle handle, const char *data, size_t datalen);
/*
  This sends "input" (i.e. keypresses) to the specified taskwindow task.
  The data need not be a C string.

  You can also send data using the Data Transfer Protocol - just send a
  DataSave message to the task, and then handle the resulting RAM transfer.
*/



void TaskWindow_Suspend(task_handle handle);
/*
  This sends a message to suspend the specified taskwindow task.
  It can be started up again using TaskWindow_Resume.
*/


int TaskWindow_Check(void);
/*
  Checks if the task is running in a TaskWindow.  It returns 0 if not in
  a taskwindow, and a non-zero value otherwise.
*/

#ifdef __cplusplus
}
#endif

#endif
