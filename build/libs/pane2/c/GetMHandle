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

extern window_handle Pane2_GetMainHandle(window_handle panewindow)
{
 /*
  * Returns window handle of main window which 'panewindow' is attached to
  * or 0 if not found.
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;

  mainelement = LinkList_FirstItem(&main_anchor);

  while(mainelement != NULL){

     paneelement = LinkList_FirstItem(&(mainelement->paneanchor));

     while(paneelement != NULL){

        if(paneelement->panewindow == panewindow)
           return(paneelement->mainwindow);

        paneelement = LinkList_NextItem(&(paneelement->header));

     }

     mainelement = LinkList_NextItem(&(mainelement->header));
  }

  return(0);
}
