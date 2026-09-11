/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.Hide.c
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

extern void Pane2_Hide(window_handle mainwindow)
{
 /*
  * Closes the window 'mainwindow' (removes it from screen) and all its panes
  * Use in the same manner to Window_Hide
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;

 /*
  * find element for main window and return if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return; /* not found */

 /*
  * hide main window
  */
  Window_Hide(mainelement->mainwindow);

 /*
  * hide all panes belonging to the main window
  */

 /*
  * find first pane window
  */
  paneelement = LinkList_FirstItem(&(mainelement->paneanchor));

  while(paneelement != NULL){

    Window_Hide(paneelement->panewindow);

    /*
     * find next pane window
     */
     paneelement = LinkList_NextItem(&(paneelement->header));
  }

}
