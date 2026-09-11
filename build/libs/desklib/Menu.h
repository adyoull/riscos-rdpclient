/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Menu.h
    Author:  Copyright © 1993, 1994, 1995 Jason Williams
             Thanks to Shaun Blackmore, Tim Browse, and others for various
             bits and pieces, menu template ideas, etc, which were mostly
             not used for this simple implementation, but may be used in
             the future.
    Version: 1.05 (17 Apr 2005)
    Purpose: Equivalent of RISC OS Lib's "menus" functions - Give a simple
             method for creating and showing menus.

    Notes:   These are simple menu create and show functions along very
             similar lines to RISC OS Lib's 'menu' functions. However, there
             are major differences -
               + menu items are ALWAYS indexed starting from 0.
               + menu pointers point to real Wimp menu structures, no
                 messing about
               + Menu_Show is provided to show the menus for you, including
                 placing a menu in the correct position over the icon bar.
*/


#ifndef __dl_menu_h
#define __dl_menu_h

/* Abstract */
/*
  This header declares some simple functions for creating and displaying
  Wimp menus.

  Remember that menu item numbering starts at zero (as you should be used
  to in C).
*/


#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifndef __dl_event_h
#include "DeskLib:Event.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


extern menu_ptr Menu_New(const char *title, const char *description);
/*
  This creates a new Wimp menu with the given title whose contents are
  defined by the given description.

  A menu description consists of a list of the text for the items,
  separated by commas for normal items or "pipes" ('|') for items with
  dotted lines between them.  Obviously this means you can't use commas
  or pipes in your menu items.

  The items themselves can be preceded by option characters to set flags
  for the given item.  '!' means the item will be ticked, '~' means it
  will be shaded (greyed out). Leading spaces are ignored.

  An example is "!Ticked,~Disabled,Sub-menu,!~Ticked and disabled, Normal".

  If this cannot allocate enough memory, it calls Error_OutOfMemory and then
  returns NULL, otherwise it returns a pointer to the created menu.
*/


extern menu_ptr Menu_Extend(menu_ptr menu, const char *description);
/*
  If possible, this extends the menu by appending the items in 'description'
  onto the end of the menu structure of 'menu'.  Like realloc, it returns
  a new pointer which is either a pointer to the new menu or else a pointer
  to the old menu structure, unaltered, if it couldn't extend the old one.

  See Menu_New for details of the description string syntax.
*/


extern menu_ptr Menu_NewEmpty(const char *title, int numitems);
/*
  This creates an empty menu with the given title and number of entries.
  This is useful if you need to dynamically create a menu - you can
  create an empty menu, and then fill in the text with Menu_SetText
  rather than having to build a descriptor string.

  If it cannot allocate memory for the menu, it returns NULL.
*/


extern void Menu_Show(menu_ptr menu, int x, int y);
/*
  This shows the given menu on screen at the given x and y positions.
  If you pass y = -1 it will open the menu at the correct height for
  an iconbar icon's menu.
*/


extern void Menu_ShowLast(void);
/*
  This reopens the last menu opened at its previous position.  You can call
  this when a menu item is clicked on with adjust to keep the menu open.

  If no menu has been opened, or the menu no longer exists, this will have
  dire results.
*/


extern void Menu_PopUp(menu_ptr menu,window_handle window,icon_handle icon);
/*
   This shows a menu just beside the given icon, as in the Acorn style guide
   for popups (who cares if it's a pain in the ass moving the pointer
   all the way over there instead of having the menu pop up in a useful
   place, right? ;-)
*/

extern void Menu_PopUpAuto(menu_ptr menu);
/*
  This is a variation on Menu_PopUp which decides where to open the menu
  based on the last event processed by Event_Process.  If there is no
  icon information from the last event, nothing happens.
*/


extern void Menu_AddSubMenu(menu_ptr menu, int entry, menu_ptr submenu);
/*
  This attaches the given submenu to an existing menu at the given item.
  Note that if you use Menu_Extend on the submenu, you will have to
  reattach the new menu in place of the old one.
*/


extern void Menu_AddSubWindow(menu_ptr menu, int entry, window_handle window);
/*
  This attaches a window to an existing menu at the given item.  This results
  in the window acting as a submenu - good for dialogue boxes or (more usually)
  "Info" windows on the iconbar menu.
*/


extern void Menu_MakeIndirected(menu_ptr menu, int entry,
                                char *buffer, int size);
/*
  This converts a menu item into an indirected item, setting it to use the
  given buffer (and thus setting it to use the text in that buffer).
*/


extern void Menu_MakeWritable(menu_ptr menu, int entry,
                               char *buffer, int size, char *valid);
/*
  This make a menu item writable.  You supply the buffer into which the
  data will be written as well as the size of that buffer.  You also
  supply a validation string (as for a Wimp icon).
*/


extern void Menu_SetFlags(menu_ptr menu, int entry, int ticked, int shaded);
/*
  This alters the state of the given  menu item.  Passing 0 for one or more of
  the options turns those options off (eg. making it unticked, or selectable)
  and passing 1 turns them on.  You must pass either 0 or 1 for each option,
  or the behaviour is undefined.
*/


