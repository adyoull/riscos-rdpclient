/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Error.h
    Author:  Copyright © 1992 Jason Williams
             Philip Colmer: Varargs improvements.
             Julian Smith:  error_PLACE
             Sergio Monesi: error_FIXED, error_global (with some ideas and code from
                            Martin Ebourne)
    Version: 1.61 (17 Apr 2005)
    Purpose: Centralised error reporting, with hopefully useful routines,
             but which YOU (the user) can change if you want something
             different.
             Also, a few useful error-releated macros.
    Mods:    7 Apr 1992 - JCW - Added Error_OutOfMemory
            14 Jun 1993 - PJC - Allowed Error_Report(Fatal) to take
                                variable arguments
            13 Jul 1993 - PJC - Added varargs to "Internal" versions of above
            06 Jun 1995 - JPS - Added error_PLACE
            06 Jun 1995 - SM  - Added error_global and error_FIXED,
            06 Jun 1995 - JPS - made error_global DLL-friendly.
            08 Jul 1995 - JPS - Removed #include of kernel.h
            21 Jul 1995 - SM  - Added error_STATIC
*/

#ifndef __dl_error_h
#define __dl_error_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header provides centralised error reporting routines, and a few macros
  to help with error reporting.

  This idea is that if you want to replace these with your own error reporting
  routines you can just link against your set instead and keep the function
  prototypes the same.  An example of this is linking against
  Desklib:o.Other.SmError which provides command-line output of errors by
  writing them to stderr.
*/

extern void Error_ReportInternal(int errornum, const char *report, ...);
/*
  This is identical to Error_Report, except it is used for most DeskLib
  library internal errors. User code should use Error_Report. This allows
  you to modify the treatment of internal errors, without affecting your
  own error handling.
*/


extern void Error_ReportFatalInternal(int errornum, const char *report, ...);
/*
  This does an Error_ReportInternal, and then calls exit()
*/


extern void Error_Report(int errornum, const char *report, ...);
/*
  This is a centralised user-error reporting function. Call it, citing any
  error number you want, and any error string you want. The current
  implementation merely invokes Wimp_ReportError, but use it from your
  code, as then the method by which you report errors can be altered just
  by changing Error.c

  The report is a 'printf' style formatting string, optionally followed by
  arguments just as in printf commands. This saves you from having to
  pre-assemble strings to pass in.

  examples:

  Error_Report(5, "My VIDC just blew up!");

  Error_Report(6, "You can't put the number %d in there!", thenumber);
*/



extern void Error_ReportFatal(int errornum, const char *report, ...);
/*
  Exactly the same as Error_Report, except after reporting the error, it
  terminates the program by calling exit()

  Takes variable arguments a la 'printf'. See Error_Report for more info.
*/


extern BOOL Error_Check(os_error *error);
/*
  Used to encapsulate an OS call to automatically check for error return.
  If the OS call returns an error, then it will be reported, and the
  function returns TRUE. Otherwise, no action is taken, and FALSE is
  returned
*/


extern void Error_CheckFatal(os_error *error);
/*
  Identical to Error_Check, but calls exit() if an error ocurred
*/


extern BOOL Error_OutOfMemory(BOOL fatal, const char *place);
/*
  Reports a memory error (e.g. "Unable to claim enough memory for
  [Messages]"), where you supply just the 'place' of the failure
  (e.g. in this case, in the "Messages" section).

  If 'fatal' is TRUE, Error_ReportFatal is used, else Error_Report is used.

  This function ALWAYS returns FALSE (== 0 == NULL), so you can use:

  if (spritearea == NULL) return( Error_OutOfMemory(FALSE, "icon sprites"));
*/


#define error__SR(x)	error__VL(x)
/* Macro used in error_PLACE */


#define error__VL(x)	#x
/* Macro used in error__SR */


#define error_PLACE	"File '" __FILE__ "', line " error__SR( __LINE__) ". "
/*
  error_PLACE turns into something like: "File 'c.main', line 67. "
  so for example you can use this:

  if (x < 0) Error_Report(0, error_PLACE "x negative (x = %g)", x);

  (Note that there is no comma after error_PLACE.)

  This will automatically get the filename and linenumber where the error
  occurred: e.g. "File 'c.main', line 67. x negative (x = -3.5)"
*/
/*
  It would be nicer to use a macro to #define Error_Report so that
  __LINE__ and __FILE__ get inserted automatically, but macros can't cope
  with variable length argument lists, as used in Error_Report().

  All the clever macro stuff is copied from DeskLib:WAssert.h
*/



extern os_error error_global;
/*
  error_global is a global variable that can be used by any function that
  needs to return a custom (os_error *). This should be used if you want
  to build the error inside the function itself (using, for example,
  sprintf()).  You should use error_FIXED if the error is fixed (ie.
  hard-coded into the program).

  You simply fill in an errnum and errmess appropriate to the error, and then
  return it from your function.
*/


#define error_FIXED( id, number, description) \
  const struct {                              \
    int  errnum;                              \
    char errmess[ 1 + sizeof( description)];  \
    }                                         \
  id##_= { number, description};              \
  os_error *id = (os_error *) &id##_
/*
  MACRO: os_error id_ = {number, description};
         os_error *id = &id_;

  This declares and initialises an os_error block for the fixed error message
  given as description.

  id doesn't actually point to a fully-formed os_error block, it is just long
  enough to hold the description, which is why it must be a string literal
  like "Not enough memory".

  The typical use of this macro is to define the 'custom' errors returned
  by the user's functions.  This should be used at the top level of a file so
  that the resulting error block is usable as a return value.

  eg. error_FIXED(outofmemory, 0x80003, "Not enough memory");

  outofmemory can then be used as a return value for a function returning
  an (os_error *).

  error_STATIC may be used to provide a static (ie. local to one file) version
  of this macro.
*/


#define error_STATIC( id, number, description)     \
  static struct {                            \
    int   errnum;                                  \
    char errmess[ 1 + sizeof( description)]; \
    }                                              \
  id##_= { number, description};                   \
  static os_error *id = (os_error *) &id##_
/*
  MACRO: static os_error id_ = {number, description};
         static os_error *id = &id_;

  This declares and initialises an os_error block for the fixed error message
  given as description.

  id doesn't actually point to a fully-formed os_error block, it is just long
  enough to hold the description, which is why it must be a string literal
  like "Not enough memory".

  The typical use of this macro is to define the 'custom' errors returned
  by the user's functions.  This should be used at the top level of a file so
  that the resulting error block is usable as a return value.

  eg. error_FIXED(outofmemory, 0x80003, "Not enough memory");

  outofmemory can then be used as a return value for a function returning
  an (os_error *).

  error_FIXED may be used to provide a global version of this macro.
*/


#ifdef __cplusplus
}
#endif


#endif
