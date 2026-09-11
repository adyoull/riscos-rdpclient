/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    TextFile.h
    Author:  Copyright © 1992 Jason Williams
    Version: 1.01 (17 Apr 2005)
    Purpose: Generic textfile-handling routines
*/


#ifndef __dl_textfile_h
#define __dl_textfile_h

#include <stdio.h>

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header defines routines which may be useful when dealing with
  text files.  Where these functions take a FILE variable, the file
  should have been opened by fopen in text mode, not binary mode.
*/


#define TextFile_Lowercase(x) (((x)>='A' && (x)<='Z') ? (x)+32 : (x))
/*
  Macro: char Textfile_Lowercase(char ch);

  This takes a character and returns the lowercase equivalent.
  There are only lowercase equivalents for alphabetic characters.
*/


extern void TextFile_SkipBlanks(FILE *infile);
/*
  This reads characters from the given file until the end of the file is
  reached or until a non-blank character is found.  Blank characters are
  spaces, tabs and newlines.
*/


extern void TextFile_ReadToDelimiter(FILE *infile, char delimiter,
                                     char *line, int maxlength);
/*
  This reads characters from the given file until it reaches the end of
  the file or the given delimeter character is read.  The delimiter is
  typically a newline character to read a line at a time.  Leading
  spaces or tabs are ignored.

  It will read no more than 'maxlength'-1 characters from the file, and
  zero-terminates the buffer automatically.  The delimeter itself is not
  written to the buffer.
*/


#ifdef __cplusplus
}
#endif

#endif
