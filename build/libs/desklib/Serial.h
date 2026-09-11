/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Serial.h
    Author:  Copyright © 1993 Jason Howat
    Version: 1.11 (17 Apr 2005)
    Purpose: Serial-SWI interfaces.
*/


#ifndef __dl_serial_h
#define __dl_serial_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This provides an interface to the serial port SWIs.
*/


typedef enum
{
  serial_9600BAUD = 0,
  serial_75BAUD,
  serial_150BAUD,
  serial_300BAUD,
  serial_1200BAUD,
  serial_2400BAUD,
  serial_4800BAUD,
/*serial_9600BAUD,*/
  serial_19200BAUD = 8,
  serial_50BAUD,
  serial_110BAUD,
  serial_134_5BAUD,
  serial_600BAUD,
  serial_1800BAUD,
  serial_3600BAUD,
  serial_7200BAUD
} serial_rate;
/*
  This defines the values to be used when reading or setting baud rates
  with Serial_GetRxRate, SerialSetTxRate, etc.
*/


/*
  The following #defines describe the meaning of the bits in the word used
  by Serial_GetStatus/Serial_SetStatus.
*/

#define serial_USEXONXOFF             0x00000001  /* A bit in a status word */
#define serial_IGNOREDCDBIT           0x00000002  /* A bit in a status word */
#define serial_IGNOREDSRBIT           0x00000004  /* A bit in a status word */
#define serial_DTROFF                 0x00000008  /* A bit in a status word */
#define serial_IGNORECTSBIT           0x00000010  /* A bit in a status word */
#define serial_DONTUSERTSFLOWCONTROL  0x00000020  /* A bit in a status word */
#define serial_SUPPRESSINPUT          0x00000040  /* A bit in a status word */
#define serial_RTSHIGH                0x00000080  /* A bit in a status word */
#define serial_XOFFRECEIVED           0x00010000  /* A bit in a status word */
#define serial_XOFFSENT               0x00020000  /* A bit in a status word */
#define serial_DCDHIGH                0x00040000  /* A bit in a status word */
#define serial_NOCARRIER              0x00040000  /* A bit in a status word */
#define serial_DSRHIGH                0x00080000  /* A bit in a status word */
#define serial_RIHIGH                 0x00100000  /* A bit in a status word */
#define serial_CTSHIGH                0x00200000  /* A bit in a status word */
#define serial_XOFFSENTMANUALLY       0x00400000  /* A bit in a status word */
#define serial_SPACEINRECEIVEBUFFER   0x00800000  /* A bit in a status word */


/*
  The following #defines describe the meaning of the bits in the word used
  by Serial_GetFormat/Serial_SetFormat.
*/

#define serial_8BITWORD         0x00000000
/*
  A "setting" in a serial I/O format word.
  This denotes that 8 bit words are being used.
*/

#define serial_7BITWORD         0x00000001
/*
  A "setting" in a serial I/O format word.
  This denotes that 7 bit words are being used.
*/

#define serial_6BITWORD         0x00000002
/*
  A "setting" in a serial I/O format word.
  This denotes that 6 bit words are being used.
*/

#define serial_5BITWORD         0x00000003
/*
  A "setting" in a serial I/O format word.
  This denotes that 5 bit words are being used.
*/

#define serial_1STOPBIT         0x00000000
/*
  A "setting" in a serial I/O format word.
  This denotes that 1 stop bit is being used.
*/

#define serial_2STOPBITS        0x00000004
/*
  A "setting" in a serial I/O format word.
  This denotes that 2 stop bits are being used in most cases,
  1 stop bit is being used if 8 bit word with parity is being used,
  and 1.5 stop bits if a 5 bit word without parity of being used.
*/

#define serial_NOPARITY         0x00000000
/*
  A "setting" in a serial I/O format word.
  This denotes that parity is disabled.
*/

#define serial_PARITY           0x00000008
/*
  A "setting" in a serial I/O format word.
  This denotes that parity is enabled.
*/

#define serial_ODDPARITY        0x00000000
/* A "setting" in a serial I/O format word. This denotes odd parity. */

#define serial_EVENPARITY       0x00000010
/* A "setting" in a serial I/O format word. This denotes even parity. */

#define serial_MARKPARITY       0x00000020
/*
  A "setting" in a serial I/O format word.
  This denotes that parity is always 1 on TX and ignored on RX.
*/

#define serial_SPACEPARITY      0x00000030
/*
  A "setting" in a serial I/O format word.
  This denotes that parity is always 0 on TX and ignored on RX.
*/


extern os_error *Serial_Activate(void);
/*
  This function activates the serial drivers.
  It is equivalent to OS_Byte 2,2.
*/


extern os_error *Serial_Deactivate(void);
/*
  This function deactivates the serial drivers.
  It is equivalent to OS_Byte 2,0.
*/


extern int Serial_GetStatus(void);
/*
  This function returns the current state of the serial port, as given by
  OS_SerialOp 0.  There are various #defined macros to give meaningful names
  to the returned flags.
*/


extern os_error *Serial_SetStatus(int eor_mask, int and_mask);
/*
  This function sets the current status of the serial port using
  OS_SerialOp 0. There are various #defined macros to give meaningful
  names to the status flags.

  The new state is given by:
  new_state = (old_state AND and_mask) EOR eor_mask.
*/


extern int Serial_GetFormat(void);
/*
  This function returns the current data format for transmission/reception
  of characters on the serial line.  There are various #defined macros to
  give meaningful names to the format flags.
*/


extern os_error *Serial_SetFormat(int format);
/*
  This function sets the current data format for transmission/reception
  of characters on the serial line.  There are various #defined macros to
  give meaningful names to the format flags.
*/


extern serial_rate Serial_GetTxRate(void);
/*
  This function returns the current speed used for transmission of
  characters on the serial line.
*/


extern serial_rate Serial_GetRxRate(void);
/*
  This function returns the current speed used for reception
  of characters on the serial line.
*/


extern os_error *Serial_SetTxRate(serial_rate rate);
/*
  This function sets the speed to use for transmission of
  characters on the serial line.
*/


extern os_error *Serial_SetRxRate(serial_rate rate);
/*
  This function sets the speed to use for reception of
  characters on the serial line.
*/


extern int Serial_GetByte(void);
/*
  Reads a byte from the serial input buffer if one is available, otherwise
  returns -1.
*/


extern BOOL Serial_PutByte(int c);
/*
  Inserts a byte to be transmitted into the serial output buffer if there
  is room.  It returns ERROR if it was unable to send the byte.
*/


extern os_error *Serial_SendBreak(int length);
/*
  Transmits a break of the specified length in centiseconds.
*/


extern void Serial_NonZeroPollWordOnReceive(int pollword_value, void *pollword);
/*
  This function installs a piece of code on the InsV vector to monitor the
  insertion of characters into the serial input buffer.  It will modify the
  supplied pollword to the given value whenever an insertion occurs.  Also,
  an atexit function is registered to remove the vector claimant and the
  RMA based code.
*/


#ifdef __cplusplus
}
#endif


#endif
