/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Wimp.h
    Author:  Copyright � 1992, 1993, 1994 John Winters and Jason Williams
    Version: 1.13 (17 Apr 2005)
    Purpose: Type/structure definitions for Wimp SWI interface
*/


#ifndef __dl_wimp_h
#define __dl_wimp_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This file contains type and structure definitions for the Wimp SWI
  interface, which are used in many different places in DeskLib -
  Event, Menu, Coord, and so on.
*/


#define wimp_MAXNAME 12
/*
  The maximum size of non-indirected wimp names
  (icon text fields, sprite names, etc).
*/


typedef struct
{
  int x, y;
} wimp_point;
/*
  A C struct which corresponds with how the RISC OS Window Manager usually
  represents (x,y) points.
*/


typedef wimp_point wimp_coord;
/*
  A type to represent a single screen coordinate (x,y).
*/


typedef struct
{
  wimp_point min;
  wimp_point max;
} wimp_rect;
/*
  A C struct which corresponds with how the RISC OS Window Manager usually
  represents rectangles.
*/


typedef wimp_rect wimp_box;
/*
  A type to correspond with how RISC OS normaly represents boxes.
*/


typedef int window_handle;
/*
  A window handle (the unique number used to identify a Wimp window).
*/


typedef int icon_handle;
/*
  An icon handle (the unique number used to identify each icon in
  a given window).
*/


typedef int task_handle;
/*
  A task handle (the unique number used to identify a multitasking
  application by RISC OS).
*/


/* -------------------------------------------------------------------------
   Wimp colours and palettes
   ------------------------------------------------------------------------- */

#define colour_WHITE        0
#define colour_GREY0        0
#define colour_GREY1        1
#define colour_GREY2        2
#define colour_GREY3        3
#define colour_GREY4        4
#define colour_GREY5        5
#define colour_GREY6        6
#define colour_GREY7        7
#define colour_BLACK        7
#define colour_DARK_BLUE    8
#define colour_YELLOW       9
#define colour_GREEN       10
#define colour_RED         11
#define colour_CREAM       12
#define colour_ARMY_GREEN  13
#define colour_ORANGE      14
#define colour_LIGHT_BLUE  15

#define colour_TRANSPARENT 255
#define colour_NOBORDER    255
/* This colour represents the absence of a border in the nested wimp. */

typedef union
{
  int value;
  struct
  {
    unsigned int wimpcolour : 4;
    unsigned int gcolaction : 3;
    unsigned int foreback   : 1;
    unsigned int padding    : 24;
  } data;
} wimp_colour;
/*
  This holds information about a wimp colour, including the gcol action
  (ie. whether it sets the colour, EORs with the current colour, etc),
  and whether it is a background colour (foreback = 1), or a foreground
  colour (foreback = 0).
*/


typedef union
{
  int value;
  struct
  {
    char gcol;
    char red;
    char green;
    char blue;
  } data;
} palette_entry;
/*
  This holds an RGB colour in the correct form for the ColourTrans SWIs.
*/


typedef struct
{
  palette_entry colour[16];
  palette_entry border;
  palette_entry mouse1;
  palette_entry mouse2;
  palette_entry mouse3;
} palette_block;
/*
  This is a Wimp desktop palette block.  'border' is the colour of the
  border around the edge of the screen, 'mouse1'..'mouse3' define the
  pointer colours.
*/


/* Icon button types */

#define iconbtype_NEVER            0
/* The icon returns no click events except menu clicks */

#define iconbtype_ALWAYS           1
/* The icon reports an event all the time the pointer is over the icon */

#define iconbtype_AUTOREPEAT       2
/*
  The icon reports events when the mouse is clicked and regularly
  for as long as the mouse button is held down.
*/

#define iconbtype_CLICK            3
/*
  The icon reports an event when any mouse button is pressed over the icon.
*/

#define iconbtype_RELEASE          4
/*
  A click on the icon selects it, moving away deselects it and releasing
  the button generates an event.
*/

#define iconbtype_DOUBLECLICK      5
/* A click selects the icon, a double-click generates an event. */

#define iconbtype_CLICKDRAG        6
/*
  A click generates an event.  If a drag starts, it generates an event
  with the button code the normal click code * 16.
*/

#define iconbtype_RELEASEDRAG      7
/*
  A click selects the icon, and releasing the button generates an
  event.  If a drag starts, it generates an event with the button
  code the normal click code * 16.
*/

#define iconbtype_DOUBLEDRAG       8
/*
  A click selects the icon, and a double-click generates an event.
  If a drag starts, it generates an event with the button code
  the normal click code * 16.
*/

#define iconbtype_MENU             9
/*
  The icon is selected when the pointer is over the icon (like a menu
  item), and clicking generates an event.
*/

#define iconbtype_DOUBLECLICKDRAG 10
/*
  A click generates an event, with the button code the normal click
  code * 256.  A drag starting generates the click code * 16, and a
  double-click generates the normal click code.
*/

#define iconbtype_RADIO           11
/*
  A click selects and generates an event, a drag generates an event
  with the click code * 16.
*/

