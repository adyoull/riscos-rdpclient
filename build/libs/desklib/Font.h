/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Font.h
    Author:  Copyright © 1993 Shaun Blackmore, Julian Smith
    Version: 1.03 (17 Apr 2005)
    Purpose: Assembler SWI veneers for the Font Manager
             NOTE that some names are not strictly 'DeskLib' style, to keep
             them consistent with the SWIs they represent ("Converttopoints",
             etc). However, macros with 'DeskLib' naming are provided to allow
             either naming style to be used.
    Mods:    08 Jul 1995 - Julian Smith - Added Font_Paint3 for use with RISC
                                          OS 3. Also various macros for the
                                          'plottype' argument of Font_Paint3.
*/

#ifndef __dl_font_h
#define __dl_font_h

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
  This header provides assembler veneers for the FontManager SWIs.

  Some of the names are not strictly in the DeskLib style, instead they
  are consistent with the SWIs they correspond to, eg. Font_Converttopoints.
  For these cases, macros with the DeskLib style are provided to allow
  either style to be used.
*/


typedef unsigned char font_handle;
/* A RISC OS font manager font handle */


typedef struct
{
  char name[128];
  unsigned int xsize, ysize;
  unsigned int xres, yres;
  unsigned int age;
  unsigned int usage;
} font_defn;
/*
  This is a structure to hold information about a font.  Most of the fields
  are fairly self-explanatory, but note that the name may be CR terminated
  rather than nul terminated.  The sizes are in 16ths of a point, and the
  resolutions are in dots per inch.

  'age' is the number of font accesses since this font was last accessed,
  and 'usage' is the number of times the Font_Find has found the font,
  minus the number of time Font_Lost has been used on it.
*/


typedef struct
{
    int minx, miny, maxx, maxy;
} font_info;
/*
  This is used to give a bounding box for a font or a character in a
  font.  The values represent the minimal area covering a character
  in the font, and are in either OS units or millipoints, depending on
  how they were obtained.
*/


typedef struct
{
  char *s;              /* string */
  int x;                /* in/out, in 1/72000s of an inch */
  int y;                /* in/out, in 1/72000s of an inch */
  int split;            /* in/out: space char, or -1      */
                        /*         on exit, = count of space or printable */
  int term;             /* in/out: index into s */
} font_string;
/*
  This is a type used by Font_FindCaret and Font_StringWidth, both of which
  are deprecated in favour of the Font_ScanString SWI.

  x and y are in always millipoints (1/72000th of an inch), the contents of
  the other fields are largely determined by which function it is used with.
*/


typedef struct
{
  int background, foreground;
} font_colours;
/*
  The logical colours of the font.
*/


typedef struct
{
  font_handle  font;
  font_colours colour;
  int          offset;
} font_state;
/*
  Holds information about a "font state" - ie. what font, colour and
  anti-aliasing colour offset will be used for a Font_Paint call.
*/

typedef struct
{
  char offset;
  char thresholds[15];
} font_thresholds;
/*
  This holds the list of threshold values used for anti-aliasing by the
  Font Manager (as read with Font_ReadThresholds).
*/

   /*  Font usage array  (font_array)
    *  This array is indexed by the font handle - each entry contains a
    *  count of how many times we have FindFont'd this font. This is used
    *  mainly by the template system (Wimp_LoadTemplate).
    *  See Font_LoseAllFonts
    *  For convenience, this is defined in Wimp.h (it is mainly used for
    *  template loading)
    *
    *  typedef struct
    *  {
    *    char fonts[256];
    *  } font_array;
    */

extern os_error *Font_CacheAddr(int *version, int *cacheused, int *cachesize);
/*
  This function finds information about the Font Manager - the version number
  (*100), the font cache size and the amount of that cache used.
*/


extern os_error *Font_ListFonts(const char *name, int *count);
/*
  This function allows you to list the fonts currently available, in
  alphabetical order.

  You call it with a pointer to a buffer of at least 40 characters in 'name',
  and with count pointing to an int containing zero.  After each call, a font
  name will be placed in 'name', and 'count' will contain the correct value
  to pass for the next call of the function, or -1 if you have read all font
  names.
*/


