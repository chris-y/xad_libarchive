/* XAD ZIP client
 * by Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_ZIP_C
#define XADMASTER_ZIP_C

#include "xad_zip.h"

#include "Zip_rev.h"

#ifndef XADMASTERFILE
#define zip_Client		FirstClient
#define NEXTCLIENT		0
#ifdef __amigaos4__
#define XADMASTERVERSION	13
#else
#define XADMASTERVERSION	12
#endif
const char *version = VERSTAG;
#endif

XAD_MACRO_RECOGFILE(zip)
{
  if((data[0]=='P') & (data[1]=='K') & (data[2]==0x03) & (data[3]==0x04)) {
		return 1; /* known file */
  }

    return 0; /* unknown file */
}

XAD_MACRO_GETINFO(zip)
XAD_MACRO_UNARCHIVE(zip)
XAD_MACRO_CLIENT(zip, "Zip")

#endif /* XADMASTER_ZIP_C */
