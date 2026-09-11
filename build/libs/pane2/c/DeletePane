/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.DeletePane.c
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

extern void Pane2_DeletePane(window_handle mainwindow, int panenumber)
{
 /*
  * This hides and deletes the 'panenumber'th pane attached to the window
  * 'mainwindow'
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;

 /*
  * find element for main window and return if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return; /* not found */


  paneelement = FindPaneWindow(mainelement, panenumber);


  if(paneelement != NULL){

    Window_Delete(paneelement->panewindow);

   /*
    * remove pane element from list and free memory used by it
    */
    LinkList_Unlink(&(mainelement->paneanchor), &(paneelement->header));
    free(paneelement);

  }
}