extern os_error *Font_ReadDefn(font_handle font, font_defn *defn);
/*
  This function reads information about the font whose handle is given in
  'font', putting it in the block pointed to by 'defn'.  See the definition
  of font_defn for details of what is returned.
*/


extern os_error *Font_ReadInfo(font_handle font, font_info *info);
/*
  This function gets the bounding box in OS units of the font whose handle
  is given in 'font'.  The bounding box is the minimal area convering every
  character in the font.  Note that the bounding box is the minimal size
  for screen printing and not for printing use for which millipoint
  measurements should be used.

  You should use Font_CharBBox in preference, according to the PRMs,
  possibly because it gives the choice of millipoint or OS unit bounding boxes.
*/


extern os_error *Font_ConverttoOS(int x, int y, int *xout, int *yout);
/*
  This converts x and y from millipoints to OS units, according to the
  current scale factor (normally 400millipoints = 1 OS unit).

  'xout' and 'yout' must both be valid addresses even if you are only
  interested in one of them.
*/

#define Font_ConvertToOS Font_ConverttoOS
/*
  This is simply to provide a more DeskLib-like name for the
  Font_ConverttoOS function.
*/


extern os_error *Font_Converttopoints(int x, int y, int *xout, int *yout);
/*
  This converts x and y from OS units to millipoints, according to the
  current scale factor (normally 400millipoints = 1 OS unit).

  'xout' and 'yout' must both be valid addresses even if you are only
  interested in one of them.
*/

#define Font_ConvertToPoints Font_Converttopoints
/*
  This is simply to provide a more DeskLib-like name for the
  Font_Converttopoints function.
*/


extern os_error *Font_FindFont(font_handle *font, const char *name,
                               int xsize, int ysize, int xres, int yres);
/*
  This finds the font handle of the font whose idenitifier, size and resolution
  are given.  It also sets this font as the current font for use by Font_Paint.
  When you have finished with the font, you should call Font_LoseFont.

  The identifier passed in 'name' is just the font name in its simplest form,
  but can also hold information about the encoding, etc. to use.  If you want
  to use these, you'll have to look them up in the PRMs.

  The size is specified in 16ths of a point and the resolutions in dots per
  inch.  The resolutions can be zero to use the default resolution for the
  font, or -1 to use the current resolution.
*/


extern os_error *Font_LoseFont(font_handle font);
/*
  This tells the font manager you are finished with the font you "found" with
  Font_FindFont.
*/


extern void Font_LoseAllFonts(font_array *fonts);
/*
  This calls Font_LoseFont for all the fonts in the font usage array 'fonts'.
  These arrays are mainly used whn generating windows using anti-aliased fonts
  in icons, and so are defined in DeskLib:Wimp.h and not here.
*/


extern os_error *Font_SetFont(font_handle font);
/*
  This sets the given font to be used by future calls to the Font Manager.
*/


extern os_error *Font_CurrentFont(font_state *state);
/*
  This reads the currently set font handle and colours into the block pointed
  to by 'state'.
*/


extern os_error *Font_FutureFont(font_state *state);
/*
  This function can be used after a call to Font_StringWidth to read what the
  state after the next call to Font_Paint would be.
*/


extern os_error *Font_Paint(const char *str, int options, int x, int y);
/*
  This writes a string to the screen in the simple RISC OS 2 way.  It doesn't
  support tranformations of the text, for instance - you should use
  Font_Paint3 to access such features.

  The string can contain control characters to change font, colours, etc.
  as it is painted.  You will find details of these in the PRMs or the
  appropriate Stronghelp manual.

  The flags you can use in 'options' are font_justify_CURSOR, font_rubout_CURSOR
  and font_plot_OSCOORS.  All other bits of 'options' should be zero.
*/


