/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Menu2.h
    Author:  Copyright © 1995 Julian Smith
    Version: 1.01 (17 Apr 2005)
    Purpose: Easy menu handling
*/


#ifndef __dl_menu2_h
#define __dl_menu2_h

#ifndef __dl_menu_h
#include "DeskLib:Menu.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  The aim of Menu2 is to provide a higher-level interface to menus than
  the Menu library.  It handles menu choices, setting of menu flags,
  submenus and dynamically created menus through a set of callback
  functions. If you choose to use Menu2, you should call Menu2_Create
  for every menu you are going to use in the app at start-up, and store
  the returned pointers.

  You can use the pointers to open the menu using Menu2_Open, or to
  specify what submenu should be opened when required.  Menu2 creates
  the menu blocks for menus when they are opened, and destroys them when
  they are closed, to save memory.  You can also "attach" a menu to an
  particular icon so that it is opened automatically when the icon is
  clicked on.
*/


typedef int menu2_handle;
/*
  This is used to identify a Menu2 menu,
*/


typedef menu_ptr (*menu2_makefn)(void *reference);
/*
  Functions of this type should make a menu and return a pointer
  to it.  This allows you to create "dynamic menus" which rely
  on some of the fields being filled in differently depending on
  the current selection or icon clicked on - like in the Filer for
  instance.

  'reference' is whatever reference you gave to Menu2_Create.
*/


typedef void (*menu2_flagsfn)(menu_ptr menu, void *reference);
/*
  This function-type should set any flags which need to be set in
  response to an adjust-click on the menu.  It will be called whenever
  the menu is opened or re-opened, and passes you the reference you gave
  to Menu2_Create and a pointer to the menu being updated.
*/


typedef void (*menu2_selectfn)(int itemnum, void *reference);
/*
  Functions of this type are called whenever a choice is made from a Menu2
  menu.  'itemnum' is the menu item-number which was chosen, and
  'reference' is whatever reference you passed to Menu2_Create.

  If you need to know more about which menu was clicked on or the event
  corresponding wimp event, you should use the supplied globals such as
  event_lastevent and menu_currentopen.
*/


typedef menu2_handle (*menu2_subfn)(int itemnum, event_pollblock *event,
                                    void *reference);
/*
  This function-type is called whenever a submenu is required.  'itemnum'
  is the number of the menu item which needs a submenu and event is the
  pollblock corresponding to the MenuWarn message which prompted the
  function call.  'reference' is the reference you passed to Menu2_Create.

  If you wish to open a sub-window or sub-menu yourself rather than letting
  Menu2 do it, you should return NULL so that it doesn't attempt to re-open
  the already-opened window.
*/


menu2_handle Menu2_Create(const char *title, const char *spec,
                          menu2_makefn makefn, menu2_flagsfn flagsfn,
                          menu2_subfn subfn, menu2_selectfn selectfn,
                          void *reference);
/*
  This returns a unique handle for the newly created menu.  You should store
  this so that you can open the menu later.  It returns zero if it cannot
  make the menu for any reason (eg. running out of memory).

  It makefn is NULL, the menu will be constructed directly from the 'title'
  and 'spec' when it is needed, with those parameters being passed to
  Menu_Create.  Note that these strings are not copied, so they should be
  "permanent strings".  If makefn is non-NULL, the given function will be
  called to create the menu.

  If any of the other functions specified are non-NULL, they are called when
  a menu is opened or re-opened as with an adjust-click (flagsfn) when a
  menuwarn message is received to open a submenu or window (subfn) or when
  a selection is made from the menu (selectfn).

  All the callback functions are passed the reference you specify here.
*/


menu2_handle Menu2_CreateFromMsgs(const char *titletag, const char *spectag,
                                  menu2_makefn makefn, menu2_flagsfn flagsfn,
                                  menu2_subfn subfn, menu2_selectfn selectfn,
                                  void *reference);
/*
  This is identical in function to Menu2_Create, but it allows you to
  specify the title and menu spec with tags from a message file which
  has been loaded with Msgs_LoadFile.
*/


void Menu2_Open(menu2_handle handle, int x, int y);
/*
  This opens the specified Menu2 menu at the x and y positions given.
  This calls Menu_Show, so if you want an iconbar menu pass -1 as y.
*/


void Menu2_AttachMenu(window_handle window, icon_handle icon,
                      menu2_handle menu, int button);
/*
  This uses Event_Claim to open the given Menu2 menu when the specified icon
  is clicked on with the specified button(s).  If the click is on the
  iconbar, the menu is opened at the correct height for an iconbar menu.

  The mouse buttons are button_SELECT, button_MENU and button_ADJUST, and you
  can specify more than one by bitwise ORing them.
*/


void Menu2_DetachMenu(window_handle window, icon_handle icon,
                      menu2_handle menu, int button);
/*
  This reverses the action of Menu2_AttachMenu.  It stops the menu being
  opened when you click on the icon with the given button or buttons.

  The buttons are dealt with individually, so you can remove the menu
  when it is clicked with one button but not another.  However, you should
  not try to detach a menu from an icon/mouse button combination you
  didn't previously attach it to.
*/


#ifdef __cplusplus
}
#endif


#endif
