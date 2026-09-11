/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    File.h
    Author:  Copyright © 1993, 1994 Jason Williams, Tim Browse,
                                    Julian Smith, Sergio Monesi,
                                    Jason Howat.
    Version: 1.05 (17 Apr 2005)
    Purpose: Low level file operations, much faster than stdio equivalents,
             and more convenient when interacting with other code that uses
             RISC OS file handles to refer to files.
             Also sundry useful macros and functions to make file
             handling a bit easier.
    Mods:    22 Jul 1995 - JS - Added File_AllocLoad0().
             01 Aug 1995 - Sergio Monesi - Added File_ReadExtent()

    Mods:    31 Aug 1995 - JH - a few new filetypes.
*/


#ifndef __dl_file_h
#define __dl_file_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_swi_h
#include "DeskLib:SWI.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header defines a number of functions for handling files.  These
  functions use RISC OS file handles, and may be quicker than the
  standard C file access functions (though obviously less portable).

  It also defines a load of macros for various filetypes to save you
  having to make your own.
*/


extern os_error *file_lasterror;
/*
  file_lasterror is used to provide a more convenient SWI veneer for
  some of the file operations by returning handles or values directly.
  This leaves no convenient way to return error details from these functions.

  file_lasterror is set by many of the functions, but as it is just a pointer
  to the error returned by the SWI it *must* by used immediately after
  the call or else it may be invalid.  For instance, if you wish to close
  a file before returning the error, you must take a copy of the error
  first.

  If no error occurred on the last file operation, file_lasterror will be
  NULL.  Most of the functions which set it will return another value which
  will indicate an error at which point you can check file_lasterror for
  more information.  Some of the functions return the error, but still set
  file_lasterror for convenience.
*/


#define File_Copy(srce, dest, flags) \
  SWI(4, 0, SWI_OS_FSControl, 26, srce, dest, flags)
/*
  MACRO: os_error *File_Copy(char *source, char *dest, int flags);

  Copies a filer object from the source location to the destination location.
  The source filename can contain normal OS wildcards.  The destination may
  only contain '*' meaning "take destination leafname from source leafname".

  There are 14 flag bits to define the exact action - full details are in
  the PRMs.  Some of the more useful ones are bit 0 to enable recursive
  copying; bit 1 to force overwriting of of existing files; bit 3 to ask
  for confirmation before copying; bit 7 to delete the source files after
  copying them, to move files between filing systems; bit 12 to only copy
  if the source is newer than the destination.
*/


#define File_CreateDir(pathname) \
  SWI(5, 0, SWI_OS_File, 8, (pathname), NULL, NULL, 0)
/*
  MACRO: os_error *File_CreateDir(char *filename);

  Creates a directory of the given name.  It is not an error if the directory
  already exists.
*/


extern os_error *File_Delete(const char *filename);
/*
  Attempts to delete the named file. Use carefully!
*/


extern int File_Size(const char *filename);
/*
  Reads the size (in bytes) of the file. A return value of -1 indicates
  that an error occurred (which usually means that the file doesn't exist).
*/


extern BOOL File_Exists(const char *filename);
/*
  This returns TRUE if the given filename is a file of some kind.
  This includes the file being an "image file".
*/


typedef int file_position;
/* This is a type capable of holding a sequential file pointer. */

typedef int file_handle;
/* This is a type capable of holding an OS handle for an open file. */

#define file_READERROR (-1)
/* Used to indicate an error while reading from a file */


typedef enum
{
  file_READ   = 0x40,          /* Open the file for reading                 */
  file_WRITE  = 0x80,          /* CREATE/WIPE the file, open it for writing */
  file_APPEND = 0xC0           /* Open an existing file for reading and
                                  writing. */
} file_access;
/*
  These are used when opening files.

  file_READ opens for reading.  file_WRITE opens for writing, creating a new
  file or wiping an old one as necessary.  file_APPEND opens an existing
  file for read/write access.

  All modes open with the file pointer (position in the file) set to zero.
*/


extern file_handle File_Open(const char *filename, file_access access);
/*
  This attempts to open the given file for reading and/or writing
  depending on the 'access' asked for.

  It returns the file handle, or NULL if it cannot open the file.

  This sets file_lasterror, but in this case there may be no error message
  generated even if the file couldn't be opened, so check that it isn't
  NULL before using it to report an error.
*/


extern os_error *File_Close(file_handle handle);
/*
  Closes an open file stream, using the handle returned by File_Open.
  After a close, the file_handle is no longer valid, so do not try
  to use it.

  This function sets file_lasterror.
*/


extern BOOL File_EOF(file_handle handle);
/*
  Returns TRUE if the end of the file has been reached.

  This function sets file_lasterror.
*/


extern os_error *File_Seek(file_handle handle, file_position position);
/*
  Seeks the file pointer to the given position (an offset in terms of
  bytes offset from the start of the file). Subsequent read/write
  operations will read from or write to that part of the file.

  This function sets file_lasterror.
*/