#define iconbtype_RESERVED1       12
/* A reserved button type.  Do not use. */

#define iconbtype_RESERVED2       13
/* A reserved button type.  Do not use. */

#define iconbtype_WRITECLICKDRAG  14
/*
  A click causes the icon to gain the caret to edit the text in it.
  A drag starting generates an event with click code * 16.
*/

#define iconbtype_WRITABLE        15
/* A click causes the icon to gain the caret. */


#define iconbar_RIGHT -1
/*
  Specifies that a created icon should be on the right hand side
  of the iconbar.
*/

#define iconbar_LEFT  -2
/*
  Specififes that a created icon should be on the left hand side
  of the iconbar.
*/

#define window_ICONBAR -2
/*
  The window handle of the iconbar, for click events and suchlike.
*/


/* Icon flag bits */
#define icon_TEXT        0x00000001      /* icon contains text               */
#define icon_SPRITE      0x00000002      /* icon is a sprite                 */
#define icon_BORDER      0x00000004      /* icon has a border                */
#define icon_HCENTRE     0x00000008      /* text is horizontally centred     */
#define icon_VCENTRE     0x00000010      /* text is vertically centred       */
#define icon_FILLED      0x00000020      /* icon has a filled background     */
#define icon_FONT        0x00000040      /* text is an anti-aliased font     */
#define icon_NEEDSHELP   0x00000080      /* redraw needs application's help  */
#define icon_INDIRECTED  0x00000100      /* icon data is 'indirected'        */
#define icon_RJUSTIFY    0x00000200      /* text right justified in box      */
#define icon_ALLOWADJUST 0x00000400      /* Allow multiple select with adjust*/
#define icon_HALVESPRITE 0x00000800      /* plot sprites half-size           */
#define icon_BUTTONTYPE  0x00001000      /* 4-bit field: button type         */
#define icon_SELECTED    0x00200000      /* icon selected by user (inverted) */
#define icon_SHADED      0x00400000      /* icon cannot be selected (shaded) */
#define icon_DELETED     0x00800000      /* icon has been deleted            */

#define icon_FORECOLOUR  0x01000000      /* 4-bit field: foreground colour   */
#define icon_BACKCOLOUR  0x10000000      /* 4-bit field: background colour   */

#define iconvalid_ALLOW           'A'
/* Specifies the characters that are allowed in the icon */

#define iconvalid_BORDERTYPE      'R'
/* Specifies the 3D border style */

#define iconvalid_DISPLAY         'D'
/*
  Specifies that all typed characters in the icon whould be displayed
  as the character given immediately after this.  eg. for a password
  field.
*/

#define iconvalid_FONTCOLOURS     'F'
/* Specifies the colours for an icon containing anti-aliased text. */

#define iconvalid_LINESPACE       'L'
/*
  Specifies the linespacing for a multi-line text field.
  This can only be used with icons which are horizontally and vertically
  centred, not antil-aliased text and not writeable.
*/

#define iconvalid_SPRITENAME      'S'
/*
  Specifies a sprite (or pair of sprites) to use with a text-and-sprite
  indirected icon.
*/

#define iconborder_PLINTH          0
#define iconborder_SURROUND        1
#define iconborder_OKBUTTON        2
#define iconborder_INDENT          3


typedef union
{
  unsigned int value;

  struct
  {
    unsigned int garbage1      : 6;    /* New struct to allow easy access to */
    unsigned int font          : 1;    /* the font handle of an icon. NOTE   */
    unsigned int garbage2      : 17;   /* that handle is ONLY valid IF       */
    unsigned int handle        : 8;    /* 'font' is TRUE.                    */
  } font;

  struct
  {
    unsigned int text          : 1;
    unsigned int sprite        : 1;
    unsigned int border        : 1;
    unsigned int hcentre       : 1;
    unsigned int vcentre       : 1;
    unsigned int filled        : 1;
    unsigned int font          : 1;
    unsigned int needshelp     : 1;
    unsigned int indirected    : 1;
    unsigned int rightjustify  : 1;
    unsigned int allowadjust   : 1;
    unsigned int halfsize      : 1;
    unsigned int buttontype    : 4;
    unsigned int esg           : 5;
    unsigned int selected      : 1;
    unsigned int shaded        : 1;
    unsigned int deleted       : 1;
    unsigned int foreground    : 4;
    unsigned int background    : 4;
  } data;
} icon_flags;
/*
  This holds the flags for a Wimp icon, allowing access to the various
  flags with bitfields in the union, if you so desire.
*/


typedef struct
{
  char         *buffer;
  char         *validstring;
  unsigned int bufflen;
} icon_indtext;
/*
  Defines the icon data for an indirected text icon, giving a pointer
  to the indirected text buffer, the length of that buffer and a
  pointer to the validation string for the icon.
*/


