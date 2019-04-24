/* XAD RAR client
 * by Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_RAR_C
#define XADMASTER_RAR_C

#include <proto/exec.h>

#ifdef __amigaos4__
struct ExecIFace *IExec;
struct Library *newlibbase;
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
#include "xad_rar.h"
#include <proto/exec.h>
#include <proto/utility.h>

#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>

#include "RAR_rev.h"

#ifndef XADMASTERFILE
#define rar_Client		FirstClient
#define NEXTCLIENT		0
#ifdef __amigaos4__
#define XADMASTERVERSION	13
#else
#define XADMASTERVERSION	12
#endif
const char *version = VERSTAG;
#endif

#ifdef __amigaos4__
BOOL rar_RecogData(ULONG size, STRPTR data,
struct XadMasterIFace *IXadMaster)
#else
ASM(BOOL) rar_RecogData(REG(d0, ULONG size), REG(a0, STRPTR data),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  if((data[0]=='R') & (data[1]=='a') & (data[2]=='r') & (data[3]=='!') & (data[4]==0x1A) & (data[5]==0x07)) {
#ifdef XAD_RAR5
	if((data[6]==0x01) & (data[7]=0x00))
		return 1; /* known file */
#endif
#ifdef XAD_RAR4
	if((data[6]==0x00))
		return 1; /* known file */
#endif
  }

    return 0; /* unknown file */
}

#ifdef __amigaos4__
LONG rar_GetInfo(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster)
#else
ASM(LONG) rar_GetInfo(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	return xad_common_GetInfo(ai, IXadMaster, &archive_read_support_format_rar);
}

#ifdef __amigaos4__
LONG rar_UnArchive(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster)
#else
ASM(LONG) rar_UnArchive(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	return xad_common_UnArchive(ai, IXadMaster, &archive_read_support_format_rar);
}

/* You need to complete following structure! */
const struct xadClient rar_Client = {
NEXTCLIENT, XADCLIENT_VERSION, XADMASTERVERSION, VERSION, REVISION,
6, XADCF_FILEARCHIVER|XADCF_FREEFILEINFO|XADCF_FREEXADSTRINGS,
0 /* Type identifier. Normally should be zero */, XAD_MOD_NAME,
(BOOL (*)()) rar_RecogData, (LONG (*)()) rar_GetInfo,
(LONG (*)()) rar_UnArchive, (void (*)()) xad_common_Free };

#ifndef __amigaos4__
void main(void)
{
}
#endif
#endif /* XADMASTER_RAR_C */