extern os_error *Font_Caret(int colour, int height, int flags, int x, int y);
/*
  This sets the colour, height (in OS coordinates) and position of the caret.
  If 'flags' is font_OSCOORS then x and y are in OS coordinates, otherwise
  they are in millipoints.
*/


extern os_error *Font_FindCaret(font_string *string);
/*
  On entry the string block contains a string and an x and y offset in
  millipoints from the start of that string.  The function finds the
  character nearest to that point and returns information about it in
  the other fields of the block.

  On exit the 'term' field contains the index into the string in characters,
  and the 'split' field contains the number of printable characters up to
  that point.  The 's' field (which pointed to the start of the string on
  entry) is updated to point to where the search ended.

  This is deprecated in favour of the Font_ScanString SWI.
*/


extern os_error *Font_FindCaretJ(font_string *s, int xoffset, int yoffset);
/*
  This does much the same as Font_FindCaret, but for a justified string.

  The x and y offsets are calculated by dividing the extra gap to be filled
  by the number of space characters in the string.  If these are both zero,
  it is exactly equivalent to Font_FindCaret.

  This is deprecated in favour of the Font_ScanString SWI.
*/


extern os_error *Font_StringWidth(font_string *string);
/*
  This calculates how wide a string would be under the given limits.

  On entry the string block contains a pointer to the string, the maximum
  height and width (in the x and y fields), and optionally the character code
  of the character at which to split the string (eg. 32 to split at spaces).
  If you don't want to use a split character, this should be -1.  The 'term'
  field should contain the index of the character to terminate by.

  On exit, the x and y fields are updated to the offsets after printing the
  string, the number of split characters are returned in the 'split' field if
  you specified a split character - otherwise the number of printable
  characters is returned.  The 's' and 'term' fields are updated to point at
  the terminating position.

  This is deprecated in favour of the Font_ScanString SWI.
*/


extern os_error *Font_CharBBox(font_handle font, char c,
                               int flags, font_info *info);
/*
  This function gets the bounding box of the given character in the given font,
  and places it in 'info'.  'flags' can be either zero or font_OSCOORS,
  in which case the bounding box will be in millipoints or OS coordinates
  respectively.
*/


extern os_error *Font_StringBBox(const char *string, font_info *info);
/*
  This function gets the bounding box of the given string in the current font,
  in millipoints, and places it in 'info'.  The string may contain Font_Paint
  control sequences.

  This is deprecated in favour of the Font_ScanString SWI.
*/


extern os_error *Font_ReadScaleFactor(int *xscale, int *yscale);
/*
  This reads the current scale factors in millipoints per OS unit.
  The default value is 400.
*/


extern os_error *Font_SetFontColours(font_handle font,
                                     int back_colour, int fore_colour,
                                     int offset);
/*
  This function sets the current font colours (and font if 'font' isn't zero).
  The colours for 'back' and 'fore' are logical colours.  'offset' sets the
  offset from 'fore' to the real foreground colour.  Values between 'fore' and
  'fore + offset' are used for anti-aliasing.

  For example, to have colour 0 as backgound and 1-7 for anti-aliasing you
  would use Font_SetFontColours(0, 0, 1, 6);

  This is best used in conjunction with ColourTrans_ReturnFontColours, so
  that you can get the best colours to use in the current mode.
*/


extern os_error *Font_SetPalette(int back_log,  int fore_log,
                                 int offset,
                                 int back_phys, int fore_phys);
/*
  This sets the anti-alias palette, and should not be used within the desktop.

  'back_log', 'fore_log' and offset' are the background and foreground logical
  colours and the offset between 'pfore' and the real foreground colour (as
  with Font_SetColours).  'back_phys' and 'fore_phys' are colour words in the
  form 0xBBGGRR00 which set the physical background and foreground colours.
  The specified palette entries are then altered to contain a suitable palette
  for anti-aliasing.
*/


extern os_error *Font_ReadThresholds(font_thresholds *t);
/*
  This reads the list of threshold values for anti-aliasing and stores them
  in 't'.  The PRMs have full details of what the threshold values mean.
*/


