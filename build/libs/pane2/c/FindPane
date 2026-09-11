/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.FindPane.c
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

extern pane_listelement *FindPaneWindow(main_listelement *mainelement, int panenumber)
{
 /*
  * returns the pane element for the 'panenumber'th pane attached to
  * 'mainelement' or NULL if not found
  */
  int panecount;
  pane_listelement *paneelement;

 /*
  * find first pane window
  */
  paneelement = LinkList_FirstItem(&(mainelement->paneanchor));

  panecount = 1;

  while((paneelement != NULL) && (panecount < panenumber)){

    /*
     * find next pane window
     */
     paneelement = LinkList_NextItem(&(paneelement->header));

     panecount++;
  }

  return(paneelement);
}