typedef struct
{
  const char   *name;
  void         *spritearea;
  unsigned int nameisname;
} icon_indsprite;
/*
  Defines the icon data for an indirected sprite icon, giving a pointer
  to the sprite name, a pointer to the sprite area containing the sprite
  (or 1 for the Wimp sprite area) and 'nameisname', which should be the
  length of the buffer holding the sprite name or zero if the 'name'
  field is actually a sprite pointer.
*/


typedef union
{
  char           text[wimp_MAXNAME];
  icon_indtext   indirecttext;
  char           spritename[wimp_MAXNAME];
  icon_indsprite indirectsprite;
} icon_data;
/*
  This represents the different information that can be held in
  the data part of an icon definition, depending on the flags set
  for the icon.
*/


typedef struct
{
  wimp_box   workarearect;
  icon_flags flags;
  icon_data  data;
} icon_block;
/*
  This is an Wimp icon block, used when creating an icon and
  when you read information about an icon.  It defines the position
  of the icon on the window, the properties of the icon, and the
  text or sprite it contains.
*/


typedef struct
{
  window_handle window;
  icon_block    icondata;
} icon_createblock;
/*
  This specifies everything needed to create an icon - the icon data
  and the window it is to be created in.
*/


/* -------------------------------------------------------------------------
   Window data:
   ------------------------------------------------------------------------- */

typedef union
{
  struct
  {
    unsigned int hastitle          : 1;  /* This is an "old-style" flag -
                                            don't use it */
    unsigned int moveable          : 1;
    unsigned int hasvscroll        : 1;  /* This is an "old-style" flag -
                                            don't use it */
    unsigned int hashscroll        : 1;  /* This is an "old-style" flag -
                                            don't use it */
    unsigned int autoredraw        : 1;
    unsigned int pane              : 1;
    unsigned int nobounds          : 1;
    unsigned int nobackclose       : 1;  /* This is an "old-style" flag -
                                            don't use it */
    unsigned int scrollrq          : 1;
    unsigned int scrollrqdebounced : 1;
    unsigned int realcolours       : 1;
    unsigned int backwindow        : 1;
    unsigned int hotkeys           : 1;
    unsigned int keeponscreen      : 1;
    unsigned int ignoreright       : 1;
    unsigned int ignorebottom      : 1;
    unsigned int open              : 1;
    unsigned int top               : 1;
    unsigned int fullsize          : 1;
    unsigned int istoggling        : 1;
    unsigned int focus             : 1;
    unsigned int forceonce         : 1;
    unsigned int toggleiconbar     : 1;
    unsigned int childfurniture    : 1;
    unsigned int backicon          : 1;
    unsigned int closeicon         : 1;
    unsigned int titlebar          : 1;
    unsigned int toggleicon        : 1;
    unsigned int vscroll           : 1;
    unsigned int adjusticon        : 1;
    unsigned int hscroll           : 1;
    unsigned int newflags          : 1;  /* This should always be TRUE */
  } data;
  int value;
} window_flags;
/*
  This holds the flags for a Wimp window, allowing access to the various
  flags with bitfields, if desired.

  You should always use new-style flags (they aren't really new any more)
  which means setting 'newflags' to 1, 'hastitle', 'hasvscroll', 'hashscroll'
  and 'nobackclose' should all be set to zero.  You will usually set up
  your windows using a template editor, and so do not have to worry about
  setting the values in this form.
*/


typedef struct                /* Minimum width/height of window. Used to be: */
{                             /*   unsigned short minwidth, minheight;       */
  unsigned int x : 16;        /* This has been changed for compatability     */
  unsigned int y : 16;        /* with compilers other than Desktop C         */
} window_minsize;
/*
  This specifies a minimum size for a Wimp window - this is the size
  beyond which the Wimp will refuse to make the window any smaller when
  it is being resized.
*/

typedef union
{
  struct
  {
    unsigned char titlefore; /* 0xff means no border and foreground is 7 */
    unsigned char titleback;
    unsigned char workfore;
    unsigned char workback;  /* 0xff means Wimp won't clear              */
    unsigned char scrollouter;
    unsigned char scrollinner;
    unsigned char titlefocus;
    /* The 8th byte holds the extended window flags (bit0 fullcolour,
       bit1 extendedscroll, bit2 never3d, bit3 always3d, bit4 returnshaded).
       This was originally `unsigned int` bitfields, but ANSI C only allows int
       bitfields and this modern Norcroft then word-aligns them at offset 8,
       making the struct 12 bytes and shifting numicons/icons by 4 (which
       corrupts template loading). A single byte keeps it 8 bytes, matching the
       Wimp window-block format. No code references the individual flags. */
    unsigned char extflags;
  } cols;

  struct {
    unsigned char colours[7];
    unsigned char extra;
  } vals;
} wimp_colourflags;


