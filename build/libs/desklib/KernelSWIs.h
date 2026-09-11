/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    KernelSWIs.h
    Author:  Copyright © 1993, 1994 Jason Howat
                         1995       Sergio Monesi
    Version: 1.04 (17 Apr 2005)
    Purpose: General low level SWI veneers.
    Mods:    07 Jun 1995 Sergio Monesi: Added OS_SWINumberTo/FromString
             13 Jul 1995 Sergio Monesi: Added OS_GSTrans
             02 Sep 1995 JH Added osbyte_WRITEVDUDRIVERBANK and
                            osbyte_WRITEDISPLAYHARDWAREBANK.
*/

#ifndef __dl_kernelswis_h
#define __dl_kernelswis_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* Abstract */
/*
  This provides some low level veneers to certain OS_ SWIs.
*/


typedef enum
{
  osbyte_DISPLAYOSVERSION = 0,
  osbyte_SPECIFYINPUT = 2,
  osbyte_SPECIFYOUTPUT,
  osbyte_CURSORKEYSTATUS,
  osbyte_WRITEPRINTERDRIVERTYPE,
  osbyte_WRITEPRINTERIGNORECHAR,
  osbyte_WRITERS423RXRATE,
  osbyte_WRITERS423TXRATE,
  osbyte_WRITEDURATION1STFLASHCOLOUR,
  osbyte_WRITEDURATION2NDFLASHCOLOUR,
  osbyte_WRITEKEYREPEATDELAY,
  osbyte_WRITEKEYREPEATRATE,
  osbyte_DISABLEEVENT,
  osbyte_ENABLEEVENT,
  osbyte_FLUSHBUFFER,
  osbyte_RESETFUNCTIONKEYS = 18,
  osbyte_WAITFORVSYNC,
  osbyte_RESETFONTDEFINITIONS,
  osbyte_FLUSHSELECTEDBUFFER,
  osbyte_RESETGROUPOFFONTDEFINITIONS = 25,
  osbyte_RW_COUNTRYNUMBER = 70,
  osbyte_RW_ALPHABET = 71,
  osbyte_RW_KEYBOARD = 71,
  osbyte_SELECTPOINTER = 106,
  osbyte_ACTIVATEMOUSE = 106,
  osbyte_WRITEVDUDRIVERBANK = 112,
  osbyte_WRITEDISPLAYHARDWAREBANK,
  osbyte_WRITESHADOWSTATE,
  osbyte_READVDUSTATUS = 117,
  osbyte_REFLECTKEYBOARDSTATUSINLEDS,
  osbyte_KEYBOARDSCAN = 121,
  osbyte_KEYBOARDSCANNOSHIFT,
  osbyte_CLEARESCAPECONDITION = 124,
  osbyte_SETESCAPECONDITION,
  osbyte_ACKNOWLEDGEESCAPECONDITION,
  osbyte_CHECKFOREOF,
  osbyte_GETBUFFERORMOUSESTATUS,
  osbyte_SCANFORKEY = 129,
  osbyte_READOSIDENTIFIER = 129,
  osbyte_READTEXTCURSORPOSITION = 134,
  osbyte_READCHARACTERATTEXTCURSOR = 135,
  osbyte_READSCREENMODE = 135,
  osbyte_INSERTCHARACTERCODEINTOBUFFER = 138,
  osbyte_WRITEFSOPTIONS,
  osbyte_ISSUEMODULESERVICECALL = 143,
  osbyte_SETTVCHARACTERISTICS,
  osbyte_GETCHARACTERFROMBUFFER,
  osbyte_EXAMINEBUFFERSTATUS = 152,
  osbyte_INSERTCHARACTERINTOBUFFER,
  osbyte_RW_ASYNCCOMMSSTATE = 156,
  osbyte_READVDUVARIABLE = 160,
  osbyte_READCMOSRAM,
  osbyte_WRITECMOSRAM,
  osbyte_RW_GENERALGRAPHICSINFO,
  osbyte_READOUTPUTCURSORPOSITION = 165,
  osbyte_RW_50HZCOUNTER = 176,
  osbyte_READINPUTSOURCE,
  osbyte_RW_RS423INPUTINTERPTRETATION = 181,
  osbyte_RW_NOIGNORESTATE,
  osbyte_RW_RS423BUSYFLAG = 191,
  osbyte_READRS423CONTROLBYTE,
  osbyte_RW_FLASHCOUNTER,
  osbyte_READDURATION2NDFLASHCOLOUR,
  osbyte_READDURATION1STFLASHCOLOUR,
  osbyte_RW_KEYREPEATDELAY,
  osbyte_RW_KEYREPEATRATE,
  osbyte_RW_EXECFILEHANDLE,
  osbyte_RW_SPOOLFILEHANDLE,
  osbyte_RW_BREAKESCAPEEFFECT,
  osbyte_RW_KEYBOARDDISABLEFLAG,
  osbyte_RW_KEYBOARDSTATUSBYTE,
  osbyte_RW_RS423INPUTBUFFERMINSPACE,
  osbyte_RW_RS423IGNOREFLAG,
  osbyte_RW_BELLCHANNEL = 211,
  osbyte_RW_BELLVOLUME,
  osbyte_RW_BELLFREQUENCY,
  osbyte_RW_BELLDURATION,
  osbyte_RW_FUNCTIONKEYSTRINGLENGTH = 216,
  osbyte_RW_PAGEDMODELINECOUNT,
  osbyte_RW_BYTESINVDUQUEUE,
  osbyte_RW_TABKEYCODE,
  osbyte_RW_ESCAPECHARACTER,
  osbyte_RW_INTERPRETATIONOFBUFFERCODES_C0_CF,
  osbyte_RW_INTERPRETATIONOFBUFFERCODES_D0_DF,
  osbyte_RW_INTERPRETATIONOFBUFFERCODES_E0_EF,
  osbyte_RW_INTERPRETATIONOFBUFFERCODES_F0_FF,
  osbyte_RW_FUNCTIONKEYINTERPRETATION,
  osbyte_RW_SHIFTFUNCTIONKEYINTERPRETATION,
  osbyte_RW_CTRLFUNCTIONKEYINTERPRETATION,
  osbyte_RW_CTRLSHIFTFUNCTIONKEYINTERPRETATION,
  osbyte_RW_ESCAPEKEYSTATUS,
  osbyte_RW_ESCAPEEFFECTS,
  osbyte_RW_CHARACTERDESTINATIONSTATUS = 236,
  osbyte_RW_CURSORKEYSTATUS,
  osbyte_RW_NUMERICKEYPADINTERPRETATION,
  osbyte_READCOUNTRYFLAG = 240,
  osbyte_READRS423BAUDRATES = 242,
  osbyte_READTIMERSWITCHSTATE,
  osbyte_READPRINTERDRIVERTYPE = 245,
  osbyte_RW_PRINTERIGNORECHARACTER,
  osbyte_RW_BREAKKEYACTIONS,
  osbyte_READVDUDRIVERSCREENBANK = 250,
  osbyte_READDISPLAYSCREENBANK,
  osbyte_READLASTBREAKTYPE = 253,
  osbyte_RW_EFFECTOFSHIFTCTRLONNUMERICKEYPAD,
  osbyte_RW_BOOTOPTION
} osbyte_number;
/*
  These are reasonably meaningful names for a large number of the
  possible reason codes for the OS_Byte SWI.

  Don't you just love "osbyte_RW_EFFECTOFSHIFTCTRLONNUMERICKEYPAD"?
*/


