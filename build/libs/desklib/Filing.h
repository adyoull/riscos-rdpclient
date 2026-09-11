/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Filing.h
    Author:  Copyright © 1994, 1995 Sergio Monesi
    Version: 1.05 (17 Apr 2005)
    Purpose: Easy access to files informations, directory scans, filenames
             handling.
    Mods:    06 Jun 1995 - changed some procedure names to a more
                           DeskLib-compliant style
             15 Jul 1995 - added Filing_CanonicalisePath
             27 Aug 1995 - made Filing_ScanDir and Filing_ScanDirByDir
                           reentrant. Added Filing_SingleDirEntry and
                           Filing_SingleDirEntry2
*/

#ifndef __dl_filing_h
#define __dl_filing_h

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract */
/*
  This header defines functions to give easier access to file information,
  different ways to scan directory contents, and a couple of helpful
  filename handling routines.
*/


#ifndef filing_MAXLEAFNAMELEN
  #define filing_MAXLEAFNAMELEN 32
  /*
    Defines the maximum length of a leafname that can be stored in a
    filing_direntry or filing_fulldirentry structure.

    At the time the library was written, leafnames were generally limited
    to 10 characters, and 32 seemed (and in most cases still seems) a
    reasonable limit.

    As this value isn't used in the compiled library, you can #define it
    to whatever you want before including this header to set the value
    yourself.

    As far as I can tell, if it is too small for a filename you may simply
    not be able to read information about a file, as it will not fit
    into the buffer you supply.
  */
#endif


typedef enum
{
  filing_NOTFOUND  = 0,
  filing_FILE      = 1,
  filing_DIRECTORY = 2,
  filing_IMAGEFILE = 3
} filing_objtype;
/*
  Object types, as returned by all manner of RISC OS file-related SWIs.
  Used in filing_direntry and filing_fulldirentry.
*/


typedef struct
{
  int loadaddr;
  int execaddr;
  int length;
  int attrib;
  filing_objtype objtype;
  char name[filing_MAXLEAFNAMELEN];
} filing_direntry;
/*
  Holds information on a file (read from a directory).
  The fields are fairly self-explanatory.
*/


typedef struct
{
  int loadaddr;
  int execaddr;
  int length;
  int attrib;
  filing_objtype objtype;
  int SIN;
  char date[5];
  char name[filing_MAXLEAFNAMELEN];
} filing_fulldirentry;
/*
  Holds all the available information about a file in a directory.

  SIN is the internal name of the file.  date is a RISC OS 5 byte time
  block, suitable for use with Time_ConvertDateAndTime or similar.
*/


typedef enum
{
  readdirtype_DIRENTRY,
  readdirtype_FULLDIRENTRY,
  readdirtype_NAMEONLY
} filing_readdirtype;
/*
  These are the ways objects in a directory can be read using Filing_ReadDir.

  readdirtype_DIRENTRY reads filing_direntry objects, readdirtype_FULLDIRENTRY
  reads the corresponding filing fulldirentry objects and readdirtype_NAMEONLY
  just reads the names of the files.
*/


typedef struct
{
  char  *dirname;
  void  *buf;
  int   size;
  filing_readdirtype type;
  union
  {
    filing_direntry     *direntry;
    filing_fulldirentry *fulldirentry;
    char *name;
    void *act;
  } act;
  int  offset;
  int  read;
  char *match;
} filing_dirdata;
/*
  Contains the data needed by various Filing functions.

  It should not be accessed by the user, except for the 'act' field
  which points to the current object when using Filing_OpenDir and
  Filing_ReadDir.
*/


os_error *Filing_OpenDir(const char *dirname, filing_dirdata *dirdata,
                         int bufsize, filing_readdirtype type);
