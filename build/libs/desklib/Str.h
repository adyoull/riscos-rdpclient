/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Str.h
    Author:  Copyright © 1992, 1993 Jason Williams
    Version: 1.13 (17 Apr 2005)
    Purpose: Useful string functions not provided by ANSI, including functions
             to deal with CR-terminated strings, as used by the RISC OS Wimp.
*/


/* Abstract */
/*
  This header provides string functions not provided by the ANSI/ISO standard
  libraries.  These include functions to deal with CR-terminated strings, as
  are sometimes used by the Wimp.
*/


#ifndef __dl_str_h
#define __dl_str_h

/* We need size_t from stddef.h */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


extern int strlencr(const char *s);
/*
  This is an exact equivalent of strlen, but recognising any character
  with an ASCII code below 32 (ie. any control code) as a terminator.
  This makes it suitable for use with CR-terminated strings as are
  sometimes used by RISC OS.
*/


extern int strcmpcr(const char *s1, const char *s2);
/*
  This is an exact equivalent of strcmp, but recognising any character
  with an ASCII code below 32 (ie. any control code) as a terminator.
  This makes it suitable for use with CR-terminated strings as are
  sometimes used by RISC OS.
*/


extern char *strcatcr(char *s1, const char *s2);
/*
  This is an exact equivalent of strcat, but recognising any character
  with an ASCII code below 32 (ie. any control code) as a terminator.
  This makes it suitable for use with CR-terminated strings as are
  sometimes used by RISC OS.

  The resulting string will be zero-terminated.
*/


extern char *strcpycr(char *s1, const char *s2);
/*
  This is an exact equivalent of strcpy, but recognising any character
  with an ASCII code below 32 (ie. any control code) as a terminator.
  This makes it suitable for use with CR-terminated strings as are
  sometimes used by RISC OS.

  The resulting string will be zero-terminated.
*/


extern char *strncpycr(char *s1, const char *s2, int n);
/*
  This is an exact equivalent of strncpy, but recognising any character
  with an ASCII code below 32 (ie. any control code) as a terminator.
  This makes it suitable for use with CR-terminated strings as are
  sometimes used by RISC OS.

  The resulting string will be zero-terminated if it is shorter than 'n'.
*/


extern char *Str_LeafName(const char *path);
/*
  This returns a pointer to the "leaf" part of the given filepath,
  ie. the first character after the last '.' in 'path'.  If there is
  no '.' in 'path', it returns a pointer to the start of 'path'.
*/

#define LeafName(str) Str_LeafName(str)
/*
  Macro: char *LeafName(char *path);

  This returns a pointer to the "leaf" part of the given filepath,
  ie. the first character after the last '.' in 'path'.  If there is
  no '.' in 'path', it returns a pointer to the start of 'path'.
*/


extern int stricmp(const char *s1, const char *s2);
/*
  This is a case-insensitive version of the strcmp function.
*/

#define strincmp(s1, s2, n) strnicmp(s1, s2, n)
/*
  This is an alternate name for "strnicmp", for those who prefer
  their 'i's before their 'n's.
*/

extern int strnicmp(const char *s1, const char *s2, size_t n);
/*
  This is a case-insensitive version of the strncmp function.
*/


extern int stricmpcr(const char *s1, const char *s2);
/*
  This is a case-insensitive strcmp which accepts any control character
  (character with ASCII code below 32) as a terminator, not just zero.
*/

#define strincmpcr(s1, s2, n) strnicmpcr(s1, s2, n)
extern int strnicmpcr(const char *s1, const char *s2, size_t n);
/*
  This is a case-insensitive strncmp which accepts any control character
  (character with ASCII code below 32) as a terminator, not just zero.
*/


extern char *strdup(const char *s);
/*
  This allocates memory (with malloc) for a copy of the given string, and
  copies it to the new block.  It returns a pointer to the new string, or
  NULL if it runs out of memory.
*/


extern void Str_MakeCR(char *s, int max_len);
/*
  This alters the given string to make it CR-terminated rather than zero
  terminated.  'maxlen' if the number of characters to look for the nul
  terminator before giving up (in which case it is terminated at the
  'max_len'th character).
*/


extern void Str_MakeASCIIZ(char *s, int max_len);
/*
  This alters the given string to make it zero-terminated rather than CR
  terminated.  'maxlen' if the number of characters to look for the CR
  terminator before giving up (in which case it is terminated at the
  'max_len'th character).
*/


extern char Str_MakeHex(int n);
/*
  This takes a number (which should be in the range 0 to 15) and converts
  it to the equivalent hexademical character ('0'-'F').  If 'n' is outside
  the convertable range, '?' is returned.
*/


extern int Str_DecodeHex(char digit);
/*
  This takes a hexadecimal digit (ie. '0'-'9', 'A'-'F') and returns the
  integer value it represents.  If the character is not a valid haxadecimal
  digit, -1 is returned.
*/


#ifdef __cplusplus
}
#endif

#endif
