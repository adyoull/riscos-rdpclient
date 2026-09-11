/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Print.h
    Author:  Copyright © 1995 Julian Smith
    Version: 1.01 (17 Apr 2005)
    Purpose: Automatic handling of RO 3 wimp printing protocol messages,
             and some of the PDriver calls involved in printing.

*/

#ifndef __dl_print_h
#define __dl_print_h

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif


#ifndef __dl_pdriver_h
#include "DeskLib:PDriver.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header defines one function and a number of types to handle the
  Wimp printing protocol messages automatically, calling various functions
  specified by you to output the data, or report the end of the printing
  process.
*/


typedef struct print_block
{
  void  *reference;         /* As passed to Print_StartPrint. */
  const char *jobtitle;     /* As passed to Print_StartPrint. */
  printer_info printerinfo; /* This info may be useful to printfn. */
  print_job job;            /* The current job handle. */
  print_job oldjob;         /* The previous job (if any). */
} print_block;
/*
  A pointer to a block of this type is passed to your various print handler
  functions, so that you have information about the print job.

  The reference is whatever you set with Print_StartPrint, as is the job
  title.  The 'printerinfo' is supplied so that you can adapt your printout
  to the capabilities of the printer (where appropriate).  'job' is the
  current job handle and 'oldjob' the previous job (if there was one).
*/


typedef enum
{
  print_result_OK = 0,            /* Printing is finished. */
  print_result_QUEUED,            /* Data has been saved into printer queue. */
  print_result_NEEDPRINTERMANAGER,/* printfn==NULL, but !Printers not running. */
  print_result_PRINTERROR,        /* message_PRINTERROR received. */
  print_result_SAVEFAILED,        /* savefn failed. */
  print_result_CANTSAVE,           /* savefn==NULL, !Printers busy */
  print_result_CANTOPENPRINTER,   /* File_Open( "Printer:") failed. */
  print_result_FAILED             /* Something else has gone wrong. */
} print_results;
/*
  These are the various non-os_error return values you can be passed in
  your printing result handler.

  "OK" and "QUEUED" both indicate successful completion of the printing,
  either directly or to the queue.  All the other return values mean some
  sort of failure.

  "NEEDPRINTERMANAGER" means that the printer driver is not running and
  so you can't print.  "PRINTERROR" means that we received an error during
  printing, "SAVEFAILED" means that the save function returned ERROR and
  "CANTSAVE" means that there is no save function specified and !Printers
  is busy.  "CANTOPENPRINTER" means that File_Open("Printer:") failed, and
  "FAILED" is used for all other problems which may occur.
*/


typedef int print_result;
/*
  This type is passed to the result function to report the outcome of the
  attempt to print.

  If it is greater than 255 it is a pointer to a standard OS error block
  (an os_error *), otherwise it is one of the print_results values.
*/


typedef BOOL (*print_printfn)(print_block *print);
/*
  A function of this type is called to do the actual printing after the
  wimp messaging has finished.  The print_block is supplied to give you
  any information you might need to carry out the printing.

  You should return ERROR if there is a problem while printing, or
  NOERROR if you conclude the print successfully.
*/


typedef BOOL (*print_savefn)(print_block *print,
                             message_datasaveack *datasaveack);
/*
  A function of this type is called to save the printout to file in a
  form suitable for printing later, for instance if the job is queued.

  The print_block is passed to give you any information you might need
  to help you save the printout to file.  The message_datasaveack is
  supplied to give you all the information such as filetype, the full
  filename to save it to, and so on.

  You only need to save the data and return NOERROR if successful, the
  Print library will take care of any other messages which need to be sent.
  If for some reason you cannot save the file, you should return ERROR.
*/


typedef void (*print_resultfn)(print_block *print, print_result result);
/*
  A function of this type is called when the printing has finished, whether
  successfully or not.

  The print_block is supplied to give you any identifying information you
  might need about the print job, and the print_result gives the result
  of the printing process.  If 'result' is greater than 255 it is a pointer
  to a standard Wimp error block (an os_error *), otherwise it is one of
  the print_results enumerated values.
*/


BOOL Print_StartPrint(print_printfn printfn,
                      print_savefn savefn,
                      print_resultfn resultfn,
                      void *reference, /* Some value to be passed to handlers */
                      int filetype,    /* Used if print data is saved. */
                      int estsize,     /* Used if print data is saved. */
                      const char *leafname,  /* Used if print data is saved. */
                      const char *jobtitle);
/*
  You call this function to start a printout, specifying handlers to print,
  save the printout to file in a form that can be printed later, and to
  receive the result of the attempt to print.  This function then handles
  the wimp messages prior to printing, and calls your handlers as appropriate.

  If you either cannot print or save to file you can pass NULL instead of
  one of the handler functions.  You must supply a result function, however,
  as this is your only way of determining the result of the print.

  The save function will be called if the print is queued or if you don't
  specify a print function.  In this case the printer driver will print the
  saved file itself if it can handle it directly (eg. text), or else
  broadcast a message_PRINTTYPEODD if it can't.  You should have an event
  handler to receive the message if the resultfn tells you the file has been
  queued to print later, and it is not in a directly-printable format.

  The 'jobtitle' is used to give a identifiable name to the print job,
  and *is not copied*.  This string must continue to exist until the
  print job has finished.  If you choose not to have a job title, you
  can pass NULL instead.

  The 'reference' is simply passed to all of your handler functions, and
  can be any value helpful to you.

  The 'filetype', 'estsize' and 'leafname' are all used to save the file if
  the print data needs to be saved.

  This function returns NOERROR if the printing protocol was started OK
  and ERROR otherwise.
*/


#ifdef __cplusplus
}
#endif


#endif