typedef struct
{
  wimp_box         screenrect;
  wimp_point       scroll;
  window_handle    behind;
  window_flags     flags;
  wimp_colourflags colours;
  wimp_box         workarearect;
  icon_flags       titleflags;
  icon_flags       workflags;
  void             *spritearea;
  window_minsize   minsize;
  icon_data        title;
  unsigned int     numicons;

  /* ANSI C doesn't allow this.  For Norcroft, you can use -Ez */
#if defined(DESKLIB_zeroarray)
  icon_block     icons[0];
#endif
} window_block;
/*
  This represents the definition of a Wimp window.  It contains details
  about the position on the screen, the locations of the scroll bars,
  the position in the window stack, colours, flags, and so on.

  It is followed immediately in memory by the icon blocks defining the
  icons for this window.  This is represented by the array of icon blocks
  at the end of the definition.  This will only be defined if you have
  defined DESKLIB_zeroarray and are using GCC, or Norcroft with the option
  -Ez, as zero-length arrays (representing an array of unknown length)
  are not part of ANSI/ISO C.
*/


typedef struct
{
  window_handle window;
  wimp_box      screenrect;
  wimp_point    scroll;
  window_handle behind;
} window_openblock;
/*
  This specifies the position of the window on the screen
  and is used with Wimp_OpenWindow to show a
  window on the screen.
*/


typedef struct
{
  window_openblock openblock;
  window_flags     flags;
} window_state, window_openblocknest;
/*
  This holds all the information about the current state of a window.

  When used with Wimp_OpenWindowNest, the supplied flags will update
  the window if 'updateflags' is set in the supplied nested flags
*/


typedef struct
{
  window_handle window;
  window_block  block;
} window_info;
/*
  This holds information about a window, as returned by Wimp_GetWindowInfo.
*/


typedef struct
{
  window_handle window;
  wimp_box      rect;
  wimp_point    scroll;
  wimp_box      cliprect;
} window_redrawblock;
/*
  This defines an area of a window to be redrawn.  It is used
  with Wimp_ForceRedraw and Wimp_UpdateWindow, for example.
*/


typedef struct
{
  window_handle window;
  wimp_box      screenrect;
} window_outline;
/*
  This defines the area of the screen which is the bounding box of
  the window in OS coordinates (including any scrollbars, titlebar, etc).
*/


typedef union
{
  struct
  {
    unsigned int updateflags : 1;   /* Upate normal window flags too */
    unsigned int reserved    : 15;
    unsigned int leftedge    : 2;
    unsigned int bottomedge  : 2;
    unsigned int rightedge   : 2;
    unsigned int topedge     : 2;
    unsigned int xscroll     : 2;
    unsigned int yscroll     : 2;
  } flags;
  int value;
} window_nestflags;
/*
  This holds the nesting flags for a window opened within another
  window under the nested Wimp.  The possible values for the flags
  are given by the enumerated type windownest_align.  These determine
  what part(s) of the parent window the child window is linked to.
*/


typedef enum
{
  windownest_WORKAREA   = 0,
  windownest_LEFTBOTTOM = 1,
  windownest_RIGHTTOP   = 2
} windownest_align;
/*
  These enumerated constants are used to give meaningful names to
  the ways parts of a child window can be linked to its parent.
*/


typedef enum
{
  windowenum_PARENT = 6,
  windowenum_FRONT  = 7,
  windowenum_BACK   = 8,
  windowenum_BEHIND = 9,
  windowenum_INFRONT  = 10
} window_enumstack;
/*
  This gives meaningful names to the types of information we can
  read about the window stack, as available under the nested Wimp.
  Used with Wimp_Enumerate.
*/


/* -------------------------------------------------------------------------
   Mouse/Pointer information:
   ------------------------------------------------------------------------- */

#define button_ADJUST      0x00000001
/* The button code for an adjust-click */

#define button_MENU        0x00000002
/* The button code for a menu-click */

#define button_SELECT      0x00000004
/* The button code for a select-click */

#define button_DRAGADJUST  0x00000010
/* The button code for a drag with the adjust mouse button */

#define button_DRAGSELECT  0x00000040
/* The button code for a drag with the select mouse button */

#define button_CLICKADJUST 0x00000100
/*
  The button code for a click with the adjust button when received on
  a double/click/drag button.
*/

#define button_CLICKSELECT 0x00000400
/*
  The button code for a click with the select button when received on
  a double/click/drag button.
*/

typedef union
{
  unsigned int value;
  struct
  {
    unsigned int adjust       : 1;
    unsigned int menu         : 1;
    unsigned int select       : 1;
    unsigned int dummy3       : 1;
    unsigned int dragadjust   : 1;
    unsigned int dummy5       : 1;
    unsigned int dragselect   : 1;
    unsigned int dummy7       : 1;
    unsigned int clickadjust  : 1;
    unsigned int dummy9       : 1;
    unsigned int clickselect  : 1;
  } data;
} button_state;
/*
  This holds the button state, ie. what button was used to create a
  click event.  It allows bitfield access to the flags saying which
  button caused the event.  The clickxxx fields represent a single click
  on a double/click/drag button, and the dragxxx fields represent the start
  of a drag on any draggable icon.
*/

typedef struct
{
  wimp_point    pos;
  button_state  button;
  window_handle window;
  icon_handle   icon;
} mouse_block;
/*
  This is used to hold the current mouse state, as read with
  Wimp_GetPointerInfo or returned with a Poll event.  It gives the
  screen position of the pointer, the state of the mouse buttons
  and what window and icon were clicked on.
*/


