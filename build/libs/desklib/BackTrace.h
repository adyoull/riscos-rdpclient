/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    BackTrace.h
    Author:  Copyright (c) 1995 Tom Hughes, Paul Field, Julian Smith
    Version: 1.01 (17 Apr 2005)
    Purpose: Functions which walk the current stack.
*/


/*
  NB, A warning to people using CMHG IRQ veneers - I have discovered to my
  cost that BackTrace functions fail to walk the stack provided by
  these veneers, and so give an address exception. I suspect that the fp
  pointer isn't NULL when it should be, or something.
 */


#ifndef __dl_backtrace_h
#define __dl_backtrace_h


#include <stdio.h>
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TARGET_UNIXLIB__

void BackTrace_SupportCurrentFrame(_kernel_unwindblock *frame);
/*
  Read the current values of fp, sl and pc (so that _kernel_unwind() can
  be called).
  */

void BackTrace_OutputToStdErr(void);
/*
  Generates a stack backtrace on stderr.
 */


void BackTrace_OutputToStreamWithPrefix(FILE* stream, const char* prefix);
/*
  Sends a backtrace to 'stream', prefixing each line with 'prefix'.
 */


typedef int (*backtrace_printf_fn)(void* reference, const char* format, ...);

void BackTrace_OutputToFFunctionWithPrefix(backtrace_printf_fn fn, void* reference, const char* prefix);
/*
  Sends a backtrace to 'fn', prefixing each line with 'prefix'.
 */


extern unsigned int BackTrace_GetPC(void);
/*
  Returns PC for caller.
 */

extern unsigned int BackTrace_GetSL(void);
/*
  Returns value of stack-limit register.
 */


extern unsigned int BackTrace_GetPC2(void);
/*
  Strips off status flags and processor mode from the current PC.
 */


int BackTrace_GetNestingDepth(void);
/*
  Returns the current function-nesting depth. Only functions which set up
  a stack-frame are detected, and note that Shared C Lib stack-extension
  functions create a stack-frame.
 */


typedef struct {
  int            n;
  unsigned int** functions;
} BackTrace_functionlist;

#define BackTrace_MAXFNS 256


const BackTrace_functionlist *BackTrace_GetCurrentFunctions(void);
/*
  Returns a pointer to an internal object containing array of current
  functions. Each entry in the array is the address of the save
  instruction used to create each stack-frame. This instruction is usually
  a 'STM ...'.
  'functions[0]' is address of most recent save instruction. 'n' is number
  of functions found.

  A maximum of BackTrace_MAXFNS are found.
 */



const char *BackTrace_GetFunctionName(const void* ptr);
/*
  Returns function name if ptr is <= 4 words after start of function and
  the name is embedded in code.
  Otherwise returns NULL.
 */

#endif

#ifdef __cplusplus
}
#endif

#endif