extern file_position File_ReturnPos(file_handle handle);
/*
  Returns the current file pointer position.
  This is a value which, when passed to File_Seek, will return you to the
  same position you were at when you called File_ReturnPos.

  This function sets file_lasterror.
*/


extern os_error *File_WriteBytes(file_handle handle,
                                 const void *buffer, int numbytes);
/*
  This writes a chunk of data to the current position in a file opened
  for writing. 'numbytes' bytes will be written from the given buffer.
  It is important to check for errors from this call as Disc Full errors
  are a common occurrence.

  This function sets file_lasterror.
*/


extern int File_ReadBytes(file_handle handle, void *buffer, int numbytes);
/*
  Reads the given number of bytes of data from the given file, into the
  given buffer. Returns the number of bytes NOT successfully read.
  (i.e. if you hit EOF during the read, the return value will be non-zero)

  This function sets file_lasterror.
*/


extern os_error *File_Write8(file_handle handle, int byte);
/*
  Writes an 8-bit value (a byte/char) to the given output file.

  This function sets file_lasterror.
*/


#define File_WriteByte File_Write8
/*
  Simply an alias for File_Write8.
*/


#define File_WriteChar File_Write8
/*
  Simply an alias for File_Write8.
*/


extern int File_Read8(file_handle handle);
/*
  Reads an 8-bit value (a byte/char) from the given input file.
  If an error (typically EOF) occurs during the read, file_READERROR
  will be returned.  (Note that this is a perfectly legal value to read
  from the file, though, so you should check file_lasterror to see if
  an error occurred).

  VERY IMPORTANT NOTE!

  DO NOT use File_Read8() in a line like the following:

  shortvalue = File_Read8(infile) | (File_Read8(infile) << 8);

  The problem here is that the C compiler is allowed to evaluate the 2
  functions in any order it sees fit, so may read the two bytes in the
  incorrect order. You MUST therefore do such an operation with code
  in the following form if you are to sure it will work:

  shortvalue = File_Read8(infile);
  shortvalue |= File_Read8(infile) << 8;

  Of course, you should be checking for errors, so it is probably a bad
  idea anyway.
*/


#define File_ReadByte File_Read8
/* Alias for File_Read8. */


#define File_ReadChar File_Read8
/* Alias for File_Read8. */


extern os_error *File_Write32(file_handle handle, int word);
/*
  Writes an 32-bit value (a word/int/long) to the given output file.

  This function sets file_lasterror.
*/


#define File_WriteWord File_Write32
/* Alias for File_Write32. */


#define File_WriteInt  File_Write32
/* Alias for File_Write32. */


#define File_WriteLong File_Write32
/* Alias for File_Write32. */


extern int File_Read32(file_handle handle);
/*
  Reads a 32-bit value (a word/int/long) from the given input file.
  If an error (typically EOF) occurs during the read, file_READERROR
  will be returned. (Note that this is a perfectly legal value to read
  from the file, though, so you should check file_lasterror to see if
  an error occurred).
*/


#define File_ReadWord File_Read32
/* Alias for File_Read32. */


#define File_ReadInt  File_Read32
/* Alias for File_Read32. */


#define File_ReadLong File_Read32
/* Alias for File_Read32. */


extern int File_Read32R(file_handle handle);
/*
  Identical to File_Read32, but reverses the byte ordering as it reads
  (converts from little endian to big endian format). This is very useful
  for reading textual tags from files, as they can then be compared to
  integer constants of the form 'aTag' rather than having to reverse the
  byte order manually (i.e. using the tag 'gaTa' which is less human
  readable).

  This function sets file_lasterror.
*/


#define File_ReadWordR File_Read32R
/* Alias for File_Read32R. */


#define File_ReadIntR  File_Read32R
/* Alias for File_Read32R. */


#define File_ReadLongR File_Read32R
/* Alias for File_Read32R. */


extern os_error *File_Write32R(file_handle handle, int word);
/*
  Same as Write32, but reverses the byte order before writing. See
  File_Read32R for more details about why you'd want to do this.

  This function sets file_lasterror.
*/

#define File_WriteWordR File_Write32R
/* Alias for File_Write32R. */


#define File_WriteIntR  File_Write32R
/* Alias for File_Write32R. */


#define File_WriteLongR File_Write32R
/* Alias for File_Write32R. */


extern void File_SetType(const char *filename, int type);
/* Attempts to change the filetype of the named file. */


extern int File_GetType(const char *filename);
/* Returns the filetype of the named file, or -1 if there is an error. */


#define File_GetLength(filename, size_ptr) \
  SWI(2, 5, SWI_OS_File, 5, (filename), NULL, NULL, NULL, NULL, (size_ptr))