typedef struct
{
  int           shapenumber;
  char          *shapedata;
  unsigned int  width;
  unsigned int  height;
  wimp_point    active;
} pointer_shapeblock;
/*
  This defines a shape and active point for the mouse pointer.
*/


/* -------------------------------------------------------------------------
   Wimp drag data:
   ------------------------------------------------------------------------- */

typedef enum
{
  drag_MOVEWINDOW    = 1,
  drag_RESIZEWINDOW,
  drag_SCROLLX,
  drag_SCROLLY,
  drag_FIXEDBOX,
  drag_RUBBERBOX,
  drag_INVISIBLE,
  drag_USERFIXEDBBOX,
  drag_USERRUBBERBOX,
  drag_USERFIXEDNC,
  drag_USERRUBBERNC
} drag_type;
/*
  This gives meaningful names to the various types of drags that can be
  initiated with the Wimp_DragBox SWI.  The "NC" drag types are not
  cancelled when the buttons are released.  None of the "USER" drag
  types are directly supported by DeskLib.
*/


typedef struct
{
  window_handle window;
  drag_type     type;
  wimp_box      screenrect;
  wimp_box      parent;
} drag_block;
/*
  This holds information needed to start a Wimp_DragBox operation.
  You only need the 'window' filled in for the first four drag types,
  as they directly affect the window's position or size.  'screenrect'
  defines the initial size and position of the dragbox, and 'parent'
  defines the limits imposed on the drag where appropriate.
*/


/* -------------------------------------------------------------------------
   Wimp errors:
   ------------------------------------------------------------------------- */

typedef union
{
  unsigned int value;
  struct
  {
    unsigned int ok               : 1;
    unsigned int cancel           : 1;
    unsigned int highlightcancel  : 1;
    unsigned int noprompt         : 1;
    unsigned int noprefix         : 1;
    unsigned int nowait           : 1;
    unsigned int closebox         : 1;
  } data;
} error_flags;
/*
  These are the flags to set up the behaviour of a standard Wimp
  non-multitasking error box. 'noprompt' means that you won't get the
  "Press space or click mouse to continue" prompt.  'noprefix' means that
  the title will not be prefixed by "Error from".
*/


/* -------------------------------------------------------------------------
   Menus
   ------------------------------------------------------------------------- */

typedef struct menu_block *menu_ptr;
/* See Wimp_CreateMenu, Wimp_CreateSubMenu	*/

typedef union
{
  unsigned int value;
  struct
  {
    unsigned int ticked     : 1;
    unsigned int dotted     : 1;
    unsigned int writable   : 1;
    unsigned int notifysub  : 1;
    unsigned int openshaded : 1;
    unsigned int dummy2     : 2;
    unsigned int last       : 1;
    unsigned int indtitle   : 1;
    unsigned int dummy13    : 13;
    unsigned int shaded     : 1;
    unsigned int dummy1     : 1;
    unsigned int foreground : 4;
    unsigned int background : 4;
  } data;
} menu_flags;
/*
  This holds the flags for a menu item, which decide the colours, whether the
  icon is selectable or greyed-out, whether the item is ticked, and so on.
*/


typedef union
{
  menu_ptr      menu;
  window_handle window;
  int           value;
} menu_submenu;
/*
  This is a union defining the type of things which can be placed in the
  'submenu' field of a menu item, namely a submenu, subwindow or -1 to
  show that there is no submenu attached.
*/


typedef struct
{
  menu_flags   menuflags;
  menu_submenu submenu;
  icon_flags   iconflags;
  icon_data    icondata;
} menu_item;
/*
  These follow a menu_block in memory, and define the items in the menu.
*/


typedef struct menu_block
{
  char     title [wimp_MAXNAME];
  char     titlefore, titleback, workfore, workback;
  unsigned int width;
  unsigned int height;
  unsigned int gap;
  /*  Followed immediately by menu items */
} menu_block;
/*
  This is a menu block, defining a Wimp menu.  The menu item blocks follow
  immediately after this in memory.  It can be displayed using
  Wimp_CreateMenu, or using the functions in the Menu library,
*/



/* -------------------------------------------------------------------------
   Caret/Keypress data:
   ------------------------------------------------------------------------- */

typedef int key_press;

typedef struct
{
  window_handle window;
  icon_handle   icon;
  wimp_point    offset;
  int           height;
  int           index;
} caret_block;
/*
  This holds information about the caret position.  If the caret is not in
  a window or icon, they are set to -1.  'offset' contains the caret offset
  relative to the window's work area origin.  'index' contains the index into
  the string in the icon, or -1 if not in an icon.

  'height' contains the height of the caret in OS units (in bits 0..15), and
  the flags for the caret: bit 24 means to use a VDU 5 caret instead of the
  anti-aliased one, bit 25 means the caret is invisible, bit 26 means that
  bits 16..23 are the colour of the caret, and bit 27 means that the given
  colour is a logical rather than a wimp colour.

  You normally do not need to access the flags (or the height).
*/


