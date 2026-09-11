/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    WAssert.h
    Author:  Copyright © 1993, 1994 Mark H. Wilkinson
    Version: 1.02 (17 Apr 2005)
    Purpose: Wimp version of ANSI assert() system, based on RISCOS_Lib
             version presented in Archive magazine 6.12.53 by Remo Biagioni.
*/

/* Abstract */
/*
  This implements a version of the standard assert() system, which reports
  any problem using a standard Wimp error window.

  This allows you to ensure certain conditions are met during development
  and then recompile without them for the final release.  To use assertions,
  compile your program with _DEBUG defined (ie. passing -D_DEBUG to the
  compiler).  To build the release version, simply ensure this is not defined.
*/

/*
  Example:

  If you have a function that expects a parameter which must be positive
  then it should include a line like:
    assert(parameter >= 0);

  If 'parameter' is not >= 0 then a wimp error box will report the fact,
  indicating where in your source the assertion failed.
*/

#ifndef __dl_wassert_h
#define __dl_wassert_h

#ifdef __cplusplus
extern "C" {
#endif

extern void __wassert(const char *);
/*
  The function which produces the error box when there is a problem.
  Use assert() in your code, not this.
*/

#else

#undef assert

#endif


#ifndef _DEBUG

#define assert(ignore) ((void)0)
/*
  Macro: void assert(int expression);

  This is the definition used when not debugging - it effectively eliminates
  the code from the compiled version.
*/

#else

#  define __SR(x) __VL(x)
/* Macro used by assert */

#  define __VL(x) #x
/* Macro used by __SR */

#  define assert(e) \
((e) ? (void)0 : __wassert("Assertion " #e " failed at line " __SR(__LINE__) " of file " __FILE__))
/*
  Macro: void assert(int expression);

  You simply call this with an expression you assert to be true, ie. it
  should evaluate to non-zero.  If the expression evaluates to be true
  nothing happens, otherwise it raises a Wimp error saying what the
  assertion was and where the assertion was made.
*/

#ifdef __cplusplus
}
#endif

#endif