os_error *OS_Byte(osbyte_number reason, int r1in, int r2in,
                                        int *r1out, int *r2out);
/*
  This is a veneer to the OS_Byte SWI which does a bewilderingly large
  number of different things depending on the reason code given in
  'reason'.  If you want to know exactly what any of these does, you'll
  either need years of experience, a copy of the PRMs or the OS SWIs
  Stronghelp manual.
*/


typedef enum
{
  osword_READLINE = 0,
  osword_READSYSCLOCK,
  osword_WRITESYSCLOCK,
  osword_READINTERVALTIMER,
  osword_WRITEINTERVALTIMER,
  osword_READPIXELCOLOUR = 9,
  osword_READCHARACTERDEFN,
  osword_READPALETTE,
  osword_WRITEPALETTE,
  osword_READGFXCURSORS,
  osword_READCMOSCLOCK,
  osword_WRITECMOSCLOCK,
  osword_DEFINEPOINTERANDMOUSE = 21,
  osword_WRITESCREENBASE
} osword_number;
/*
  These are meaningful names for the various reason codes used with
  the OS_Word SWI.
*/


os_error *OS_Word(osword_number reason, void *param_block);
/*
  This is a veneer to the OS_Word SWI, and does a number of different things
  depending on the reason code given in 'reason'.  You'll have to build
  your own parameter blocks to pass in param_block depending on the
  action you wish to take or the value you wish to read.
*/


extern BOOL OS_ReadVarVal(const char *varname, char *buf, int bufsize);
/*
  This reads the value of the system variable whose name is 'varname',
  placing the text in the buffer 'buf' of size 'bufsize'.
*/


typedef enum
{
  modevar_MODEFLAGS = 0,
  modevar_RIGHTCOLUMN,
  modevar_BOTTOMROW,
  modevar_NCOLOUR,
  modevar_XEIGFACTOR,
  modevar_YEIGFACTOR,
  modevar_LINELENGTH,
  modevar_SCREENSIZE,
  modevar_YSHIFTFACTOR,
  modevar_LOG2BPP,
  modevar_LOG2BPC,
  modevar_XWINDOWLIMIT,
  modevar_YWINDOWLIMIT
} mode_variable;
/*
  This gives useful names to the various mode variables you can read with
  OS_ReadModeVariables.
*/


extern os_error *OS_ReadModeVariable(int mode, mode_variable variable,
                                     int *value);
/*
  This function reads a mode variable for the mode 'mode', or the current
  screen mode if 'mode' is -1.  The value is returned in 'value'.
*/


os_error *OS_SWINumberToString(int number, char *buffer, int size);
/*
  This converts the given SWI number to the corresponding SWI name.
  The name is placed in the string pointed to by 'buffer' of length
  'size'.

  This returns NULL if there is no error.
*/


os_error *OS_SWINumberFromString(const char *string, int *number);
/*
  This converts the given SWI name to the corresponding SWI number.
  The number is placed in 'number'.

  This returns NULL if there is no error.
*/


os_error *OS_GSTrans(const char *string, char *buffer, int size, char **end);
/*
  This is a veneer to the OS_GSTrans SWI, and converts the string 'string'
  using GS operations, placing the resulting string in 'buffer' whose size
  is given by 'size'.  It also sets 'end' to point to the character after
  the terminator.  This expands system variable references in the source
  string.

  It returns NULL if there is no error.
*/


os_error *OS_CLI(const char *cmd);
/*
  This is a veneer for the OS_CLI SWI, executing the given star command.
*/


#ifdef __cplusplus
}
#endif


#endif