typedef struct
{
  caret_block caret;
  key_press   code;
} key_block;
/*
  This holds information about a keypress event in the event_data union.
*/


typedef struct
{
  int address;
  int contents;
} pollword_block;
/*
  This holds information about a non-zero-pollword event in the
  event_data union.
*/


/* -------------------------------------------------------------------------
   Wimp message data:
   ------------------------------------------------------------------------- */

typedef enum
{
  message_ANY            = -99, /* Ensure enum isn't unsigned int */
  message_QUIT           = 0,
  message_CLOSEDOWN      = 0,
  message_DATASAVE       = 1,
  message_DATASAVEACK    = 2,
  message_DATASAVEOK     = 2,
  message_DATALOAD       = 3,
  message_DATALOADACK    = 4,
  message_DATALOADOK     = 4,
  message_DATAOPEN,
  message_RAMFETCH,
  message_RAMTRANSMIT,
  message_PREQUIT,
  message_PALETTECHANGE,
  message_SAVEDESK,
  message_DEVICECLAIM,
  message_DEVICEINUSE,
  message_DATASAVED,

  message_CLAIMENTITY    = 15,
  message_DATAREQUEST    = 16,
  message_DRAGGING       = 17,
  message_DRAGCLAIM      = 18,
  message_RELEASEENITY   = 19,

  message_FILEROPENDIR   = 0x400,
  message_FILERCLOSEDIR,
  message_FILEROPENDIRAT,
  message_FILERSELECTIONDIRECTORY,
  message_FILERADDSELECTION,
  message_FILERACTION,
  message_FILERCONTROLACTION,
  message_FILERSELECTION,

  message_HELPREQUEST    = 0x502,
  message_HELPREPLY,

  message_NOTIFY         = 0x40040,
  message_MENUWARNING    = 0x400C0,
  message_MENUWARN       = 0x400C0,
  message_MODECHANGE,
  message_TASKINITIALISE = 0x400C2,
  message_INITTASK       = 0x400C2,
  message_TASKCLOSEDOWN  = 0x400C3,
  message_CLOSETASK      = 0x400C3,
  message_SLOTSIZE       = 0x400C4,
  message_SLOTCHANGE     = 0x400C4,
  message_SETSLOT        = 0x400C5,
  message_TASKNAMERQ,
  message_TASKNAMEIS,
  message_TASKSTARTED,
  message_MENUSDELETED,              /* RO3 Menu closed warning    */
  message_ICONISE,                   /* RO3 For window iconisation */
  message_WINDOWCLOSED,
  message_WINDOWINFO,

  message_POPUPRQ        = 0x46D40,  /* PopUp messages */
  message_POPUPREQUEST   = 0x46D40,
  message_POPUPSTATE,
  message_POPUPCLOSED,

  message_PRINTFILE      = 0x80140,
  message_WILLPRINT,
  message_PRINTSAVE,
  message_PRINTINIT,
  message_PRINTERROR,
  message_PRINTTYPEODD,
  message_PRINTTYPEKNOWN,
  message_SETPRINTER,

  message_PSPRINTERQUERY = 0x8014c,
  message_PSPRINTERACK,
  message_PSPRINTERMODIFIED,
  message_PSPRINTERDEFAULTS,
  message_PSPRINTERDEFAULTED,
  message_PSPRINTERNOTPS,
  message_RESETPRINTER,
  message_PSISFONTPRINTRUNNING,

  message_TASKWINDOW_INPUT = 0x808c0,
  message_TASKWINDOW_OUTPUT,
  message_TASKWINDOW_EGO,
  message_TASKWINDOW_MORIO,
  message_TASKWINDOW_MORITE,
  message_TASKWINDOW_NEWTASK,
  message_TASKWINDOW_SUSPEND,
  message_TASKWINDOW_RESUME



} message_action;
/*
  This gives meaningful names to some of the many Wimp user message codes,
  which are used to pass between between different applications and the OS.
*/


typedef int message_destinee;
/*
  A window or task handle identifying the task to which a message will be sent.
*/


typedef struct
{
  unsigned int   size;
  task_handle    sender;
  unsigned int   myref;
  unsigned int   yourref;
  message_action action;
} message_header;
/*
  This is the standard header for a Wimp message block.

  'size' should be the total size of the message block (including this header),
  rounded up to the nearest four bytes.  'sender' is the person who sent the
  message - this is filled in for you when you send the message.  'myref' and
  'yourref' help you to keep track of replies to messages.  'myref' is filled
  in for you when you send a message, 'yourref' should be 0 for an original
  message, or the reference of the message being replied to.

  The message action code determines the contents of the rest of the message.
*/


typedef struct
{
  window_handle window;
  icon_handle   icon;
  wimp_point    pos;
  int           estsize;
  int           filetype;
  char          leafname[212];
} message_datasave;
/*
  This contains the message data needed for a datasave request..

  'window', 'icon' and 'pos' are found using Wimp_GetPointerInfo when the
  drag to save the data ended.  'estsize' if the estimated size (in bytes)
  of the file to be saved.  'filetype' is the filetype you are intending to
  save, with -1 meaning untyped, 0x1000 meaning an application and
  0x2000 meaning a directory.  Finally, the suggested leafname (zero-terminated)
  should be in 'leafname'.
*/