extern os_error *Font_SetThresholds(font_thresholds *t);
/*
  This sets the list of threshold values for anti-aliasing and stores them
  in 't'.  The PRMs have full details of what the threshold values mean.
*/


extern os_error *Font_GetWimpFont(font_handle *handle);
/*
  Get handle of desktop font - handle is 0 if the "system" font is in use.
*/


#define font_justify_NONE (0)
/*
  No justification.
*/

#define font_justify_CURSOR (1<<0)
/*
  Justify between the start position and the last graphics cursor position.

  Not to be used with font_justify_COORBLOCK or font_justify_NONE.
*/

#define font_justify_COORBLOCK (1<<5)
/*
  Justify according to font_coorblock given.

  Not to be used with font_justify_CURSOR, font_justify_NONE or
  font_plot_OSCOORS.
*/

#define font_rubout_NONE (0)
/*
  Don't plot a rubout box.
*/

#define font_rubout_CURSOR (1<<1)
/*
  Use the last two graphics cursor positions to define the
  rubout box.  If you use this *and* font_justify_CURSOR, the
  first two movements specify the rubout box's bottom left and
  top right corner, and the last movement specifies the
  justification.
*/

#define font_rubout_COORBLOCK (1<<1 + 1<<5)
/*
  The rubout box is defined by the given font_coorblock.

  Not to be used in conjunction with font_plot_OSCOORS.
*/

#define font_OSCOORS (1<<4)
/*
  Used with many calls to choose to have sizes or positions
  returned in OS coordinates rather than millipoints.
*/

#define font_plot_OSCOORS (1<<4)
/*
  Coordinatess are in OS units, not millipoints.

  Not to be used with font_justify_COORBLOCK, font_rubout_COORBLOCK
  or font_plot_TARNSMATRIX.
*/

#define font_plot_TRANSMATRIX (1<<6)
/*
  Use the given transformation matrix when plotting the text.

  Not to be used in conjunction with font_plot_OSCOORS.
*/

#define font_STRLEN (1<<7)
/*
  This denotes that we are passing the length of the string being
  plotted.  This means that instead of printing all the way up to
  the terminator, only up to the given number of characters will
  be printed.
*/

#define font_plot_CURRENTHANDLE (1<<8)
/* See font_plot_GIVENHANDLE */

#define font_plot_GIVENHANDLE   (1<<8)
/*
  This denotes that we are either passing a font handle to use instead
  of the currently selected font, or passing a font handle of zero to
  use the current font.
*/

#define font_plot_KERN (1<<9)
/*
  This tells the Font Manager to kern the string, making certain
  pairs of letters fit more closely together than would otherwise
  be the case.
*/

#define font_plot_RIGHTTOLEFT (1<<10)
/*
  This denotes that the writing direction is from right to left.
  The width of each character is subtracted before painting it, rather
  than it being added after painting.
*/

#define font_plot_BLENDING (1<<11)
/*
  Background font blending - requires font manager present in RISC OS 3.7
  or later.
*/

#define font_plot_GIVEN16_BIT (1<<12)
#define font_plot_GIVEN32_BIT (1<<13)
/*
  RISC OS 5+ Unicode font plotting
*/

typedef struct
{
  struct
  {
    wimp_point space;
    wimp_point letter;
  } offsets;

  wimp_rect rubout;
} font_coorblock;
/*
  This is a block that can be used with Font_Paint3 if you set the
  font_rubout_COORBLOCK or font_justify_COORBLOCK flags.

  It holds the additional x and y offsets for spaces in 'space', the
  additional x and y offsets for each letter in 'letter', and the
  coordinates for a rubout box in 'rubout'.

  Note that all coordinates are in millipoints, as you are not allowed
  to use font_plot_OSCOORS in conjunction with either of these flags.
*/


