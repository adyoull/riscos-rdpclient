/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Validation.h
    Author:  Copyright © 1992 Jason Williams
    Version: 1.00 (22 Mar 1992)
    Purpose: Scanning of validation strings for various fields
*/


#ifndef __dl_validation_h
#define __dl_validation_h

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header provides a function to extract information from an icon's
  validation string.
*/

extern int Validation_ScanString(const char *string, char tag);
/*
  This scans the given string for the tag character given, and returns the
  index of the *next* character, or zero if the tag character is not found.
  The tag must be either the first character or directly follow a
  semi-colon.

  For instance, if you search for the tag 'r' in "sicon;r5", it will return
  the index of '5' (7, in this example).
*/


#ifdef __cplusplus
}
#endif


#endif
