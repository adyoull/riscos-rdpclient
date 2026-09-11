/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.SetFlags.c
    Author:  Copyright © 1995 Andrew Sellors.
    Version: 1.04 (4th August 1995)
    Purpose: Handles windows with panes.
*/

#include "DeskLib:Pane2.h"
#include "Pane2Defs.h"
#include "Desklib:Event.h"
#include "DeskLib:Template.h"
#include "Desklib:EventMsg.h"
#include "Desklib:Error.h"

#include <stdlib.h>


/******************************************************************************/

extern int Pane2_SetFlags(window_handle mainwindow, int panenumber, int flags)
{
 /*
  * Sets flags of 'panenumber'th pane attached to the window 'main' or NULL
  * if the 'panenumber' is invalid.
  * First pane is number 1.
  * Returns flags and if 'flags' is -1 then they are left unchanged
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;

 /*
  * find element for main window and return NULL if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return(NULL); /* not found */


  paneelement = FindPaneWindow(mainelement, panenumber);


  if(paneelement != NULL){

     if(flags != -1)
        paneelement->paneflags.value = flags;

     return(paneelement->paneflags.value);

  }
  else
     return(NULL);
}