/*
  Prepares a directory to be read with Filing_ReadDir.  It allocates the
  buffer for reading and sets the starting parameters,

  'dirname' is the pathname of the directory to be scanned, 'dirdata' is a
  pointer to an existing structure that is filled by Filing_OpenDir and
  Filing_ReadDir.

  'bufsize' is the size of the buffer to allocate for reading data.  A
  larger buffer should mean faster operation.  This must be at least as
  big as the object you are trying to read for each file: filing_direntry
  filing_fulldirentry or filing_MAXLEAFNAMELEN, depending.

  'type' chooses what information you are going to read about each object
  in the directory - this should correspond to the size passed in bufsize.

  It returns NULL if succesful, and an error block otherwise.  The errors
  that can be returned in addition to standard OS ones are:
  0x00801C00 - Out of memory,
  0x00801C01 - Directory is actually a file, and
  0x00801C02 - Directory doesn't exist.
*/


void *Filing_ReadDir(filing_dirdata *dirdata);
/*
  Reads the contents of a directory "opened" by Filing_OpenDir, one object
  per call.  Filing_CloseDir must be called when there are no more entries
  to read.

  This takes a pointer to the dirdata set up by Filing_OpenDir. and returns
  the next object in the directory or NULL if there is nothing left to read.

  If there is an error, it returns NULL.
*/


BOOL Filing_CloseDir(filing_dirdata *dirdata);
/*
  This closes a directory opened with Filing_OpenDir.  It should be called
  when you have read the information in the directory to give it a chance
  to free memory it claimed on your behalf.

  It returns TRUE if there are no problems, FALSE if there is an error.
*/


os_error *Filing_ReadDirNames(const char *dirname, char *buf,
                              int *number, int *offset,
                              int size, char *match);
/*
  This is a veneer for OS_GBPB 9. and allows you to read the file names
  of objects in directory 'dirname'.

  'buf' is a pointer to the buffer where the filenames will be returned
  in a zero-separated list of leafnames.

  'number' is the maximum number of elements to read, 'size' is the length
  of buffer and 'match' is the wildcarded filename to match.

  'offset' should be 0 on the first call to this function - it will be
  updated by the function with the correct value to pass next time, or
  set to -1 if there is no more to read.

  'number' is updated with the number of objects read, which may be zero
  before you run out of objects in the directory.

  The function returns NULL if there was no error.
*/


os_error *Filing_ReadDirEntry(const char *dirname, filing_direntry *buf,
                              int *number, int *offset,
                              int size, const char *match);
/*
  This is a veneer for OS_GBPB 10. and allows you to read the directory
  entries of objects in directory 'dirname'.

  'buf' is a pointer to the buffer where the object information will be
  stored.  The records are stored word-aligned in the buffer.  Each object
  is a filing_direntry block with the name section just long enough
  to hold the nul-terminated string and word-align the next object.

  'number' is the maximum number of elements to read, 'size' is the length
  of buffer and 'match' is the wildcarded filename to match.

  'offset' should be 0 on the first call to this function - it will be
  updated by the function with the correct value to pass next time, or
  set to -1 if there is no more to read.

  'number' is updated with the number of objects read, which may be zero
  before you run out of objects in the directory.

  The function returns NULL if there was no error.
*/

os_error *Filing_ReadFullDirEntry(const char *dirname,
                                  filing_fulldirentry *buf,
                                  int *number, int *offset,
                                  int size, char *match);
/*
  This is a veneer for OS_GBPB 11. and allows you to read the full
  directory entries of objects in directory 'dirname'.

  'buf' is a pointer to the buffer where the object information will be
  stored.  The records are stored word-aligned in the buffer.  Each object
  is a filing_fulldirentry block with the name section just long enough
  to hold the nul-terminated string and word-align the next object.

  'number' is the maximum number of elements to read, 'size' is the length
  of buffer and 'match' is the wildcarded filename to match.

  'offset' should be 0 on the first call to this function - it will be
  updated by the function with the correct value to pass next time, or
  set to -1 if there is no more to read.

  'number' is updated with the number of objects read, which may be zero
  before you run out of objects in the directory.

  The function returns NULL if there was no error.
*/

os_error *Filing_ReadCatalogue(const char *filename, filing_objtype *objtype,
                               int *loadaddr, int *execaddr, int *length,
                               int *attrib, int *filetype);
