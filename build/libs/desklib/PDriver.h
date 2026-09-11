/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    PDriver.h
    Author:  Copyright © 1993, 1994 Jason Howat (and a bit by Jason Williams)
    Version: 1.12 (17 Apr 2005)
    Purpose: SWI veneers for using printer drivers (the PDriver module)
*/



#ifndef __dl_pdriver_h
#define __dl_pdriver_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This file provides veneers to the PDriver module, the RISC OS printer
  interface.
*/


typedef int print_job;
/*
  This is a reference code for a print job.
  (it is actually a RISC OS file handle)
*/

typedef struct
{
  int xx;
  int xy;
  int yx;
  int yy;
} print_transformation;
/*
  This is a Draw-style transformation matrix to be applied to the specified
  rectangle before printing it.  The new coordinates are calculated like so:

  newx = (xx * x) + (yx * x);
  newy = (xy * y) + (yy * y);
*/


/* --- Values for the drivertype field of printer_info --- */

#define printdriver_POSTSCRIPT          0
/* A "drivertype".  It represents a PostScript printer. */

#define printdriver_FX80                1
/* A "drivertype".  It represents an Epson printer. */

#define printdriver_LASERJET            2
/* A "drivertype".  It represents a LaserJet printer. */

#define printdriver_INTEGREX            3
/* A "drivertype".  It represents an Integrex ColourJet printer. */

#define printdriver_FAXMODEM            4
/* A "drivertype".  It represents a FAX modem. */

#define printdriver_DIRECTDRIVELASER    5
/* A "drivertype".  It represents a direct drive laser printer. */

#define printdriver_CASPELGRAPHLANG     6
/* A "drivertype".  It represents Caspel graphics language. */

#define printdriver_PDUMPER             7
/* A "drivertype".  It represents the PDumper interface. */

#define printdriver_EPSONJX             99
/* A "drivertype".  It represents an  Epson JX printer. */

#define printdriver_STARLC10            99
/* A "drivertype".  It represents a Star LC10 printer. */

#define printdriver_PAINTJET            99
/* A "drivertype".  It represents a PaintJet printer. */



/* --- These #defines represent the bits of the 'features' word --- */

#define printfeat_COLOUR         0x00000001
/* If set, the printer can print colour, otherwise it is monochrome */

#define printfeat_LIMITED        0x00000002
/*
  If unset, it supports the full colour range, otherwise the colour
  range is limited.
*/

#define printfeat_DISCRETE       0x00000004
/*
  If set, the printer supports only a discrete set of colours at the software
  level - no mixing, dithering, toning, etc.  If unset, the colour (or grey)
  range is semi-continuous.
*/

#define printfeat_NOFILL         0x00000100
/*
  If set, the printer cannot handle filled shapes except by outlining them.
  A plotter might have this bit set, for example.
*/

#define printfeat_NOTHICKNESS    0x00000200
/*
  If set, the printer cannot handle variable-width lines - it has only one
  line thickness available.  A plotter might have this bit set, for example.
*/

#define printfeat_NOOVERWRITE    0x00000400
/*
  If set, the printer cannot handle overwriting of one colour with another
  properly - it can only overwrite the background colour with another colour.
*/

#define printfeat_TRANSSPRITE    0x00000800
/* If set, the printer can handle transformed sprite plotting. */

#define printfeat_NEWFONTMANAGER 0x00001000
/*
  If set, the printer can handle "new" FontManager features such
  as transforms and encodings.
*/

#define printfeat_SCREENDUMP     0x01000000
/* If set, the printer supports screen dumps. */

#define printfeat_TRANSFORMS     0x02000000
/*
  If set, the printer supports arbitrary transformations supplied to
  PDriver_DrawPage.  Otherwise, it only supports scalings, translations
  and rotations by multiples of 90 degrees.
*/

#define printfeat_ILLUSTRATIONS  0x04000000
/* If set, the printer supports the PDriver_InsertIllustration call. */

#define printfeat_MISCOP         0x08000000
/* If set, the printer supports the PDriver_MiscOp call. */

#define printfeat_SETDRIVER      0x10000000
/* If set, the printer supports the PDriver_SetDriver call. */

