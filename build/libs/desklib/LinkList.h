/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    LinkList.h
    Author:  Copyright © 1992 John Winters
    Version: 1.34 (17 Apr 2005)
    Purpose: Linked list management routines
*/


#ifndef __dl_linklist_h
#define __dl_linklist_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This library provides support for a simple linked list structure.

  You supply an anchor for the list (a variable of type linklist_header)
  which is used to record the start and end of the list.  The list itself
  is made up of structures you define yourself which have a linklist_header
  as their first member.

  To insert items into the list, you must allocate memory for one of your
  own list elements, and then call the appropriate LinkList function to
  add it to the list.  To delete it, you call the LinkList_Unlink function
  to remove it from the list, and then free it as you normally would.
*/
/*
  An example user-defined linklist element:
    struct listelement
    {
      linklist_header header;
      ... Your own data here ...
    } listelement;
*/


typedef struct linklist_header
{
  struct linklist_header *next;
  struct linklist_header *previous;
} linklist_header;



extern void LinkList_AddToHead(linklist_header *anchor, linklist_header *item);
/*
  This inserts the given item at the head of the list whose anchor is given.
*/


extern void LinkList_AddToTail(linklist_header *anchor, linklist_header *item);
/*
  This inserts the given item at the end of the list whose anchor is given.
*/


extern void LinkList_InsertBefore(linklist_header *anchor,
                                  linklist_header *pos,
                                  linklist_header *item);
/*
  This inserts the given item *before* the item 'pos' in the list
  whose anchor is given.
*/


extern void LinkList_InsertAfter(linklist_header *anchor,
                                 linklist_header *pos,
                                 linklist_header *item);
/*
  This inserts the given item *after* the item 'pos' in the list
  whose anchor is given.
*/


extern BOOL LinkList_InList(linklist_header *anchor, linklist_header *item);
/*
  This returns TRUE if the given item is currently in the list whose anchor
  is given.  This only checks the pointers to items, not the contents.  If
  you want to check the contents you must chain through the list yourself.
*/


extern int LinkList_ListLength(linklist_header *anchor);
/*
  This counts and returns the number of items in the given linked list.
  If you need this value a lot, you should probably store the size and
  update it as you go, or modify the LinkList code to store do it for you.
*/


extern void LinkList_Unlink(linklist_header *anchor, linklist_header *item);
/*
  This unlinks (removes) the item from the given list.
  You need to free the memory for the item yourself.
*/


#define LinkList_FirstItem(x) ((void *)(x)->next)
/*
  MACRO: void *LinkList_FirstItem(linklist_header *anchor);

  This returns a pointer to the first item in the given list.
*/


#define LinkList_LastItem(x) ((void *)(x)->previous)
/*
  MACRO: void *LinkList_LastItem(linklist_header *anchor);

  This returns a pointer to the last item in the given list.
*/


#define LinkList_Init(x)  \
  {                       \
    (x)->next = NULL;     \
    (x)->previous = NULL; \
  }
/*
  MACRO: void LinkList_Init(linklist_header *anchor);

  This initialises a list anchor, and should be called before the anchor
  is used.  It sets both pointers to NULL to indicate the list is empty.
*/


#define LinkList_InitItem(x) \
  {                          \
    (x)->next = NULL;        \
    (x)->previous = NULL;    \
  }
/*
  MACRO: void LinkList_InitItem(linklist_header *anchor);

  This initialises a list item, and should be used if you do not immediately
  add the item to a list.  This removes any ambiguity about the item's
  status - it is clearly not in a linked list.
*/


#define LinkList_NextItem(x) ((void *) ((linklist_header *)(x))->next)
/*
  MACRO: void *LinkList_NextItem(void *item);

  This returns the item in the list after the given item.
  If there is no next item, it returns NULL.
*/


#define LinkList_PreviousItem(x) ((void *) ((linklist_header *)(x))->previous)
/*
  MACRO: void *LinkList_PreviousItem(void *item);

  This returns the item in the list before the given item.
  If there is no previous item, it returns NULL.
*/


#ifdef __cplusplus
}
#endif


#endif
