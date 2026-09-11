/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Url.h
    Author:  Copyright © 2002 Antony Sidwell
    Version: 1.01 (17 Apr 2005)
    Purpose: Handle Url launching using the Event & EventMsg bits of DeskLib
*/

#ifndef __dl_url_h
#define __dl_url_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header declares functions which are useful in dealing with URLs,
  particularly in desktop apps.

  This includes the launching of URLs for other tasks to handle, the
  receipt of such URL launches for your own program to handle, and some
  processing functions to deal with the text of URLs.
*/


typedef void urllaunch_handler(const char *url);
/*
  This type of function is used for reporting success or failure when
  launching an URL.  The string you are passed is the URL you were
  attempting to launch, and it will be freed as soon as your function
  returns so you should take a copy if you want to refer to it later.
*/


extern void Url_Launch(const char *url, urllaunch_handler *failure,
                       urllaunch_handler *success);
/*
  This function attempts to launch an Url.  You simply call Url_Launch
  with the URL you want opened, and the various strategies for passing
  URLs onto other programs will be tried until one succeeds or they all
  fail.  This involves claiming messages with EventMsg, so you should
  have initialised the Event library before using this function.

  You can supply functions to be called if the attempt succeeds or fails.
  These will be passed the URL you tried to load.  If you don't want to
  use either of these, pass NULL instead.

  Note that in this case, failure means being unable to find a program to
  handle the URL.  Passing, for instance, a non-existent web page address
  to a web browser will still be counted as a success from the point of
  view of this function - it is only a failure if there is no web browser
  to accept that request.
*/

extern BOOL Url_Encode(const char *toencode, char *encoded_buf, int *bufsize);
/*
  This function prepares a string so it is ready for inclusion into an URL.
  Note that this is intended for user input and the like, as slashes, colons,
  etc. will all be encoded here, which means it will not be useable on "full"
  URLs, such as "http://www.google.com/".

  You pass the string to be encoded, a pointer to a buffer for the encoded
  string and a pointer to a variable containing the size of that buffer.

  If the buffer is of sufficient size to hold the encoded string, it is
  filled with that string and TRUE is returned.  If the buffer is not
  big enough, or if *bufsize is zero, FALSE is returned, and 'bufsize' is
  filled with the length of buffer needed to hold the encoded string (including
  the nul terminator).

  Thus you can use a two pass system by passing zero as the buffer size to find
  the amount of memory to allocate for the buffer, allocating that memory, and
  then calling again to obtain the encoded string itself.
*/


typedef BOOL urlreceive_handler(BOOL check_only, const char *url);
/*
  This type of function is used to handle URL requests received from other
  programs.  You can register a handler function using Url_AddHandler.

  When called, the function should examine the check_only flag, and if TRUE
  it shouldn't deal with the URL, only check if it is capable of dealing with
  it.  If check_only is FALSE, it may actually deal with the URL if capable.

  It should return TRUE if capable and FALSE if it is not.
*/


extern BOOL Url_AddHandler(urlreceive_handler *handler);
/*
  This function registers a handler function to be called when a request to
  process an URL is received.  It returns TRUE if the handler was added
  successfully.

  You can attach more than one handler - the most recently added handler will
  be called first, and if that returns FALSE (i.e. it cannot handle the URL)
  the next most recently added handler will be given a chance to process it,
  and so on.
*/


extern BOOL Url_RemoveHandler(urlreceive_handler *handler);
/*
  This function removes an URL handler which was added with Url_AddHandler,
  meaning it will no longer be called when a request to handle an URL is
  received.  It returns TRUE if the handler could be removed.

  If the same handler has been attached more than once, this will remove the
  most recently added of them, not all of them.
*/


#ifdef __cplusplus
}
#endif

#endif
