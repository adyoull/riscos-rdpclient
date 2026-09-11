/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.Delete.c
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

extern void Pane2_Delete(window_handle mainwindow)
{
 /*
  * This hides and deletes the window 'mainwindow' and all its panes
  * Use in the same manner to Window_Delete
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
  * delete main window
  */
  Window_Delete(mainelement->mainwindow);


 /*
  * delete all panes belonging to the main window
  */

 /*
  * find first pane window
  */
  paneelement = LinkList_FirstItem(&(mainelement->paneanchor));

  while(paneelement != NULL){

    Window_Delete(paneelement->panewindow);

   /*
    * remove pane element from list and free memory used by it
    */
    LinkList_Unlink(&(mainelement->paneanchor), &(paneelement->header));
    free(paneelement);

    /*
     * find next pane window
     */
     paneelement = LinkList_FirstItem(&(mainelement->paneanchor));
  }

 /*
  * remove main window element from list and free memory used by it
  */
  LinkList_Unlink(&main_anchor, &(mainelement->header));
  free(mainelement);

}
