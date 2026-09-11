#ifndef __dl_wimp_h
#include "DeskLib:Wimp.h"
#endif

#ifndef __dl_core_h
#include "DeskLib:Core.h"
#endif


#ifndef __dl_linklist_h
#include "DeskLib:LinkList.h"
#endif


typedef struct{

  linklist_header header;
  window_handle   mainwindow;
  BOOL            invalideventdata;
  linklist_header paneanchor;

} main_listelement;

typedef struct{

  linklist_header header;
  window_handle   mainwindow;
  window_handle   panewindow;
  wimp_point      paneoffset;
  wimp_point      panesize;
  pane2_flags     paneflags;

} pane_listelement;

/* width of window tools in OS units */
#define tool_SIZE 40

extern linklist_header main_anchor;

extern BOOL OpenEventHandler(event_pollblock *event, void *reference);

extern BOOL PaneOpenEventHandler(event_pollblock *event, void *reference);

extern BOOL ModeChangeMessageHandler(event_pollblock *event, void *reference);

extern pane_listelement *FindPaneWindow(main_listelement *mainelement, int panenumber);

extern main_listelement *FindMainWindow(window_handle window);

extern BOOL OpenWindow(window_openblock *openblock, main_listelement *mainelement);