typedef struct
{
  window_handle window;   /* window, icon, pos, estsize, filetype all copied */
  icon_handle   icon;     /* from the initiating datasave message            */
  wimp_point    pos;
  int           estsize;
  int           filetype;
  char          filename[212];
} message_datasaveack;
/*
  This holds the message data for the reply to a datasave message.  Everything
  but the filename should be copied from the datasave message it is a reply
  to, except that the 'estsize' may be -1 if it is "unsafe".  In this context,
  unsafe means it has not been written to a storage device, but merely been
  displayed by another task, for instance.
*/


typedef struct
{
  window_handle window;
  icon_handle   icon;
  wimp_point    pos;
  int           size;
  int           filetype;
  char          filename[212];
} message_dataload;
/*
  This holds the message data for a dataload request.
*/


typedef struct         /* DataLoadAck is identical to DataLoad */
{
  window_handle window;
  icon_handle   icon;
  wimp_point    pos;
  int           size;
  int           filetype;
  char          filename[212];
} message_dataloadack;
/*
  This holds the message data for a dataload reply.  It is identical to
  the dataload request.
*/


typedef message_dataload message_dataopen;
/*
  This holds the message data for a dataopen request.  It has the same
  structure as a dataload.
*/


typedef struct
{
  char *buffer;
  int  buffsize;
} message_ramfetch;
/*
  This holds the data for a ramfetch request.  It contains a pointer to
  a buffer where the receiver of the message should put the data, and the
  size of that buffer.
*/


typedef struct
{
  char *buffer;   /* copied directly from initiating RamFetch */
  int  byteswritten;
} message_ramtransmit;
/*
  This holds the data for the response to a ramfetch message - a ramtransmit
  message - which should be sent after the data has been copied with
  Wimp_TransferBlock.  The 'buffer' should be copied from the ramfetch
  message, and 'byteswritten' should be the amount of data written to the
  buffer.
*/


typedef struct
{
  mouse_block where;  /* Where is help needed for? */
} message_helprequest;
/*
  This message is sent by a task which wants to to be given the help text
  for the given location of the pointer.
*/


typedef struct
{
  char text[236];
} message_helpreply;
/*
  This is sent in response to to a helprequest message to supply the
  help for the location requested.  A "|M" within the text indicates a
  line break.
*/


typedef struct
{
  int filler[5];
  int filetype;
  char filename[212];
} message_print;
/*
  This holds the data for a PrintTypeOdd message.  This is sent when !Printers
  doesn't know how to print the given filetype.  If you know how to print it,
  you should respond with a PrintTypeKnown message and print it.
*/


typedef struct
{
  int        id;                    /* The number given to Wimp_CreateMenu */
  wimp_point openpos;               /* Position to open window/menu at     */
  int        selection[10];         /* -1 terminated list of menu hits     */
} message_menuwarn;
/*
  This holds the data for a menuwarn message, which is sent to you if you
  have set the appropriate flag for a menu item.  'id' is the number set in
  the submenu field of the item, 'openpos' is the (top left) location to
  open any submenu or window, and 'selection' is the list of selections
  for the currently open menus, terminated by a -1.
*/


typedef struct
{
  window_handle window;
  task_handle   task;
  char          title[20];
} message_iconize;
/*
  This is the data for an windowiconise message.  It is broadcast by the
  Wimp when a window is iconised.  'task' is the task handle of the owner
  of the window, and 'title' is 20 bytes of the title string.
*/


typedef struct
{
  window_handle window;
  int           dummy;
  char          spritename[8];
  char          title[20];
} message_windowinfo;
/*
  This is the data for a windowinfo message.  This is used to give information
  to the "iconiser" (usually the pinboard) about what sprite and title to
  give to the iconised window.
*/


typedef struct
{
  int           selectionclaimed : 2;
  int           clipboardclaimed : 1;
} message_claimentity;
/*
  Broadcast to all tasks when the caret/seletion or clipboard are claimed
*/


typedef struct
{
  window_handle window;
  unsigned int  internal_handle;
  int           x, y;
  int           dummy    : 2;
  int           senddata : 1;
  int           filetypes[1];
} message_datarequest;
/*
  Broadcast by an application when it wishes to paste data that it doesn't
  own.
*/


typedef struct
{
  message_header  header;
  union
  {
    char                 bytes [236];
    int                  words [59];
    message_datasave     datasave;
    message_datasaveack  datasaveack;
    message_dataload     dataload;
    message_dataopen     dataopen;
    message_ramfetch     ramfetch;
    message_ramtransmit  ramtransmit;
    message_helprequest  helprequest;
    message_helpreply    helpreply;
    message_print        print;
    message_menuwarn     menuwarn;
    message_iconize      iconize;
    message_windowinfo   windowinfo;
    message_claimentity  claimentity;
    message_datarequest  datarequest;
  } data;
} message_block;
/*
  This is the structure of a wimp message block.  It consists of a header
  which is the same for all messages, and the message data which varies
  between messages types.

  The bytes and words fields allow you to access the data section for any
  message, while many of the more common messages have their own defined
  structures to make them easier to use.

  Messages are sent with Wimp_SendMessage and received as Wimp Poll events.
*/


