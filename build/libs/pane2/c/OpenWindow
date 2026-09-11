/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.OpenWindow.c
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

extern BOOL Pane2_OpenWindow(window_handle mainwindow, window_openblock *openblock)
{
 /*
  * Calls Wimp_OpenWindow for the mainwindow and all its panes as if it was
  * send an openwindow request.
  * If 'openblock' is NULL then the current state of the main window is used.
  * Supplying an openblock allows the main window to be changed and the panes to be
  * altered to match.
  * Can be used to make a pane window appear that has been added to an open mainwindow.
  */
  main_listelement *mainelement;
  window_state mainstate;

 /*
  * find element for main window and return NULL if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return(FALSE); /* not found */

 /*
  * read main window state if openblock not supplied
  */
  if(openblock == NULL){
     Wimp_GetWindowState(mainwindow, &mainstate);
     openblock = &(mainstate.openblock);
  }

  return(OpenWindow(openblock, mainelement));
}
