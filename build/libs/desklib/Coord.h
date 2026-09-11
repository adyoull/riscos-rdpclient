/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Coord.h
    Author:  Copyright © 1992, 1993, 1994 Edouard Poor, Jason Williams
                                          and Tim Browse
    Version: 1.03 (17 Apr 2005)
    Purpose: Coord (point and rectangle) handling functions
*/

#ifndef __dl_coord_h
#define __dl_coord_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header defines functions for handling points and rectangles,
  such as whether a point is inside a given rectangle, or to convert
  from work area coordinates to screen coordinates.

  "Screen Coordinates" refers to OS coordinates, with the bottom
  left corner of the screen being placed at the screen origin (0,0).
  "Work Area Coordinates" refers to Coordinates within the Window's
  work area, where the (0,0) origin is at the TOP left of the work
  area.
*/


typedef struct
{
  wimp_rect  screenrect;
  wimp_point scroll;
} convert_block;
/*
  Holds information about the position of a window and its scroll
  bars.  It is used to provide the information needed to convert
  between work area and screen coordinates.
*/


extern BOOL Coord_PointInRect(wimp_point *point, wimp_rect *rect);
/*
  This function tests whether the point in 'point' is within the
  rectangle given by 'rect'.  If the point lies on the line it's
  counted in (just like in tennis).
*/


extern BOOL Coord_RectContained(wimp_rect *InsideRect,
                                  wimp_rect *OutsideRect);
/*
  This function tests whether the InsideRect is entirely within the
  OutsideRect.  Shared edges and vertices are considered to be
  inside.
*/


extern BOOL Coord_RectsOverlap(wimp_rect *rect1, wimp_rect *rect2);
/*
  Checks to see if two rectangles overlap each other in any way,
  including containment.
 */


#define Coord_RectsIntersect(r1, r2) (Coord_RectsOverlap(r1, r2) && \
                                     !Coord_RectContained(r1, r2) && \
                                     !Coord_RectContained(r2, r1))
/*
  MACRO: BOOL Coord_RectsIntersect(wimp_rect *rect1, wimp_rect *rect2)

  A macro to check if two wimp_rect rectangles intersect.  It gives
  TRUE if the rectangles intersect but do not contain each other and
  FALSE otherwise.  This is unlike Coord_RectsOverlap in that the
  boundaries of the rectanlges must overlap for it to return TRUE.
*/


extern void Coord_WindowOrigin(wimp_point *origin, convert_block *convert);
/*
  The function puts the screen coordinates of the origin (that is
  the top left) of the window's work area into 'origin'.

  This can then be used as a redraw-origin for redraws - any drawing
  done relative to this origin will appear at the correct screen
  position regardless of scroll-bar offsets and screen position of
  the window.

  Remember to call this at the start of each redraw - whenever the
  window is moved or scrolled, the position of this origin will change
  so it must be recalculated.
*/


#define Coord_XToScreen(X, C) \
            (((X) - (C)->scroll.x) + (C)->screenrect.min.x)
/*
  MACRO: int Coord_XToScreen(int xpos, convert_block *convert)

  This takes an x work area coordinate and a pointer to a convert
  block and converts the work area coordinate into a screen coordinate.
*/


#define Coord_YToScreen(Y, C) \
        ( ((Y) - (C)->scroll.y) + (C)->screenrect.max.y )
/*
  MACRO: int Coord_YToScreen(int ypos, convert_block *convert)

  This takes a y work area coordinate and a pointer to a convert
  block and converts the work area coordinate into a screen coordinate.
*/


extern void Coord_PointToScreen(wimp_point *point, convert_block *convert);
/*
  This function converts the point 'point' in work area coordinates
  into screen coordinates.
*/


extern void Coord_RectToScreen(wimp_rect *rect, convert_block *convert);
/*
  This function converts the rectangle 'rect' in work area coordinates
  into screen coordinates.
*/


#define Coord_XToWorkArea(X, C) (((X)-(C)->screenrect.min.x)+(C)->scroll.x)
/*
  MACRO: int Coord_XToWorkArea(int xpos, convert_block *convert)

  This function takes an x screen coordinate and a pointer to a
  convert block and converts the screen coordinate into a work area
  coordinate.
*/


#define Coord_YToWorkArea(Y, C) (((Y)-(C)->screenrect.max.y)+(C)->scroll.y)
/*
  MACRO: int Coord_YToWorkArea(int ypos, convert_block *convert)

  This function takes an y screen coordinate and a pointer to a
  convert block and converts the screen coordinate into a work area
  coordinate.
*/


extern void Coord_PointToWorkArea(wimp_point *point,
                                    convert_block *convert);
/*
  This function converts the point 'point' in screen coordinates into
  one in work area coordinates.
*/


extern void Coord_RectToWorkArea(wimp_rect *rect, convert_block *convert);
/*
  This function converts the rectangle 'rect' in screen coordinates
  into one in work area coordinates.
*/


extern void Coord_RectUnion(wimp_rect *dest,
                              wimp_rect *src1, wimp_rect *src2);
/*
  This function finds the union of two rectangles, src1 and src2,
  and returns it in dest.  Any of src1, src2 or dest can be the same
  pointer.
*/


#ifdef __cplusplus
}
#endif


#endif
