/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Clipboard.h
    Author:  Copyright © 2005 Peter Naulls
    Version: 1.00 (5th July 2005)
    Purpose: RISC OS Clipboard manipulation
*/


#ifndef __dl_clipboard_h
#define __dl_clipboard_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header declares functions for manipulation of the RISC OS
  clipboard.

  Note: This module is incomplete.
*/


void Clipboard_Claim(BOOL Selection, BOOL Clipboard);

typedef void (*paste_handler) (message_block *message);

void Clipboard_Request(window_handle window, unsigned int internal, BOOL send,
                       paste_handler *paste, int *filetypes);



#ifdef __cplusplus
}
#endif


#endif