#define printfeat_DECLAREFONT    0x20000000
/* If set, the printer supports the PDriver_DeclareFont call. */


typedef union
{
  int value;
  struct
  {
    unsigned int colour         : 1;
    unsigned int limited        : 1;
    unsigned int discrete       : 1;
    unsigned int filler1        : 5;   /* reserved */
    unsigned int nofill         : 1;
    unsigned int nothickness    : 1;
    unsigned int nooverwrite    : 1;
    unsigned int transsprite    : 1;
    unsigned int newfontmanager : 1;
    unsigned int filler2        : 3;   /* reserved */
    unsigned int filler3        : 8;   /* reserved */
    unsigned int screendump     : 1;
    unsigned int transforms     : 1;
    unsigned int illustrations  : 1;
    unsigned int miscop         : 1;
    unsigned int setdriver      : 1;
    unsigned int declarefont    : 1;
    unsigned int filler4        : 2;   /* reserved */
  } data;
} print_features;
/*
  This represents the various features of a printer which may or may not be
  available, as is returned by PDriver_Info or can be checked for with
  PDriver_CheckFeatures.

  The exact meanings can be found in the PRMs, rough outlines can be found
  with the #definitions of the flags in this file (the printfeat_* macros).

  The 'filler' fields are reserved at present.
*/


typedef struct
{
  unsigned int     version    : 16;
  int              drivertype : 16;
  wimp_point       resolution;
  print_features   features;
  char             *description;
  wimp_point       halftone_res;
  int              id_number;
} printer_info;
/*
  This is a block containing information about a printer driver, as is
  returned by PDriver_Info.

  The 'version' is the printer driver version * 100, the 'drivertype' is
  the printer driver number as #defined elsewhere in this header.
  The x and y resolutions are in dots per inch. and the halftone resolutions
  are in repeats per inch (they are the same as the dpi values if there is no
  halftoning).  'id_number' is the driver-specific number identifying the
  configured printer - this is for internal printer driver use only.
*/


os_error *PDriver_Info(printer_info *block);
/*
  This function gets information on the printer driver.  It allows the
  application to change the way it outputs its data to suit the printer.
*/


extern char *PDriver_PrinterName(void);
/*
  This function returns the names of the current printer.  To use it, you
  should take a copy as it can change under a multiple printer system.
  The returned string is *read only*, as it is in fact a pointer to the
  PDriver's RMA workspace.

  If there is no printer driver installed, it returns NULL.
*/


os_error *PDriver_CheckFeatures(print_features mask, print_features value);
/*
  This function checks the features of the printer.  If those bits set in
  'mask' have the same values in both the current printer's features and in
  'value' then the function returns NULL, if there are differences then an
  error is generated.
*/


os_error *PDriver_PageSize(wimp_point *size, wimp_rect *printable);
/*
  This function finds how large the paper and print area is in millipoints
  (72000ths of an inch).  'size' is filled with the paper size, and
  'printable' contains the printable rectangle (origin at bottom left).
*/


os_error *PDriver_SelectJob(print_job newjob, const char *title, print_job *oldjob);
/*
  This function makes a given print job the current one. The job is identified
  by 'new' which must be a file (eg printer:) previously opened for output
  (eg by File_Open). 'title' gives a human-readable name for the job.

  The current print job is suspended and the new job (re)started. The old job
  handle (if any) is returned in the variable pointed to by 'old'.

  Note that if 'new' is zero then the current job is suspended.
*/


os_error *PDriver_CurrentJob(print_job *current);
/*
  This function gets the file handle of the current job and stores it in the
  variable pointed to by 'current'.  Zero is stored if no print job is active.
*/


os_error *PDriver_EndJob(print_job job);
/*
  This function ends a print job normally.  This may result in further printer
  output (eg. a PostScript printer driver outputs a trailer).
*/


os_error *PDriver_AbortJob(print_job job);
/*
  This function ends a print job abnormally.  It should be called immediately
  if you get an error while printing (even before showing an error message).
  No further printer output will be produced.
*/


os_error *PDriver_GiveRectangle(int rectangle_id, wimp_rect *rectangle,
                                print_transformation *matrix,
                                wimp_point *position,
                                unsigned int background_colour);
