/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Pane2.AddPane.c
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

extern BOOL Pane2_AddPane(window_handle mainwindow, window_handle panewindow,
                          wimp_point *paneoffset, wimp_point *panesize, int flags)
{
 /*
  * Adds the pane window 'panewindow' to the main window 'mainwindow'.
  * If 'paneoffset' is not NULL then this is taken as the offset between the
  * pane and the main window instead of the positions in the template.
  * If 'panesize' is not NULL then this is taken as the size of the pane
  * instead of the size in the template.
  */
  main_listelement *mainelement;
  pane_listelement *paneelement;
  window_state mainstate;
  window_state panestate;

 /*
  * find element for main window and return FALSE if mainwindow not present
  */
  mainelement = FindMainWindow(mainwindow);
  if(mainelement == NULL)
     return(FALSE); /* not found */

 /*
  * malloc memory for list element and return FALSE if fails
  */
  paneelement = malloc(sizeof(pane_listelement));
  if(paneelement == NULL)
     return(FALSE); /* failed */

 /*
  * get info about main and pane windows
  */
  Wimp_GetWindowState(mainwindow, &mainstate);
  Wimp_GetWindowState(panewindow, &panestate);

 /*
  * initialise list header
  */
  LinkList_Init(&(paneelement->header));

 /*
  * fill element
  */
  paneelement->panewindow = panewindow;
  paneelement->mainwindow = mainwindow;
  paneelement->paneflags.value = flags;

  if(paneoffset != NULL) /* if pane offset is provided */
     paneelement->paneoffset = *paneoffset;

  else{ /* use template pane offset */

     paneelement->paneoffset.x = panestate.openblock.screenrect.min.x -
                                 mainstate.openblock.screenrect.min.x;


     if(paneelement->paneflags.data.maintop) /* pane fixed to top of window */
        paneelement->paneoffset.y = mainstate.openblock.screenrect.max.y;

     else /* pane fixed to bottom of window */
        paneelement->paneoffset.y = mainstate.openblock.screenrect.min.y;

     if(paneelement->paneflags.data.panetop) /* pane fixed using top of pane */
        paneelement->paneoffset.y -= panestate.openblock.screenrect.max.y;

     else /* pane fixed using bottom of pane */
        paneelement->paneoffset.y -= panestate.openblock.screenrect.min.y;

  }

  if(panesize != NULL) /* if pane size is provided */
     paneelement->panesize = *panesize;

  else{ /* use template pane size */

     paneelement->panesize.x = panestate.openblock.screenrect.max.x -
                               panestate.openblock.screenrect.min.x;

     paneelement->panesize.y = panestate.openblock.screenrect.max.y -
                               panestate.openblock.screenrect.min.y;
  }

 /*
  * add list element to end of list
  */
  LinkList_AddToTail(&(mainelement->paneanchor), &(paneelement->header));

 /*
  * install open window event handler for pane window if it is !ArtWorks style
  */
  if(!paneelement->paneflags.data.fixed)
     Event_Claim(event_OPEN, panewindow, event_ANY, PaneOpenEventHandler, paneelement);

  return(TRUE);
}
