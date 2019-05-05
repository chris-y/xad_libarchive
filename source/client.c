/* XAD libarchive client
 * by Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#include "libarchive_rev.h"
#include "macros.h"

#ifndef XADMASTERFILE
#define rar5_Client		FirstClient
#define rar5_NextClient &7zip_Client
#ifdef CLIENT_ZIP
#define 7zip_NextClient	&zip_Client
#else
#define 7zip_NextClient 0
#endif
#ifdef __amigaos4__
#define XADMASTERVERSION	13
#else
#define XADMASTERVERSION	12
#endif
const char *version = VERSTAG "built with " ARCHIVE_VERSION_STRING "\0";
#endif

#ifdef CLIENT_ZIP
XAD_MACRO_RECOGFILE(zip)
{
  if((data[0]=='P') & (data[1]=='K') & (data[2]==0x03) & (data[3]==0x04)) {
		return 1; /* known file */
  }

    return 0; /* unknown file */
}

XAD_MACRO_GETINFO(zip)
XAD_MACRO_UNARCHIVE(zip)
XAD_MACRO_CLIENT(zip, "Zip", XADCID_ZIP, 0)
#endif

#ifdef CLIENT_7ZIP
XAD_MACRO_RECOGFILE(7zip)
{
  if((data[0]=='7') & (data[1]=='z') & (data[2]==0xBC) & (data[3]==0xAF) & (data[4]==0x27) & (data[5]==0x1C)) {
		return 1; /* known file */
  }

    return 0; /* unknown file */
}

XAD_MACRO_GETINFO(7zip)
XAD_MACRO_UNARCHIVE(7zip)
XAD_MACRO_CLIENT(7zip, "7-Zip", 0, 7zip_NextClient)
#endif

#ifdef CLIENT_RAR5
XAD_MACRO_RECOGFILE(rar5)
{
  if((data[0]=='R') & (data[1]=='a') & (data[2]=='r') & (data[3]=='!') & (data[4]==0x1A) & (data[5]==0x07) & (data[6]==0x01) & (data[7]==0x00)) {
		return 1; /* known file */
  }

    return 0; /* unknown file */
}

XAD_MACRO_GETINFO(rar5)
XAD_MACRO_UNARCHIVE(rar5)
XAD_MACRO_CLIENT(rar5, "RAR5", 0, rar5_NextClient)
#endif