/* -------------------------------------------------------------------------
   Wimp events:
   ------------------------------------------------------------------------- */

#define wimp_NUMBEROFEVENTS 20
/*
  There aren't actually twenty events, but the way the are numbered, you
  need an array of twenty to hold them all.
*/


typedef enum
{
  event_ANY                 = -99,  /*  see event.c: this is used to denote
                                     *  any event type, any window handle,
                                     *  or any icon handle ... thus -99.
                                     */
  event_NULL                = 0,
  event_REDRAW,
  event_OPEN,
  event_CLOSE,
  event_PTRLEAVE,
  event_PTRENTER,
  event_CLICK = 6,
  event_BUTTON = 6,
  event_USERDRAG,
  event_KEY,
  event_MENU,
  event_SCROLL,
  event_LOSECARET,
  event_GAINCARET,
  event_NONZEROPOLLWORD,
  event_USERMESSAGE         = 17,
  event_SEND                = 17,
  event_USERMESSAGERECORDED = 18,
  event_SENDWANTACK         = 18,
  event_USERMESSAGEACK      = 19,
  event_ACK                 = 19
} event_type;
/*
  This gives meaningful names for the types of events you can receive on
  a Wimp_Poll.  event_ANY is used by the Event library to denote any event
  code, any window handle or any icon handle, depending.
*/


typedef union
{
  unsigned int value;
  struct
  {
    unsigned int null                : 1;
    unsigned int redraw              : 1;
    unsigned int open                : 1;
    unsigned int close               : 1;
    unsigned int ptrleave            : 1;
    unsigned int ptrenter            : 1;
    unsigned int click               : 1;
    unsigned int userdrag            : 1;
    unsigned int key                 : 1;
    unsigned int menu                : 1;
    unsigned int scroll              : 1;
    unsigned int losecaret           : 1;
    unsigned int gaincaret           : 1;
    unsigned int nonzeropollword     : 1;
    unsigned int dummy14             : 1;
    unsigned int dummy15             : 1;
    unsigned int dummy16             : 1;
    unsigned int usermessage         : 1;
    unsigned int usermessagerecorded : 1;
    unsigned int usermessageack      : 1;
    unsigned int dummy20             : 1;
    unsigned int dummy21             : 1;
    unsigned int r3ispollwordptr     : 1;
    unsigned int highprioritypoll    : 1;
    unsigned int keepfpregisters     : 1;
  } data;
} event_pollmask;
/*
  This holds a pollmask for Wimp_Poll, and allows bitfield access to
  mask out events as required.
*/


typedef struct
{
  window_openblock openblock;
  wimp_point       direction;
} scroll_rq;
/*
  This holds the data from a Scroll Request Wimp Poll event.
  You are passed a window_openblock and the "direction" of the scroll.
  The scroll direction are -2 (Page Left/Down), -1 (Left/Down), 0,
  1 (Right/Up) and 2 (Page Right/Up).
*/


typedef union
{
  caret_block      caret;
  key_block        key;
  pollword_block   pollword;
  window_openblock openblock;
  message_block    message;
  mouse_block      mouse;
  wimp_box         screenrect;
  int              selection[10];
  scroll_rq        scroll;
  char             bytes [256];
  int              words [64];
} event_data;
/*
  This holds the data for a Wimp_Poll event.  It provides 'bytes' and 'words'
  for generic access to the data, and structures for accessing the data for a
  specific event with a more informative and useful interface.
*/


typedef struct
{
  event_type type;
  event_data data;
} event_pollblock;
/*
  This is the type of block returned by a Wimp_Poll.
  The information in the 'data' block is determined by the event type.
*/


/* -------------------------------------------------------------------------
   Miscellanea
   ------------------------------------------------------------------------- */


/* See Font.h for a description of this */
typedef struct
{
  char fonts[256];
} font_array;
/*
  This type contains a count for each font handle from 0-255 of how
  many times it has been found with Font_FindFont.  It is used in
  template_block for creating windows.
*/


typedef struct
{
  window_block   *buffer;
  char           *workfree;
  char           *workend;
  font_array     *font;
  char           *name;
  unsigned int   index;
} template_block;
/*
  This holds data for use by Wimp_LoadTemplate.  The buffer needs to be large
  enough to hold the window block you are trying to load, 'workfree' should
  point to workspace for indirected data with 'workend' pointing to the end of
  that workspace.
*/


typedef struct
{
  wimp_point mul;
  wimp_point div;
} scale_block;
/*
  This holds x and y multipliers and divisors, and is used by
  Wimp_ReadPixTrans to hold scaling information.
*/


#ifdef __cplusplus
}
#endif


#endif