extern void Menu_GetFlags(menu_ptr menu, int entry, int *ticked, int *shaded);
/*
  This reads the current state of the given menu item.  If 'ticked' or 'shaded'
  are non-NULL pointers then the ints will be updated to 1 if the option is
  on, or zero if they are off.
*/


extern void Menu_SetText(menu_ptr menu, int entry, const char *text);
/*
  This sets the text for a menu entry.  The entry does not have to be indirected
  to do this, as a buffer will be malloced for the new entry if there is not
  enough space currently.
*/


extern char *Menu_GetText(menu_ptr menu, int entry);
/*
  This finds the address of the text of a menu entry.  It actually returns the
  menu item text buffer, so treat it with care.
*/


extern int Menu_CalcHeight(menu_ptr menu);
/*
  This calculates the height of a menu in OS coordinates.
  It is meant for internal use.
*/


#define Menu_SysHandle(menu) (menu)
/*
  MACRO: menu_ptr *Menu_SysHandle(menu_ptr *menu);

  Returns a pointer to the underlying Wimp menu definition, which is what you
  passed in.  This was a compatibilty thing for RISCOSLib.
*/


#define Menu_SDispose(menu) free(menu)
/*
  MACRO: void Menu_SDispose(menu_ptr *menu);

  This frees the memory used by a menu.  This just frees the memory for the
  one menu, it doesn't dispose of attached submenus or indirected menu item
  text.
*/


extern void Menu_FullDispose(menu_ptr menu);
/*
  This does a full freeing of memory associated with a menu.  It doesn't
  free submenus (they could be used elsewhere as well) but it does call
  free() for the indirected menu items.
*/


extern void Menu_RemoveItem(menu_ptr menu, int entry);
/*
  This removes the given menu item from the menu and shuffles the other
  items up to give a slightly shorter menu.  It will not remove the only
  item in a menu.
*/


extern void Menu_Warn(menu_ptr menu, int entry, BOOL yesno,
                      event_handler handler, void *reference);
/*
  This sets up the given menu item so that you receive a MenuWarn message
  when the user moves the mouse pointer over the submenu arrow.  If yesno
  is TRUE is sets the MenuWarn flag, if it is FALSE it unsets it.  Note that
  this doesn't add a submenu (or the corresponding arrow on the menu), you
  must call Menu_AddSubMenu for that.

  If you wish to attach/detach a message handler at the same time, you
  can pass an event_handler in 'handler', otherwise set this to NULL.

  Note that the handler is not attached to this particular menu or entry,
  it is attached to the MenuWarn message you receive for any such menu item.
*/


extern void Menu_ReviseWidth(menu_ptr menu);
/*
  This revises the width of the given menu by scanning through the title
  and items.  It is assumed there is at least one menu item. You use
  this after calling Menu_SetText or changing the text of an indirected
  menu item to make sure the menu is the correct size.
*/


#define Menu_ForceClose() Wimp_CreateMenu((menu_ptr)-1, 0, 0)
/*
  MACRO: void Menu_ForceClose(void);

  This simply forces the Wimp to close *all* open menus (or transient
  dialogue boxes opened as menus).
*/


#define	Menu_FirstItem(menu) ((menu_item *) (((menu_ptr) (menu) + 1)))
/*
  MACRO: menu_item *Menu_FirstItem(menu_ptr *menu);

  This returns a pointer to the first item in the given menu.
*/


#define Menu_FontMenu_NOTICK      0
/* Don't tick any entry in the menu */


#define Menu_FontMenu_TICKSYSFONT ((char *)1)
/* Tick the system font entry */


extern menu_ptr Menu_FontMenu(BOOL sysfont, char *tick);
/*
  This returns a pointer to a font menu, or zero if it couldn't make one.
  If you want the system font at the top of the menu, set sysfont to TRUE.
  Pass a font name in 'tick' to tick that font in the menu, or else
  Menu_FontMenu_NOTICK or Menu_FontMenu_TICKSYSFONT.

  It is probably not a good idea to attempt to free the memory for this
  menu.
*/


extern menu_ptr Menu_FontMenu3(BOOL sysfont, char *tick);
/*
  This returns a pointer to a font menu, or zero if it couldn't make one.
  If you want the system font at the top of the menu, set sysfont to TRUE.
  Pass a font name in 'tick' to tick that font in the menu, or else
  Menu_FontMenu_NOTICK or Menu_FontMenu_TICKSYSFONT.

  It is probably not a good idea to attempt to free the memory for this
  menu.
*/


extern char *Menu_FontMenuDecode(int *selection);
/*
  This returns a pointer to the name of the font which has been selected
  by the given menu selection, or NULL if an error occurred.
*/


extern char *Menu_FontMenuDecode3(int *selection);
/*
  This returns a pointer to the name of the font which has been selected
  by the given menu selection, or NULL if an error occurred.
*/


extern menu_ptr menu_fontmenu;
/*
  This is used to compare against menu_currentopen when you attempt to
  decode a font menu suggestion.
*/


extern wimp_point menu_currentpos;
/*
  This holds the x,y position where the last menu was opened. If no menu
  has been opened then this value will be invalid.  Use with caution.
*/


extern menu_ptr menu_currentopen;
/*
  This is a pointer to the last menu opened.  If no menu has
  been opened, or the last menu no longer exists then this value
  will be invalid.  Use with caution.
*/


#ifdef __cplusplus
}
#endif


#endif