typedef struct
{
  struct
  {
    int xx; /* These names may not be right... */
    int xy;
    int yx;
    int yy;
  } scale;
  wimp_point translate;
} font_transformation;
/*
  A standard Draw transformation matrix, used with Font_Paint3 and
  Font_ScanString to plot or analyse transformed text.

  The new coordinates are calculated:

  newx = (xx * x) + (yx * x) + translate.x;

  newy = (xy * y) + (yy * y) + translate.y;
*/


os_error *Font_Paint3(font_handle font,
                      const char *string,
                      unsigned int plottype,
                      int x, int y,
                      font_coorblock *coorblock,
                      font_transformation *transformation,
                      int length);
/*
  This function paints text to screen, with the option of using all the
  transformations and justification options introduced in RISC OS 3.

  'string' contains the string to be painted.  This may include a number of
  control code sequences, such as 19,r,g,b,R,G,B,max which sets the font
  colours according to the results of a call to ColourTrans_SetFontColours.
  Other control sequences are 9,dx_low,dx_middle,dx_high which allows x
  movement within a string, and the y-axis equivalent using control code
  11, which can be useful for subscripts, tabs, and the like.  You can change
  the font by using 26,font_handle, and enable underlining by using
  25,line_position,line_thickness.  The underline position is a signed number
  in 256ths of the font size, relative to the baseline of the current font.
  The thickness is in the same units, but unsigned.

  'plottype' contains the flags, which determine the meaning of the other
  parameters.  This should be a bitwise-ORed combination of
  font_justify_NONE, font_justify_CURSOR, font_justify_COORBLOCK,
  font_rubout_NONE, font_rubout_CURSOR, font_rubout_COORBLOCK,
  font_plot_OSCOORS, font_plot_TRANSMATRIX, font_STRLEN,
  font_plot_GIVENHANDLE, font_plot_KERN, font_plot_RIGHTTOLEFT.

  Not all of the flags are compatible with one another - you can only use
  one of the font_justify_* flags and one of the font_rubout_* flags, and
  you cannot use OS coordinates (font_plot_OSCOORS) if you are specifying
  a coordinate block for justification or a rubout box, or are giving a
  transformation matrix to be used, as these are always specified in
  millipoints.

  You can use font_STRLEN and given a string length in 'length' if you want
  to print a substring of the given string - it will then stop painting after
  that character, though the next character in the string may be accessed to
  gather formatting information.

  If you set font_plot_GIVENHANDLE then 'font' is used as the initial font
  handle for plotting.  This can be zero to use the currently set font.

  Any parameters which the flags say you are not using (eg. 'transformation'
  if you haven't set font_plot_TANSMATRIX) can safely be NULL.
*/



#define font_scan_CARET (1<<17)
/*
  This denotes that we wish to find the caret position with this call to
  Font_ScanString, rather than a split point.
*/

#define font_scan_USEBLOCK (1<<5)
/*
  This indicates that a block is being passed to Font_ScanString in 'block',
  containing extra information to be used in the scan.  This includes the
  specification of a split character.
*/

#define font_scan_BBOX ((1<<18) + (1<<5))
/*
  This indicates that you wish the bounding box of the string to be returned
  in the 'bbox' field of the block passed to Font_ScanString in 'block'.

  This requires you to set up the rest of that block appropriately (i.e. you
  should specify a split character or -1, and set the additional offsets to
  the values you require).
*/

#define font_scan_RETURNTRANSMATRIX ((1<<19) + (1<<6))
/*
  This indicates that the transformation matrix passed to Font_ScanString
  should be updated on exit to contain the matrix applying at the end of
  the string.

  This requires that a matrix was passed in in the first place!
*/

#define font_scan_RETURNCOUNT ((1<<20) + (1<<7))
/*
  This indicates that the number of split characters should be returned in
  'length' on exit from Font_ScanString.  If no split character is specified,
  the number of printable characters will be returned instead.

  Note that this requires that a string length be passed in to Font_ScanString
  in 'length'.
*/


