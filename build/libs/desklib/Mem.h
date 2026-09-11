/*
    ####             #    #     # #
    #   #            #    #       #          The FreeWare C library for
    #   #  ##   ###  #  # #     # ###             RISC OS machines
    #   # #  # #     # #  #     # #  #   ___________________________________
    #   # ####  ###  ##   #     # #  #
    #   # #        # # #  #     # #  #    Please refer to the accompanying
    ####   ### ####  #  # ##### # ###    documentation for conditions of use
    ________________________________________________________________________

    File:    Mem.h
    Author:  Copyright © 1993, 1994, 1995 Jason Williams and Jason Howat
    Version: 2.01 (17 Apr 2005)
    Purpose: Dynamic memory manager
*/

#ifndef __dl_mem_h
#define __dl_mem_h

/* Abstract */
/*
  Mem provides a 'heap' of memory above the area set up by the Shared C
  Library for mallocs and stackspace, extending up to the end of the
  Wimpslot which is adjusted up and down as necessary.

  The system works with "anchors" - pointers to memory blocks in the heap
  which can be updated automatically by Mem when the blocks are rearranged.
  The allows Mem to compact the heap or rearrange the blocks in order to
  waste the least space.

  You can choose to let Mem rearrange the blocks whenever it wants, only when
  it needs to or only when you explicitly request that the heap be compacted.
  You choose this by setting the value of mem_autocompact, so you can change
  the behaviour in different parts of the program depending on how the situation
  can best be managed.

  The idea is that you call Mem_Compact before calling Wimp_Poll, which should
  ensure that the heap has little wasted space, but doesn't waste time by
  compacting after every single operation.  This leads to quicker allocation
  and deallocation of chunks.

  Note that by default, Mem will only autocompact if it has no choice, but
  *may* autocompact during any Mem_ call.  Avoid pointer arithmetic unless
  mem_autocompact is set to mem_NOCOMPACT.
*/

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


typedef void *mem_anchor;
/*
  The Mem functions accept a pointer to a mem_anchor to allocate,
  seallocate, or generally do anything with memory in the heap.  You pass
  a pointer to your anchor to the various Mem routines so that they can
  alter the contents of the anchor whenever necessary.
*/


extern int mem_autocompact;
/*
  This variable controls when Mem is allowed (or required) to compact the heap.
  If you set it to mem_NOCOMPACT, it will never move any of the blocks around
  except when you call Mem_Compact.  mem_PARTCOMPACT means that it is only
  compacted when it might free enough space to allocate a block, or when you
  call Mem_Compact.  This is the default setting.  mem_FULLCOMPACT means that
  whenever you do a Mem operation, the heap is compacted.  This offers no
  real advantage over the other options , and so is not recommended.

  You can change the value at any time and the change will take immediate effect.
  If you want to be able to rely on pointers not changing during a single
  function, simply set mem_autocompact to mem_NOCOMPACT at the start and back
  to mem_PARTCOMPACT at the end.
*/


typedef enum
{
  mem_NOCOMPACT   = 0,
  mem_FASTCOMPACT = 1,
  mem_PARTCOMPACT = 1,
  mem_FULLCOMPACT = 2
} mem_compaction;
/*
  These are the different options for the auto-compacting of the Mem heap,
  which is controlled by mem_autocompact.
*/


extern BOOL Mem_Initialise(void);
/*
  Initialises the Mem system ready for use.

  Note that this locks down the malloc and stack memory area to the current
  WimpSlot size at the point of calling, and builds a Mem heap above this.
  Malloc and stack allocation will not be able to get more memory than is
  originally available in your WimpSlot, so get it right!
*/


extern BOOL Mem_Alloc(mem_anchor *anchor, int numbytes);
/*
  This attempts to allocate the given amount of memory to the anchor pointed
  to by 'anchor'.  This updates the anchor to point to the allocated block,
  or NULL if it fails to allocate the memory.  The returned block is
  word-aligned.

  It returns TRUE if successful, FALSE otherwise.
*/


extern BOOL Mem_MidExtend(mem_anchor *anchor, int at, int by);
/*
  This function attempts to alter the size of an existing  Mem block, by
  inserting a block of 'by' bytes at offset 'at' within the block. If 'by'
  is negative then the -by bytes below 'at' will be deleted. Either way,
  existing data (except that deleted) is preserved.
*/


extern void Mem_MoveAnchor(mem_anchor *from, mem_anchor *to);
/*
  This function copies moves an anchor from one variable to another.
  This is necessary because, due to the relocating nature of Mem chunks,
  anchors are automatically updated. If you simply copy the value, the
  new anchor will not be updated, and the place where the old anchor was
  stored will instead by overwritten.

  If this function works, the anchor pointed to by 'from' will contain NULL,
  and the anchor pointed to by 'to' will contain the previous contents of 'from'.
  Otherwise, 'to' will contain NULL and 'from' will be left alone.
*/


extern void Mem_Free(mem_anchor *anchor);
/*
  This releases the given block of memory back to the free pool.  The anchor's
  contents will be set to NULL.
*/


extern void Mem_Compact(void);
/*
  This compacts the Mem heap, moving free space to the end and releasing what
  it can back to the Wimp.  This is likely to move some Mem chunks around if
  there is any free space in the heap at all.  You can't rely on anchors
  being constant across calls to this function.

  Ideally, call this just before calling Wimp_Poll so that the Wimp has access
  to as much memory as possible, and to keep the heap tidy.

  If mem_autocompact is not set to mem_NOCOMPACT, any other Mem function could
  also call this one.
*/


extern int  Mem_Size(mem_anchor *anchor);
/*
  This returns the current size (in bytes) of the given Mem chunk.
*/


extern BOOL Mem_CheckHeap(void);
/*
  This checks the state of the Heap to ensure that all links are intact and
  internally consistent.  It raises an error with Error_Report for the first
  problem it finds, and returns FALSE.  If the heap is intact, it returns TRUE.

  Useful when debugging to see if you've accidentally written over part of
  the Mem heap.
*/


#ifdef __cplusplus
}
#endif


#endif
