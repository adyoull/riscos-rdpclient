/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.SetVars.c
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

extern BOOL Pane2_SetVars(window_handle mainwindow, int panenumber,
                           wimp_point *paneoffset, wimp_point *panesize)
{
 /*
  * Sets paneoffset if 'paneoffset' is not NULL and panesize if 'panesize' is
  * not NULL for the 'panenumber'th pane attatched to 'mainwindow'.
  * TRUE is returned for success.
  * After changing the variables, Pane2_OpenWindow must be used to make the
  * windows on screen exhibit the changes.
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;

 /*
  * find element for main window and return FALSE if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return(FALSE); /* not found */

 /*
  * find first pane window
  */
  paneelement = LinkList_FirstItem(&(mainelement->paneanchor));

  paneelement = FindPaneWindow(mainelement, panenumber);

  if(paneelement != NULL){

     if(paneoffset != NULL)
         paneelement->paneoffset = *paneoffset;

     if(panesize != NULL)
         paneelement->panesize = *panesize;

     return(TRUE);

  }
  else
    return(FALSE); /* not found */

}