typedef struct
{
  struct
  {
    wimp_point space;        /* An additional offset for spaces  */
    wimp_point letter;       /* An additional offset for letters */
  } offsets;

  int split_ch;              /* Split character (-1 for none)  */
  wimp_rect bbox;            /* Optional returned bounding box */
} font_scanstringblock;
/*
  This is a type of block which can be used to specify extra information for
  Font_ScanString.  You can pass additional offsets for spaces and/or letters,
  and a split character (-1 indicates no split character should be used).

  If you set the font_scan_BBOX flag then the bounding box of the string will
  be returned in 'bbox'.  This bounding box covers only the characters of the
  string itself, not any underlining or rubout box.

  If you do not set font_scan_BBOX you must set font_scan_USEBLOCK in order
  to use this block with Font_ScanString,
*/


os_error *Font_ScanString(font_handle font,
                          const char *string,
                          char **end,
                          unsigned int flags,
                          int *x, int *y,
                          font_scanstringblock *block,
                          font_transformation *transformation,
                          int *length);
/*
  This function scans a string as if it were being painted to screen with
  Font_Paint3, and returns various data about it.  It can be used to find
  the bounding box of a string, the position of the caret in the string
  or to find where to split a line, if at all.

  The string to be scanned is in the format used by Font_Paint3 - it can
  contain various escape sequences some of which are described in the
  information with Font_Paint3 and the full details of which can be found in
  the PRMs.

  Note that unlike the Font_Paint calls Font_ScanString deals exclusively with
  coordinates in millipoints.  If you need OS unit values you should convert to
  and from millipoints, for example by using Font_ConvertToPoints.

  'flags' contains the flags which determine exactly what information is passed
  to and returned from Font_ScanString.  This should be a bitwise-ORed
  combination of the ScanString-specific font_scan_CARET, font_scan_USEBLOCK,
  font_scan_BBOX, font_scan_RETURNCOUNT and font_scan_RETURNTRANSMATRIX, and
  the flags shared with Font_Paint3 which are font_plot_TRANSMATRIX,
  font_plot_GIVENHANDLE, font_STRLEN, font_plot_KERN and font_plot_RIGHTTOLEFT.

  'x' and 'y' are offsets from the start of the string.  When determining the
  caret position (when the font_scan_CARET flag has been set), these specify
  the offset from the start of the string to the mouse click.  Otherwise they
  are the width and height to be used when determining the split point.  On
  exit from the function, these are filled with the actual offset to the caret
  position or the split point, respectively.

  On exit, 'end' will be updated to point to the position in the string
  where the scan ended.  If the font_scan_CARET flag has been set this will be
  the caret position, otherwise it will be the split point or the end of the
  string if no splitting was required.  If you do not need to know where the
  scan ended, you can pass NULL to ignore the returned value.

  If the font_scan_USEBLOCK or font_scan_BBOX flags are set, then 'block'
  should point to a block containing any additional offsets to be used when
  scanning, and a split character (or -1 to indicate no split character).  In
  the font_scan_BBOX case, it will be updated to contain the bounding box of
  the string on exit from the function.

  If the font_plot_TRANSMATRIX or font_scan_RETURNTRANSMATRIX flags are set,
  then 'transformation' points to a transformation matrix to use when scanning
  the string.  This will be updated to hold the matrix applying at the end of
  the string on exit from the function if you use the RETURNTRANSMATRIX form.

  If the font_STRLEN or font_scan_RETURNCOUNT flags are set then on entry
  'length' should contain the length of the substring to use when scanning
  the string.  In the RETURNCOUNT case, 'length' will be updated on exit to
  contain the number of split characters encountered, or if no split character
  is specified then the number of printable characters encountered.

  'font' will only be used if you set the font_plot_GIVENHANDLE flag,
  otherwise the current font will be used.  You can set the flag and still
  use the current font by passing zero for 'font'.

  If the font_plot_RIGHTTOLEFT flag has been set, you would normally pass
  a negative value of 'x', as this means that the scan continues until the
  x offset is less than this value - generally more appropriate for
  right-to-left plotting.
*/


#ifdef __cplusplus
}
#endif


#endif
