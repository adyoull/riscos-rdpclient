/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    MsgTrans.h
    Author:  Copyright © 1994 Tony Houghton
    Version: 1.03 (17 Apr 2005)
    Purpose: Interface to some of the most useful MessageTrans routines

    NOTE: This is a veneer for the OS 'MsgTrans' system. An analogue of
          this system is provided by the 'Msgs' sublibrary (Msgs.h).
          Note that each system has advantages and disavantages compared
          to the other...
*/

#ifndef __dl_msgtrans_h
#define __dl_msgtrans_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header declares functions for accessing the MessageTrans SWIs.
  It declares MessageTrans_ functions, which are veneers to some of the
  module SWIs, and MsgTrans_ functions which provide a higher level
  interface in a way likely to be of use in a desktop application.

  The file format consists of a series of lines of the form
  "Token:Text message with parameters %0 to %3".  You can specify more
  than one token for a message by separating the alternative tokens with
  '/' or LF, eg. "Token1/Token2:Message".  You can put '?' in a token,
  and it will then match any character when you look up the message.
  eg. "gr?y:The colour grey" would be found by the tokens "grey", "gray",
  and a good many more.

  Lines beginning with '#' are comments.  If you wish to use a percent sign
  in your message you should use "%%" because the percent symbol is used to
  mark parameter substitutions.
*/


typedef struct
{
  int data[4];
} msgtrans_filedesc;
/*
  This is a file descriptor block for use with the MessageTrans SWIs.
  This shouldn't be accessed directly by the user, only passed to
  and returned from various SWIs intact.
*/


extern os_error *MessageTrans_FileInfo(const char *filename, int *flags,
                                       int *size);
/*
  This gets information about a Messages file prior to opening it.
  You pass a pointer to the filename, and it returns the size of buffer
  required to hold the file and sets bit 1 of 'flags' if the file is
  held in memory.  This means that if you require read-only access you
  need not use a buffer to access it.

  It returns NULL if successful, an error block otherwise.
*/


extern os_error *MessageTrans_OpenFile(msgtrans_filedesc *filedesc,
                                       const char *filename, char *buffer);
/*
  This opens a messages file ready for reading.  You pass a pointer to a
  file descriptor block, the file path name of the file you wish to open
  and a pointer to the buffer to hold the file.

  If 'buffer' is NULL, then both the file descriptor block and the filename
  should be stored in RMA, and the module will allocate space in the RMA
  as needed to hold the file.  Otherwise, the buffer should be at least the
  size returned by MessageTrans_FileInfo to be large enough to hold the file.

  It returns NULL if sucessful, or an error block.  One error you might
  get is "Message file already open" if the descriptor is in use.
*/


extern os_error *MessageTrans_Lookup(msgtrans_filedesc *filedesc,
                                     const char *token,
                                     char **buffer, int *size,
                                     const char *p0,
                                     const char *p1,
                                     const char *p2,
                                     const char *p3);
/*
  This looks up the given token in the given open messages file, and returns
  the string in the buffer pointed to by 'buffer' (unless 'buffer' is NULL
  in which case the string isn't copied), whose size is given by 'size'.
  You can pass up to four strings to be substituted in place of %0, %1, etc.
  in the looked up message, or NULL if there is no substitution.

  It updates 'buffer' to point to the result string, and 'size' to give
  the size of the resulting string (not including the terminator).  If you
  didn't pass in a buffer, no substitution can be done in the message, and
  you get a *read-only* pointer to the message in question.

  If you give zero as the 'filedesc', the message will be looked up in the
  global messages (held in Resources:$.Resources.Global.Messages).

  You can supply a token in the form "Token:Default string" and if the
  token isn't found in the file the default string is used instead.

  This function returns NULL if there are no errors.
*/


extern os_error *MessageTrans_CloseFile(msgtrans_filedesc *filedesc);
/*
  This closes a message file opened with MessageTrans_OpenFile, you
  simply pass it the file descriptor block you received from that function.

  This function returns NULL if there are no errors.
*/


extern os_error *MsgTrans_LoadFile(msgtrans_filedesc **filedesc,
                                   const char *filename);
/*
  This function allocates space (using malloc) for the given message
  file and loads it.  If fills 'filedesc' with a pointer to a MessageTrans
  file descriptor block.

  This function returns NULL if there are no errors.
*/


#define MsgTrans_Lookup(filedesc, token, buffer, bufflen) \
  MsgTrans_LookupPS((filedesc), (token), (buffer), (bufflen), \
                    NULL, NULL, NULL, NULL)
/*
  MACRO: extern os_error *MsgTrans_Lookup(msgtrans_filedesc *filedesc, char *token, char *buffer, int bufflen);

  This looks up the given token in the given file, and copies the matched
  token's message to the given buffer (of length 'bufflen') and ensures that
  it is properly '\0' terminated.  This doesn't allow parameter substitution.

  You can supply a token in the form "Token:Default string" and if the
  token isn't found in the file the default string is used instead.

  This function returns NULL if there are no errors.
*/


extern os_error *MsgTrans_LookupPS(msgtrans_filedesc *filedesc,
                                   const char *token,
                                   char *buffer, int bufflen,
                                   const char *p0,
                                   const char *p1,
                                   const char *p2,
                                   const char *p3);
/*
  This looks up the given token in the given file, and copies the matched
  token's message to the given buffer (of length 'bufflen') and ensure that
  it is properly '\0' terminated.  This allows up to four parameters to be
  subsituted with the strings passed in p0..p3.  If you do not wish to
  substitute any of these, pass NULL instead of a string.

  You can supply a token in the form "Token:Default string" and if the
  token isn't found in the file the default string is used instead.

  This function returns NULL if there are no errors.
*/


extern os_error *MsgTrans_LoseFile(msgtrans_filedesc *filedesc);
/*
  This closes a message file opened with MsgTrans_LoadFile, and frees the
  memory occupied by that file.  You should pass the file descriptor returned
  by that earlier call.

  This function returns NULL if there are no errors.
*/


#define MsgTrans_Report(filedesc, token, fatal) \
  MsgTrans_ReportPS((filedesc),(token),(fatal),0,0,0,0)
/*
  MACRO: extern os_error *MsgTrans_Report(msgtrans_filedesc *filedesc,
                                          char *token, BOOL fatal);

  This looks up the given token in the given message file, copies it
  into an internal buffer and passes it to Error_Report if 'fatal' is FALSE
  or Error_ReportFatal if 'fatal' is TRUE.

  You can supply a token in the form "Token:Default string" and if the
  token isn't found in the file the default string is used instead.

  This function returns NULL if it is successful.
*/


extern os_error *MsgTrans_ReportPS(msgtrans_filedesc *filedesc, char *token,
                                   BOOL fatal,
                                   const char *p0,
                                   const char *p1,
                                   const char *p2,
                                   const char *p3);
/*

  This looks up the given token in the given message file, copies it
  into an internal buffer and passes it to Error_Report if 'fatal' is FALSE
  or Error_ReportFatal if 'fatal' is TRUE.  This allows subsitution of
  up to four parameters with the strings in p0..p3.   If you do not wish to
  substitute any of these, pass NULL instead of a string.

  You can supply a token in the form "Token:Default string" and if the
  token isn't found in the file the default string is used instead.

  This function returns NULL if it is successful.
*/


#ifdef __cplusplus
}
#endif


#endif
