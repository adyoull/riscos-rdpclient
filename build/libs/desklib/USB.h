/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    USB.h
    Author:  Copyright © 2003 Peter Naulls
    Version: 1.01 (17 Apr 2005)
    Purpose: Veneers for Castle USB interface
*/

#ifndef __dl_usb_h
#define __dl_usb_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
   Warning: This API has not yet been implemented.  Do not use.

   Low level DeviceFS functions

   See: http://www.iyonix.com/32bit/USBDevFS.shtml
*/


typedef enum {
  devicefs_SENDREQUST = 0x8000000,
  devicefs_FREESPACE  = 0x8000002,
  devicefs_HANDLE     = 0x8000003
} devicefs_request;


typedef struct {
  unsigned char  request_type;
  unsigned char  request;
  unsigned short wvalue;
  unsigned short windex;
  unsigned short wlength;
} devicefs_bmrequest;


typedef struct {
  unsigned char bus;
  unsigned char port1;
  unsigned char port2;
  unsigned char port3;
  unsigned char port5;
  unsigned char port6;
} usb_location;

#define devicefs_call_RETURNPOLL     1
#define devicefs_call_CALLCOMPLETE   2



extern void devicefs_request(const char *name, devicefs_bmrequest *bmrequest,
                             void *buffer, unsigned int flags,
                             void *callback, int pword);

extern void devicefs_checkspace(const char *name, int handle,
                                unsigned int *available, unsigned int *free);


extern void devicefs_handles(const char *name, int handle,
                             unsigned int *buffer, unsigned int *stream);


extern void devicefs_location(const char *name, usb_location *location);

#ifdef __cplusplus
}
#endif

#endif
