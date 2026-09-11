/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Clear.h
    Author:  Copyright © 1994 Jason Howat
    Version: 1.02 (17 Apr 2005)
    Purpose: Clear file manipulation routines.
*/


#ifndef __dl_clear_h
#define __dl_clear_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header declares functions for handling Clear files.

  Note that these functions use the Mem sublibrary to allocate memory
  for the files themselves, so you must call Mem_Initialise before
  using them.
 */

typedef struct
{
  const char   *creator;
  unsigned      creatorversion;

  unsigned      width,
                height;
  unsigned      bpp;

  palette_entry *palette;
  unsigned char *bitmap;
} clear_picture;
/*
  A structure to hold all the relevant information about a Clear file.
*/

void Clear_Init(const char *creator, unsigned version);
/*
  Initialises the Clear library with details of the Clear file creating
  program -- only needed if you use Clear_Save().  'creator' is the name
  of the program, and is *not* copied so should exist for as long as you
  are using the library.  'version' is the version number of the
  program * 100.

  e.g. Clear_Init("testprog", 100);
*/


clear_picture *Clear_Load(const char *filename);
/*
  Attempt to read a picture from the given Clear file.  Returns NULL if
  there is an error.
*/


void Clear_Save(clear_picture *picture, const char *filename);
/*
  Write the given picture to a file.
*/


clear_picture *Clear_Create(unsigned width, unsigned height, unsigned bpp);
/*
  Allocate memory for a picture of the given dimensions.  Returns NULL if
  it fails.
*/


void Clear_Free(clear_picture *picture);
/*
  Deallocate all memory used by the given picture.
*/


#ifdef __cplusplus
}
#endif


#endif