/*
  This is a veneer for OS_File 20.  It reads information about a given file.
  Simply pass it the filename and it returns objtype, loadaddr, execaddr, etc.
  If the file doesn't exist filing_NOTFOUND is returned as the objtype.

  The function returns NULL if there is no error.
*/


os_error *Filing_SingleDirEntry(const char *filename,
                                filing_direntry *buf, int size);
/*
  This reads the directory entry for the file 'filename', using
  Filing_ReadDirEntry.  You pass in a pointer to a filing_direntry block
  and the size of that block, and the block is filled in with the details
  of the file.

  If the file doesn't exist, but the rest of the file path does, the
  objtype will be set to file_NOTFOUND.  If the file path doesn't exist
  an error will be returned.

  The function returns NULL if there was no error.
*/


os_error *Filing_SingleDirEntry2(const char *dirname,
                                 filing_direntry *buf,
                                 int size, char *filename);
/*
  This reads information about a single file 'filename' in the directory
  'dirname'.   You pass in a pointer to a filing_direntry block and the
  size of that block, and the block is filled in with the details of
  the file.

  If the file doesn't exist, but the directory does, the objtype will be
  set to file_NOTFOUND.  If the directory doesn't exist an error will
  be returned.

  The function returns NULL if there was no error.
*/


os_error *Filing_SingleFullDirEntry(const char *filename,
                                    filing_fulldirentry *buf, int size);
/*
  This reads the full directory entry for the file 'filename', using
  Filing_ReadFullDirEntry.  You pass in a pointer to a filing_fulldirentry
  block and the size of that block, and the block is filled in with the
  details of the file. This is the only way (that I know) to read the
  SIN of a single file.

  If the file doesn't exist, but the rest of the file path does, the
  objtype will be set to file_NOTFOUND.  If the file path doesn't exist
  an error will be returned.

  The function returns NULL if there was no error.
*/


os_error *Filing_SingleFullDirEntry2(const char *dirname,
                                      filing_fulldirentry *buf,
                                      int size, const char *filename);
/*
  This reads information about a single file 'filename' in the directory
  'dirname'.  You pass in a pointer to a filing_fulldirentry block and the
  size of that block, and the block is filled in with the details of
  the file.  This is the only way (that I know) to read the SIN of
  a single file.

  If the file doesn't exist, but the directory does, the objtype will be
  set to file_NOTFOUND.  If the directory doesn't exist an error will
  be returned.

  The function returns NULL if there was no error.
*/


char *Filing_GetPathname(const char *filename, char *pathname);
/*
  This gets the pathname from a filename - it strips off the leafname.

  You pass in the filename and a string large enough to hold the path part.
  It returns a pointer to your string.  If the filename is a leafname then
  the pathname will be an empty string.

  You may *not* pass the same string in twice to truncate it - they must
  be different areas of memory.
*/


char *Filing_GetLeafname(const char *filename, char *leafname);
/*
  This gets the leafname from a filename - it strips out the path.

  You pass in the filename and a string large enough to hold the leaf part.
  It returns a pointer to your string.

  You may *not* pass the same string in twice - they must be different
  areas of memory.
*/


char *Filing_FindLeafname(const char *filename);
/*
  This returns a pointer to the leafname part of filename, ie. the part
  immediately after the last '.' in the filename.  If the filename is
  already a leafname, it returns a pointer to the start of 'filename'
*/


char *Filing_MakePath(char *newpath, const char *dirname, const char *leafname);
/*
  This constructs a full pathname in 'newpath' for the corresponding
  directory 'dirname' and file leafname 'leafname' you pass in.

  You should ensure that the string newpath is big enough to hold the
  resulting string (ie. dirname+'.'+leafname+'\0')
*/


os_error *Filing_CanonicalisePath(const char *pathname, char *buffer,
                                  int size, int *spare);
