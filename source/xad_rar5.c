/* XAD RAR5 client
 * by Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_RAR_C
#define XADMASTER_RAR_C

#include "RAR_rev.h"
#include "macros.h"

#ifndef XADMASTERFILE
#define rar5_Client		FirstClient
#define NEXTCLIENT		0
#ifdef __amigaos4__
#define XADMASTERVERSION	13
#else
#define XADMASTERVERSION	12
#endif
const char *version = VERSTAG;
#endif

XAD_MACRO_RECOGFILE(rar5)
{
  if((data[0]=='R') & (data[1]=='a') & (data[2]=='r') & (data[3]=='!') & (data[4]==0x1A) & (data[5]==0x07) & (data[6]==0x01) & (data[7]==0x00)) {
		return 1; /* known file */
  }

    return 0; /* unknown file */
}

XAD_MACRO_GETINFO(rar5)
XAD_MACRO_UNARCHIVE(rar5)
XAD_MACRO_CLIENT(rar5, "RAR5", 0)

#endif /* XADMASTER_RAR_C */
