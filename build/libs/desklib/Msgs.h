/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Msgs.h
    Author:  Copyright © 1992, 1993 Jason Williams
    Version: 1.02 (17 Apr 2005)
    Purpose: MessageTrans-like message handling functions.
             (If you want MessageTrans, use the SWI interface, if you want
             high-level message handling, use this code...)
*/

#ifndef __dl_msgs_h
#define __dl_msgs_h


#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header declares functions to read in token - text pairs from
  files and then lookup the text, like you do with MessageTrans.
  This is intended to aid, for instance, internationalisation of your
  program, so all error messages, menu text, etc. if in a text file or
  files rather than being compiled into the program.

  These are much simpler than the equivalent MessageTrans SWIs (which
  MsgTrans provides an interface for).  There are also various other
  library functions which allow you to directly supply Msgs tokens
  in place of strings (eg. Menu2_CreateFromMsgs).

  Not that for the system to work properly all tags should be in the form
  "group.tag" not just "tag" (they should all have two parts), and neither
  the 'group' nor 'tag' part should be longer than 8 characters.
*/


extern BOOL Msgs_Lookup(const char *tag, char *result, int maxlength);
/*
  This looks up the given tag in the current message list, and copies
  the corresponding message (if it exists) into 'result'.  No more than
  'maxlength' characters will be copied, though a terminator may be added
  in result[maxlength].  The tag string should be in the form
  "group.tag:default message" or "group.tag".  The default message (if
  given) will be used if the tag cannot be found.

  The message found will be searched for other messages included with "<tag>"
  includes.  Msgs_Lookup will be recursively invoked in order to compile
  the final string, if any such includes are found.  Note that circular
  references are not detected and so the recursion can be infinite (or at
  least until your program crashes).  However, the recursion *will* stop
  when there is no more space in the buffer, so it is only really infinite
  if a message does nothing except include itself.  Remember that messages
  which include others will need more memory than is immediately obvious.

  This function returns TRUE if the tag is found, or if a default message
  was supplied and used.  Otherwise, FALSE is returned, and result[0] is
  set to '\0' (the result string is empty).
*/


extern BOOL Msgs_printf(char *result, const char *formattag, ...);
/*
  The works like sprintf, but using a format string looked up from the
  loaded list of messages.  You simply pass in a message tag where you
  would supply a format string.

  This function returns TRUE if the tag is found, or if a default message
  was supplied and used.  Otherwise, FALSE is returned, and result[0] is
  set to '\0' (the result string is empty).
*/


extern BOOL Msgs_LoadFile(const char *leafname);
/*
  The loads the given messages file into your list of messages.  The
  file is found using resource_pathname and the given leafname.  This
  merges the loaded file with the current list, overwriting messages
  which had the same tag and leaving the others alone.

  The file should be made up of a series of "group.msg: message text"
  lines.  The "group" and "msg" parts are both limited to a maximum
  eight characters.  Comments can be included if the
  line starts with #, and messages can include other messages by putting
  the tag inside angle brackets ('<' and '>').  This means that in order
  to write '<' inside a messages file, you should use "<>" in its place.

  You should leave a couple of return characters at the end of the file,
  as otherwise the last message may be lost.
*/


extern void Msgs_DropGroup(const char *grouptag);
/*
  This deletes the given message group from memory.  This is useful for
  regaining memory when you know a given group of messages is no longer
  needed, for instance you have created the menus that the text was
  needed for.

  You cannot reload a single group of messages.  If you want to get the
  dropped messages back you have to reload the file they were read from
  originally.
*/


extern void Msgs_Report(int errnum, const char *tag, ...);
/*
  This calls Error_Report with the error message format string supplied
  by looking up the given tag in the current list of messages.
*/


extern void Msgs_ReportFatal(int errnum, const char *tag, ...);
/*
  This calls Error_ReportFatal with the error message format string
  supplied by looking up the given tag in the current list of messages.
*/


#ifdef __cplusplus
}
#endif

#endif
