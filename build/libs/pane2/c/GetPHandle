/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.GetPHandle.c
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

extern window_handle Pane2_GetPaneHandle(window_handle mainwindow, int panenumber)
{
 /*
  * Returns window handle of 'panenumber'th pane attached to the window 'main'
  * or 0 if not found.
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;

 /*
  * find element for main window and return NULL if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return(0); /* not found */


  paneelement = FindPaneWindow(mainelement, panenumber);


  if(paneelement != NULL)
     return(paneelement->panewindow);

  else
     return(0);
}