/*
  This is a veneer for OS_FSControl 37, and converts a wildcarded pathname
  to a canonicalised path.  eg. the pathname "a" may be resolved to
  "ADFS::HardDisc4.$.a".  "a*" will be resolved to the first file in the
  current directory starting with 'a'.  If there is no matching file it will
  be resolved to eg. "ADFS::HardDisc4.$.a*".

  'pathname' is the pathname to convert, 'buffer' is the pointer to the buffer
  where the canonicalised path will be stored and 'size' is the length of that
  buffer.  It returns the number of spare bytes in the buffer in 'spare'.

  You can use a two-pass system by passing in the pathname, but setting the
  buffer to NULL and the size to zero.  This will result in 'spare' being
  filled with -(length of canonicalised name).  You can then allocate
  (1 - spare) bytes for the buffer (to allow for the nul terminator) and
  get the buffer filled.  If it works, spare will contain 1, representing the
  terminator.

  The function returns NULL if there is no error.
*/


typedef os_error *(Filing_ScanDir_StartDir) (const char *dirname,
                                             filing_fulldirentry *dirdata);
/*
  This is the type of function that is called by Filing_ScanDir (and similar
  functions) every time a new directory is scanned.  If this handler
  function returns non-NULL the scan will be aborted and Filing_ScanDir will
  return this error.

  'dirname' is the directory name and 'dirdata' is the data about the
  directory, when available.
*/


typedef os_error *(Filing_ScanDir_FoundFile) (const char *dirname,
                                              filing_fulldirentry *filedata);
/*
  This is the type of function that is called by Filing_ScanDir (and similar
  functions) every time a file is found.  If this handler function returns
  non-NULL the scan will be aborted and Filing_ScanDir will return this error.

  'dirname' is the directory name and 'filedata' is the data about the current
  file inside the directory.
*/


typedef os_error *(Filing_ScanDir_EndDir) (const char *dirname,
                                           filing_fulldirentry *dirdata);
/*
  This is the type of function that is called by Filing_ScanDir (and similar
  functions) every time a scan of a directory is finished.  If this handler
  function returns non-NULL the scan will be aborted and Filing_ScanDir
  will return this error.

  'dirname' is the directory name and 'dirdata' is the data about the
  directory, when it is available.
*/


os_error *Filing_ScanDir(const char *dirname,
                         Filing_ScanDir_StartDir *startdirproc,
                         Filing_ScanDir_FoundFile *foundfileproc,
                         Filing_ScanDir_EndDir *enddirproc);
/*
  This scans a directory tree calling the specified handler functions when
  it starts a new directory, finds a file, or finishes scanning a directory.
  This function may be called from one of the handler functions it invokes -
  it is reentrant.

  Subdirectories may be scanned before the parent directory has been
  completely scanned, so 'startdirproc' may be called more than once before
  the corresponding 'enddirproc' is called.  If you need to scan a complete
  directory before starting the scan of a subdirectory use
  Filing_ScanDirByDir.

  The 'startdirproc' is called every time a new directory is started
  (including the one specified by by 'dirname') before the files within it
  are scanned.

  The 'foundfileproc' is called every time a new file or directory is found.

  The 'enddirproc' is called whenever a directory scan is finished for a
  directory, ie. after all the files in it have been scanned.

  Any of the handler functions may be given as NULL, and they will then not
  be called.

  This function returns NULL if there is no error, or the error generated by
  RISC OS or returned from one of the handler functions.
*/


os_error *Filing_ScanDirByDir(const char *dirname,
                              Filing_ScanDir_StartDir *startdirproc,
                              Filing_ScanDir_FoundFile *foundfileproc,
                              Filing_ScanDir_EndDir *enddirproc);
/*
  This works almost exactly the same as Filing_ScanDir, except it finishes
  scanning one directory before recursing into subdirectories.  This isn't
  as quick as Filing_ScanDir so if you don't care about the order, use that
  instead.

  It is easy to stop this function from recursing into subdirectories by
  returning an error eg. ((os_error *) -1) from the 'enddirproc' so that
  only the first directory will be scanned and the error returned by
  Filing_ScanDirByDir.

  This function returns NULL if there is no error, or the error generated by
  RISC OS or returned from one of the handler functions.
*/

#ifdef __cplusplus
}
#endif

#endif
