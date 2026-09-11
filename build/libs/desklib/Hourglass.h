/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Hourglass.h
    Author:  Copyright © 1993 Jason Howat
    Version: 2.00 (24 Jul 1993)
    Purpose: Hourglass module veneers
*/


#ifndef __dl_hourglass_h
#define __dl_hourglass_h

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This declares function veneers for the Hourglass module.
*/

extern void Hourglass_On(void);
/*
  Changes the mouse pointer to show the user that some delay will occur.
*/


extern void Hourglass_Start(int startdelay);
/*
  Changes the mouse pointer to show the user that some delay will occur.

  The change will occur 'startdelay' centiseconds after the call, so if you
  only want to show the hourglass if the action takes more than half a
  second, you can call this with startdelay set to 50 and the user won't
  see the hourglass if you call Hourglass_Off in that half-second.
*/


extern void Hourglass_Percentage(int percent);
/*
  This indicates to the user that the delay is 'percent'% completed.

  Don't wind people up by initially setting it to 99 and then waiting
  half an hour.
*/


extern void Hourglass_Off(void);
/*
  This turns off the hourglass, if it was set on.  You need one Hourglass_Off
  call for each Hourglass_On.
*/


extern void Hourglass_Smash(void);
/*
  This forces the indication of a delay to be removed.

  Hourglass_Off must be called for each Hourglass_On or
  Hourglass_Start call made - this will turn it off no matter
  how many calls have been made to turn it on.
*/


#ifdef __cplusplus
}
#endif

#endif
