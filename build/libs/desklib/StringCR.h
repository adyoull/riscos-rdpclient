/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    stringcr.h
    Author:  Copyright © 1992, 1993 Jason Williams
    Version: 1.13 (30 May 1993)
    Purpose: Provides functions identical in operation to those defined in
             ANSI <string.h>, except that these functions all work with strings
             that have non-zero terminating characters (BASIC and the WIMP
             often use a char 13 instead of a char 0 to terminate their
             strings). ANY char < 32 will terminate these strings.
             These functions are currently defined in Sublibrary Misc.c.*
*/

#ifndef __dl_stringcr_h
#define __dl_stringcr_h

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  Compatibility header.
  It simply includes the header for the "Str" sub-library.
*/


#include "DeskLib:Str.h"

#ifdef __cplusplus
}
#endif


#endif
