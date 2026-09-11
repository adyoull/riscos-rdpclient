/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Desklib:Time.h
    Author:  Copyright © 1992, 1993, 1994 Jason Williams, Jason Howat
    Version: 1.03 (17 Apr 2005)
    Purpose: time handling
*/


#ifndef __dl_time_h
#define __dl_time_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_core_h
#include "DeskLib:KernelSWIs.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This header declares functions for handling times.
*/


typedef struct
{
  unsigned int cs;
  unsigned int sec;
  unsigned int min;
  unsigned int hour;
  unsigned int mday;
  unsigned int month;
  unsigned int year;
  unsigned int wday;
  unsigned int yday;
} time_ordinals;
/*
  This is an ordinal block, used to store the ordinals returned by
  Time_ConvertTimeToOrdinals and similar.  It is a more accessible
  form of clock than the five-byte block clock.

  Note that it is *not* equivalent to a C time block, in that the
  month numbers (mday) range from 1 to 12, the weekday (wday) range is
  from 1-7 and the days of the year (yday) start at 1, not zero.

  Note also that the week starts on Sunday, not Monday.
*/


os_error *Time_ReadClock(unsigned char *fivebyteblock);
/*
  This gets a copy of the CMOS clock as a five-byte integer, suitable for
  passing to Time_ConvertDateAndTime and similar functions.

  The integer represents the number of centiseconds from 1900/01/01 00:00:00,
  in UTC (which is equivalent to GMT).
*/


extern unsigned int Time_Monotonic(void);
/*
  This is an assembler veneer for the OS_ReadMonotonicTime SWI.
  It returns an integer representing the time since the computer was
  switched on, in centiseconds.
*/


os_error *Time_ConvertTimeToOrdinals(unsigned char *fivebyteblock, time_ordinals *ord);
/*
  This converts the five byte system clock value into time ordinals (ie.
  it breaks it down into minutes, hours, etc.) for the current Territory.

  Simply pass in a pointer to a five byte block containing the system time
  and a pointer to a time_ordinals block to be filled.
*/


os_error *Time_ConvertTimeToUTCOrdinals(unsigned char *fivebyteblock,
                                        time_ordinals *ord);
/*
  This converts the five byte system clock value into time ordinals (ie.
  it breaks it down into minutes, hours, etc.) in UTC rather than local
  time.  UTC being equivalent to GMT, by the way.

  Simply pass in a pointer to a five byte block containing the system time
  and a pointer to a time_ordinals block to be filled.
*/


extern os_error *Time_ConvertDateAndTime(unsigned char *fivebyteblock, char *buffer,
                                         int bufflen, char *format);
/*
  This is a veneer for the OS_ConvertDateAndTime SWI.  It converts the
  five-byte system time to a string under the control of the format string.
  The format string may contain a number of different tokens, preceded by
  a '%', which will be expanded to the correct textual reprsentation for
  the given time.  The resulting string if placed in 'buffer'.

  Tokens are: CS, centiseconds;
              SE, seconds;
              MI, minutes;
              12, hours in the twelve hour format;
              24, hours in the twenty-four hour format;
              AM, giving 'am' or 'pm' as appropriate;
              PM, the same as 'AM';
              WE, the day of the week in full;
              W3, a three-character abbreviated weekday;
              WN, the weekday as a number, 1-7 meaning Sunday-Saturday;
              DY, the day of the month;
              ST, 'st', 'nd', 'rd' or 'th' as appropriate;
              MO, the full month name;
              M3, a three-character abbreviated month name;
              MN, the month as a number;
              CE, the century;
              YR, the year in the century;
              WK, the week (Mon-Sun) number in the year;
              DN, the day of the year;
              0,  a nul character ('\0');
              %,  insert a '%' character.
*/


#define Time_CTIME     "%W3 %M3 %DY %z24:%MI:%SE %CE%YR"
/*
  A pre-defined time format, corresponding to the standard format as
  used by the ctime function in the standard library, but without a
  newline at the end.
*/

#define Time_STANDARD  "%24:%mi:%se %dy-%m3-%ce%yr"
/*
  A pre-defined time format, corresponding to the default setting on
  most RISC OS machines.  This is a fixed format at *not* the system
  variable Sys$DateFormat.
*/

#define Time_FANCYDATE "%WE, %zDY%ST of %MO, %CE%YR"
/*
  A pre-defined time format, as you might write a date in text.
  eg. "Sunday, 16th of January, 1994."
*/

#define Time_SHORTTIME "%z12:%MI%pm"
/*
  A pre-defined date format, giving the time in the twelve-hour clock.
  eg. "11:30pm"
*/

#define Time_LONGTIME  "%z12:%MI:%SE%pm"
/*
  A pre-defined date format, giving the time in the twelve-hour clock
  to "second" resolution.  eg. "11:30:44pm"
*/

#define Time_EUROTIME  "%z24:%MI:%SE"
/*
  A pre-defined date format, giving the time in the twenty-four-hour
  clock, to "second" resolutiion.  eg. "23:30:44"
*/


extern os_error *Time_ConvertStandardDateAndTime(unsigned char *fivebyteblock,
                                                 char *buffer, int bufflen);
/*
  This is a veneer for the OS_ConvertStandardDateAndTime SWI.  It converts
  the five-byte system time using the format string stored in the system
  variable Sys$DateFormat (ie. the default time format as specified on
  the machine we are running on).  The resulting string is placed in
  'buffer'.
*/


#ifdef __cplusplus
}
#endif


#endif
