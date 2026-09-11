/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Dispatch.h  (for Misc.s.Dispatch)
    Author:  Copyright © 1992 Shaun Blackmore
    Version: 1.00 (03 Apr 1992)
    Purpose: Function dispatcher
*/

#ifndef __dl_dispatch_h
#define __dl_dispatch_h

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header declares a function for dispatching other functions - calling
  a given function for a given value of a variable.
*/

typedef void (*dispatch_proc)();
/*
  This type holds a pointer to a function to be called by the Dispatch function.
  Functions can have up to four arguments.
*/


extern int Dispatch(int value, int size, int table[],
                    dispatch_proc functions[],
                    int arg0, int arg1, int arg2, int arg3);
/*
  This function looks up 'value' in table[size], and calls the corresponding
  function in functions[size] with up to four arguments (any excess arguments
  will be safely ignored).

  If one of the elements in table has the value -1, it is always called, so
  if you want a default action, make it the last in the table.
*/


#ifdef __cplusplus
}
#endif


#endif
