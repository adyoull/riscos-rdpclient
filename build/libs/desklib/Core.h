/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Core.h
    Author:  Copyright © 1992, 1993, 1994 Jason Williams and Tim Browse
    Version: 1.02 (17 Apr 2005)
    Purpose: Core definitions used by most of DeskLib
    Mods:    20 Mar 1995 Julian Smith
             removed OSCLI if _desklib_DLL is defined.
             15 Jul 1995 Sergio Monesi
             removed OSCLI since OS_CLI has been added to KernelSWIs.h
             25 Jul 1995 Julian Smith
             Now #includes <stddef.h> rather than #defining NULL
*/


#ifndef __dl_core_h
#define __dl_core_h

#include <stddef.h>
/* For NULL, size_t etc */

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header file contains core definitions used by most of DeskLib.
*/

/* --- Boolean values */
#ifndef BOOL
#define BOOL  unsigned

#define FALSE 0
#define TRUE  1

#define ERROR   1
#define NOERROR 0
#endif


/*
  Operating system errors (== _kernel_oserror).
  Conditional compilation to avoid clashes with EasyC's roslib and
  Acorn's "os.h".
*/
#ifndef __roslib_h
  #ifndef __os_h
    typedef struct
    {
      int  errnum;
      char errmess[252];
    } os_error;
    /* The structure of operating system errors. */
  #endif
#endif


#define UNUSED_ARG(x) ((x) = (x))
/*
  A simple macro to avoid compiler warnings when you don't actually use
  one of the parameters passed to a particular function.

  Also useful for conditional compilation where one of the paths doesn't
  use a variable.
*/

#define UNUSED(x) ((x) = (x))
/*
  A simple macro to avoid compiler warnings when you don't actually use
  one of the parameters passed to a particular function.

  Also useful for conditional compilation where one of the paths doesn't
  use a variable.
*/

#define WORDALIGN(ADDR) (((ADDR) + 3) & (~3))
/*
 * A simple macro that rounds an address up to the nearest word boundary.
 */

#ifndef MAX

  #define MAX(x,y)   ((x) > (y) ? (x) : (y))
  #define MIN(x,y)   ((x) < (y) ? (x) : (y))
/*
  Purpose:  The usual definitions of MAX and MIN.
*/
#endif

#ifdef __cplusplus
}
#endif


#endif