/*
  MACRO: os_error *File_GetLength(char *filename, int *size_ptr);

  This gets the size of the given file and places it into size_ptr.

  It differs from File_Size in that it returns a standard error block if
  the size cannot be read for some reason.
*/


extern BOOL File_IsDirectory(const char *pathname);
/*
  This checks if the given file is in fact a directory.  Note that
  in this case, the definition of a directory includes an image file
  as it can be treated as a directory.
*/


extern os_error *File_LoadTo(const char *filename, void *address, int *size);
/*
  This function loads the file whose filename is given into memory at the
  address specified.  If 'size' is not NULL, the file size will be returned in
  *size.
*/


extern void File_Date(const char *filename, unsigned char *fivebytedate);
/*
  This reads the datestamp of a file into a five byte block suitable for
  use with, for example, Time_ConvertDateAndTime.
*/


extern char *File_AllocLoad0(const char *filename);
/*
  This allocates space for the file with malloc, loads it, and appends a '\0'.
  The intention is to allow text-files to be loaded and dealt with as strings.

  Returns a pointer to the loaded file, or NULL if an error occurred.
*/


extern int File_printf(file_handle file, const char *format, ...);
/*
  This is equivalent to fprintf, but taking a RISC OS file handle instead
  of a C stream.  It prints a string to the open file whose handle is 'file'
  under the control of the printf-style format string format.

  It returns the number of characters written or a -ve number for an error.

  Note: This uses a 512 byte buffer to compose the string to be output.
  If your string overflows this,  your program will crash or else have even
  nastier side-effects.
*/


extern int File_ReadExtent(file_handle handle);
/*
  Returns the current file extent (or -1 if an error occurrs)

  This function sets file_lasterror.
*/


#define FILETYPE(x) (((x) & 0xFFF00) >> 8)
/*
  MACRO: int FILETYPE(int loadaddr);

  This simply returns a filetype from a load address.  Note that for
  the filetype to be valid, the top twelve bits of the load address
  must be set.  ie. (loadaddr & 0xFFF00000) == 0xFFF00000.
*/


/*
 * #defines for some common filetypes.
 */

#define filetype_AIM        0x004
#define filetype_CLEAR      0x690
#define filetype_DEGAS      0x691
#define filetype_IMG        0x692
#define filetype_AMIGAIFF   0x693
#define filetype_MACPAINT   0x694
#define filetype_GIF        0x695
#define filetype_PCX        0x697
#define filetype_QRT        0x698
#define filetype_MTV        0x699
#define filetype_CADSOFT    0x69A
#define filetype_IRLAM      0x69B
#define filetype_BMP        0x69C
#define filetype_TARGA      0x69D
#define filetype_PBMPlus    0x69E
#define filetype_ZVDA       0x69F
#define filetype_MSX2       0x6A0
#define filetype_RLE        0x6A1
#define filetype_COLORIX    0x6A2
#define filetype_FITS       0x6A3
#define filetype_HAWKV9     0x6A4
#define filetype_REPLAY     0xAE7
#define filetype_ALARMS     0xAE9
#define filetype_DRAWFILE   0xAFF
#define filetype_BBCROM     0xBBC
#define filetype_AUDIOWRK   0xBD6
#define filetype_RENDPIC    0xD58
#define filetype_ARCHIVE    0xDDC
#define filetype_PROART     0xDE2
#define filetype_PICTURE    0xDFA
#define filetype_HTML       0xFAF
#define filetype_PRNTDEFN   0xFC6
#define filetype_DOSDISC    0xFC8
#define filetype_SUNRASTR   0xFC9
#define filetype_DEVICE     0xFCC
#define filetype_CACHE      0xFCF
#define filetype_PCEMCONF   0xFD0
#define filetype_DEBIMAGE   0xFD3
#define filetype_MIDI       0xFD4
#define filetype_TASKEXEC   0xFD6
#define filetype_TASKOBEY   0xFD7
#define filetype_MAKEFILE   0xFE1
#define filetype_DOS        0xFE4
#define filetype_DESKTOP    0xFEA
#define filetype_OBEY       0xFEB
#define filetype_TEMPLATE   0xFEC
#define filetype_PALETTE    0xFED
#define filetype_TIFF       0xFF0
#define filetype_CONFIG     0xFF2
#define filetype_PRINTOUT   0xFF4
#define filetype_POSCRIPT   0xFF5
#define filetype_FONT       0xFF6
#define filetype_BBCFONT    0xFF7
#define filetype_ABSOLUTE   0xFF8
#define filetype_SPRITE     0xFF9
#define filetype_MODULE     0xFFA
#define filetype_BASIC      0xFFB
#define filetype_UTILITY    0xFFC
#define filetype_DATA       0xFFD
#define filetype_COMMAND    0xFFE
#define filetype_TEXT       0xFFF

#ifdef __cplusplus
}
#endif

#endif
