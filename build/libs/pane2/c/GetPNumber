/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.GetPNumber.c
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

extern int Pane2_GetPaneNumber(window_handle mainwindow, window_handle panewindow)
{
 /*
  * Returns pane number of 'panewindow' attatched to 'mainwindow' or 0 if the
  * window was not found.
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;
  int panecount;

 /*
  * find element for main window and return 0 if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return(0); /* not found */

 /*
  * find first pane window
  */
  paneelement = LinkList_FirstItem(&(mainelement->paneanchor));

  panecount = 1;

  while(paneelement != NULL){

     if(paneelement->panewindow == panewindow)
        return(panecount); /* pane window found */

    /*
     * find next pane window
     */
     paneelement = LinkList_NextItem(&(paneelement->header));

     panecount++;
  }

  return(0); /* not found */

}
