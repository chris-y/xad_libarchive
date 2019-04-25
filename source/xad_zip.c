/* XAD ZIP client
 * by Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_ZIP_C
#define XADMASTER_ZIP_C

#include <proto/exec.h>

#ifdef __amigaos4__
struct ExecIFace *IExec;
struct LibZIPy *newlibbase;
struct Interface *INewlib;
#include "sys/types.h"
#else
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include "libnixstuff.h"
#endif

#include <proto/xadmaster.h>
#include <dos/dos.h>
#include "SDI_compiler.h"
#include "common.h"
#include "xad_zip.h"
#include <proto/exec.h>
#include <proto/utility.h>

#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>

#include "Zip_rev.h"

#ifndef XADMASTERFILE
#define ZIP_Client		FirstClient
#define NEXTCLIENT		0
#ifdef __amigaos4__
#define XADMASTERVERSION	13
#else
#define XADMASTERVERSION	12
#endif
const char *version = VERSTAG;
#endif

#ifdef __amigaos4__
BOOL ZIP_RecogData(ULONG size, STRPTR data,
struct XadMasterIFace *IXadMaster)
#else
ASM(BOOL) ZIP_RecogData(REG(d0, ULONG size), REG(a0, STRPTR data),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  if((data[0]=='P') & (data[1]=='K') & (data[2]==0x03) & (data[3]==0x04)) {
		return 1; /* known file */
  }

    return 0; /* unknown file */
}

#ifdef __amigaos4__
LONG ZIP_GetInfo(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster)
#else
ASM(LONG) ZIP_GetInfo(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	return xad_common_GetInfo(ai, IXadMaster, &archive_read_support_format_zip);
}

#ifdef __amigaos4__
LONG ZIP_UnArchive(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster)
#else
ASM(LONG) ZIP_UnArchive(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	return xad_common_UnArchive(ai, IXadMaster, &archive_read_support_format_zip);
}

/* You need to complete following structure! */
const struct xadClient ZIP_Client = {
NEXTCLIENT, XADCLIENT_VERSION, XADMASTERVERSION, VERSION, REVISION,
6, XADCF_FILEARCHIVER|XADCF_FREEFILEINFO|XADCF_FREEXADSTRINGS,
0 /* Type identifier. Normally should be zero */, XAD_MOD_NAME,
(BOOL (*)()) ZIP_RecogData, (LONG (*)()) ZIP_GetInfo,
(LONG (*)()) ZIP_UnArchive, (void (*)()) xad_common_Free };

#ifndef __amigaos4__
void main(void)
{
}
#endif
#endif /* XADMASTER_ZIP_C */
