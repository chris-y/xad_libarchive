/* XAD libarchive macros 
 * (c) 2019 Chris Young <chris@unsatisfactorysoftware.co.uk>
  */
  
#ifndef XAD_LIBARCHIVE_MACROS_H
#define XAD_LIBARCHIVE_MACROS_H 1

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

#include <proto/exec.h>
#include <proto/utility.h>

#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>


#define XAD_MACRO_RECOGFILE(CLIENT) BOOL CLIENT##_RecogData(ULONG size, STRPTR data,	\
								struct XadMasterIFace *IXadMaster)

#define XAD_MACRO_GETINFO(CLIENT,LIBARCHIVECLIENT) LONG CLIENT##_GetInfo(struct xadArchiveInfo *ai, struct XadMasterIFace *IXadMaster)	\
	{	\
		return xad_common_GetInfo(ai, IXadMaster, &archive_read_support_format_##LIBARCHIVECLIENT);	\
	}

#define XAD_MACRO_UNARCHIVE(CLIENT,LIBARCHIVECLIENT) LONG CLIENT##_UnArchive(struct xadArchiveInfo *ai, struct XadMasterIFace *IXadMaster)	\
	{	\
		return xad_common_UnArchive(ai, IXadMaster, &archive_read_support_format_##LIBARCHIVECLIENT);	\
	}
	
#define XAD_MACRO_CLIENT(CLIENT,MODNAME,REPLACE,NEXTCLIENT) const struct xadClient CLIENT##_Client = {	\
	(struct xadClient *)NEXTCLIENT, XADCLIENT_VERSION, XADMASTERVERSION, VERSION, REVISION,	\
	8, XADCF_FILEARCHIVER|XADCF_FREEFILEINFO|XADCF_FREEXADSTRINGS,	\
	REPLACE /* Type identifier. Normally should be zero */, MODNAME,	\
	(BOOL (*)()) CLIENT##_RecogData, (LONG (*)()) CLIENT##_GetInfo,	\
	(LONG (*)()) CLIENT##_UnArchive, (void (*)()) xad_common_Free };
	
#endif
