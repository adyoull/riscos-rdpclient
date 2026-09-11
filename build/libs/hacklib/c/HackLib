
#include "DeskLib:Msgs.h"
#include "DeskLib:Error.h"
#include "DeskLib:WimpSWIs.h"

#include <stdarg.h>

static char err_msg[128];

char *msgs_lookup(char *tag_and_default)
{

  /* Msgs_Lookup(tag_and_default, err_msg, 127);

  return(&err_msg);
  */
  return(tag_and_default);
}

void werr(int fatal, char* format, ...)
{
  va_list ap;

  va_start(ap, format);
  if(fatal != 0)
     Error_ReportFatal(0, format, ap);
  else
     Error_Report(0, format, ap);
  va_end(ap);
}

void wimpt_noerr(os_error *e)
{

  if(e != 0)
     Error_ReportFatal(0, "Flex has suffered a fatal internal error and must crash this program immediately");

}

os_error *wimp_slotsize(int *currentslot /*inout*/,
                        int *nextslot /*inout*/,
                        int *freepool /*out*/)
{
  return(Wimp_SlotSize(currentslot, nextslot, freepool));
}