/*
  This function specifies a rectangle that the application wants to print.
  'rectangle_id' is a value of your choosing to identify the rectangle,
  'rectangle' is the rectangle being specified (in OS units), and 'matrix'
  holds the transformation to be performed on the area before being printed.

  The 'position' is the location of the bottom left hand corner of the
  rectangle on the page, in millipoints.  The background colour is given
  in the standard notation of 0xBBGGRR00.

  You specify one or more rectangles to print with this function, before
  calling PDriver_DrawPage and then PDriver_GetRectangle.  The printer
  driver can ask you to print these rectangles, or parts of them, in any
  order it wishes.

  Your rectangles are advised to be a few OS units larger than the area
  being printed so that rounding errors don't lose information from the
  rectangle, only empty space.
*/


os_error *PDriver_DrawPage(int copies, wimp_rect *rectangle_to_print,
                           int sequence_no, const char *page,
                           int *finished, int *rectangle_id);
/*
  This function should be called after all rectangles to be printed have
  been specified with PDriver_GiveRectangle, to draw the page for printing.

  'copies' is the number of copies to print, 'sequence_no' should be zero
  or contain the page number and 'page' should be NULL or give a textual
  form of the page number (without spaces).

  On return, 'rectangle_to_print' contains the rectangle whose user id is
  returned in 'rectangle_id'.  'finished' contains the number of copies
  still to be printed, hence is zero when the job is finished. You should
  not user this to determine behaviour, only to give user information about
  how much is left to print, as it may not be accurate.
*/


os_error *PDriver_GetRectangle(wimp_rect *rectangle_to_print,
                               int *finished, int *rectangle_id);
/*
  This function should be called after PDriver_DrawPage and then called
  repeatedly until 'finished' returns zero.

  On return, 'rectangle_to_print' contains the rectangle whose user id is
  returned in 'rectangle_id'.  'finished' contains the number of copies
  still to be printed, hence is zero when the job is finished.  You should
  not user this to determine behaviour, only to give user information about
  how much is left to print, as it may not be accurate.
*/


os_error *PDriver_CancelJob(print_job job);
/*
  This function causes subsequent attempts to output to the print job given
  to do nothing other than generate the error 'Print cancelled'. An
  application is expected to respond to this by aborting the print job.
*/


os_error *PDriver_ScreenDump(print_job job);
/*
  This function makes the printer driver output a screen dump to the job
  given. Note that none of the standard Acorn driver support this feature
  (check the features first using PDriver_CheckFeatures or PDriver_Info).
*/


os_error *PDriver_EnumerateJobs(print_job *handle);
/*
  This function enumerates all existing print jobs.  To get the complete list
  you should call with 'handle' containing zero.  It will then be updated to
  the next handle in the list.  You then continue calling until it is once
  again zero.
*/


os_error *PDriver_CancelJobWithError(print_job job, os_error *error);
/*
  This function cancels a print job so that any attempt to output to it will
  do nothing other than generate the error given in 'error'.
*/


os_error *PDriver_SelectIllustration(print_job newjob, const char *title,
                                     print_job *oldjob);
/*
  This does the same as PDriver_SelectJob, except that if a new print job is
  started it must only be one page long and the output may be different
  (eg. the PostScript printer driver will generate Encapsulated PostScript).

  The idea is that you should use this function rather than PDriver_SelectJob
  when the application is printing a single page which could be useful as
  an illustration in another document.  It is recommended that you should only
  call this when the user chooses to save the print job to file as an
  illustration.
*/


os_error *PDriver_InsertIllustration(int illustration_handle, void *drawpath,
                                     wimp_point *bottom_left,
                                     wimp_point *bottom_right,
                                     wimp_point *top_left);
/*
  If this function is supported (check the PDriver_Info), it inserts an external
  file into the current job's output.

  'illustration_handle' is a RISC OS file handle for the file being inserted.
  'drawpath' is a pointer to a clipping path for the illustration, or zero.
  The other parameters define the position of the illustration in 256ths of
  an OS unit.
*/

#ifdef __cplusplus
}
#endif


#endif
