/* XAD RAR client
 * by Chris Young <chris@unsatisfactorysoftware.co.uk>
 * and Stephan Matzke <stephan.matzke.adev@gmx.de>
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
#include "xad_rar.h"
#include <proto/exec.h>
#include <proto/utility.h>

#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>

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

#ifndef MEMF_PRIVATE
#define MEMF_PRIVATE 0
#endif

ssize_t
xad_rar_read(struct archive *a, void *client_data, const void **buff)
{
  struct callbackuserdata *cbdata = client_data;
	struct XadMasterIFace *IXadMaster = cbdata->IxadMaster;

  int pos = cbdata->ai->xai_InPos;
  
  if(cbdata->inbuffer == NULL) {
		cbdata->inbuffer = xadAllocVec(1024, MEMF_CLEAR);
  }

  xadHookAccess(XADAC_READ, 1024, cbdata->inbuffer, cbdata->ai);
  *buff = cbdata->inbuffer;  
  
/*
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	DebugPrintF("read: %ld, pos: %ld, old pos: %ld\n", cbdata->ai->xai_InPos - pos, cbdata->ai->xai_InPos, pos);
*/
  
  return (ssize_t)(cbdata->ai->xai_InPos - pos);
}

int
xad_rar_close(struct archive *a, void *client_data)
{
  struct callbackuserdata *cbdata = client_data;
	struct XadMasterIFace *IXadMaster = cbdata->IxadMaster;

  if(cbdata->inbuffer != NULL) {
		xadFreeObjectA(cbdata->inbuffer, NULL);
		cbdata->inbuffer = NULL;
  }

    xadHookAccess(XADAC_INPUTSEEK, - cbdata->ai->xai_InPos, NULL, cbdata->ai);
  
  return (ARCHIVE_OK);
}


#ifdef __amigaos4__
BOOL rar_RecogData(ULONG size, STRPTR data,
struct xadMasterIFace *IxadMaster)
#else
ASM(BOOL) rar_RecogData(REG(d0, ULONG size), REG(a0, STRPTR data),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  if((data[0]=='R') & (data[1]=='a') & (data[2]=='r') & (data[3]=='!'))
    return 1; /* known file */
  else
    return 0; /* unknown file */
}

#ifdef __amigaos4__
LONG rar_GetInfo(struct xadArchiveInfo *ai,
struct xadMasterIFace *IxadMaster)
#else
ASM(LONG) rar_GetInfo(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	struct XadMasterIFace *IXadMaster = IxadMaster;
	struct xadrarprivate *xadrar;
	struct xadFileInfo *fi;
	long err=XADERR_OK;
	struct callbackuserdata *cbdata;
	struct archive *a;
	struct archive_entry *entry;

	#ifdef __amigaos4__
    IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	
    newlibbase = OpenLibrary("newlib.library", 52);
    if(newlibbase)
        INewlib = GetInterface(newlibbase, "main", 1, NULL);
	#else
		libnixopen();
	#endif
	
	ai->xai_PrivateClient = xadAllocVec(sizeof(struct xadrarprivate),MEMF_PRIVATE | MEMF_CLEAR);
	xadrar = (struct xadrarprivate *)ai->xai_PrivateClient;

	cbdata = xadAllocVec(sizeof(struct callbackuserdata), MEMF_PRIVATE | MEMF_CLEAR);
	cbdata->ai = ai;
	cbdata->IxadMaster = IxadMaster;
	xadrar->cbdata = cbdata;
	
	a = archive_read_new();
	xadrar->a = a;
	archive_read_support_format_rar(a);
	archive_read_open(a, cbdata, NULL, xad_rar_read, xad_rar_close);
	
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
	  	fi = (struct xadFileInfo *) xadAllocObjectA(XADOBJ_FILEINFO, NULL);
		if (!fi) return(XADERR_NOMEMORY);
		
		fi->xfi_PrivateInfo = entry;
		fi->xfi_DataPos = 0;
		fi->xfi_Size = archive_entry_size(entry);
		if (!(fi->xfi_FileName = xadConvertName(CHARSET_HOST,
							XAD_CHARACTERSET, CHARSET_UNICODE_UTF8,
						//	XAD_STRINGSIZE,strlen(archive_entry_pathname(entry)),
							XAD_CSTRING, archive_entry_pathname_utf8(entry),
							TAG_DONE))) return(XADERR_NOMEMORY);

		xadConvertDates(XAD_DATEUNIX,archive_entry_ctime(entry),
					XAD_GETDATEXADDATE,&fi->xfi_Date,
					TAG_DONE);

//		fi->xfi_DosProtect = templist->item.FileAttr;    archive_entry_perm???

//		fi->xfi_CrunchSize  = templist->item.PackSize; //(long) (db->Database.PackSizes[i] << 32); //fi->xfi_Size;

		fi->xfi_Flags = 0;

		if(archive_entry_filetype(entry) & AE_IFDIR)
		{
			fi->xfi_Flags |= XADFIF_DIRECTORY;
		}

		if ((err = xadAddFileEntryA(fi, ai, NULL))) return(XADERR_NOMEMORY);
		
		
//		printf("%s\\n",archive_entry_pathname(entry));
	}

//	archive_read_free(a);
  
//	FreeVec(cbdata);
  
	return(err);
}

#ifdef __amigaos4__
LONG rar_UnArchive(struct xadArchiveInfo *ai,
struct xadMasterIFace *IxadMaster)
#else
ASM(LONG) rar_UnArchive(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	struct XadMasterIFace *IXadMaster = IxadMaster;
	struct xadrarprivate *xadrar = (struct xadrarprivate *)ai->xai_PrivateClient;
  	struct xadFileInfo *fi = ai->xai_CurFile;
	long err=XADERR_OK;
	ULONG data_size = 0;
	struct archive_entry *entry = (struct archive_entry *)fi->xfi_PrivateInfo;

	#ifdef __amigaos4__
	if(!newlibbase)
	{
    	IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	    newlibbase = OpenLibrary("newlib.library", 52);
    	if(newlibbase)
        	INewlib = GetInterface(newlibbase, "main", 1, NULL);
	}
	#else
		libnixopen();
	#endif
	
	err=XADERR_UNKNOWN;

	return(err);
}

#ifdef __amigaos4__
void rar_Free(struct xadArchiveInfo *ai,
struct xadMasterIFace *IxadMaster)
#else
ASM(void) rar_Free(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	struct XadMasterIFace *IXadMaster = IxadMaster;
  /* This function needs to free all the stuff allocated in info or
  unarchive function. It may be called multiple times, so clear freed
  entries!
  */

	struct xadrarprivate *xadrar = (struct xadrarprivate *)ai->xai_PrivateClient;

	if(xadrar->a) {
		archive_read_free(xadrar->a);
		xadrar->a = NULL;
	}
	
	xadFreeObjectA(ai->xai_PrivateClient,NULL);
	ai->xai_PrivateClient = NULL;

	#ifdef __amigaos4__
    DropInterface(INewlib);
    CloseLibrary(newlibbase);
	INewlib = NULL;
	newlibbase = NULL;
	#else
		libnixclose();
	#endif
}

/* You need to complete following structure! */
const struct xadClient rar_Client = {
NEXTCLIENT, XADCLIENT_VERSION, XADMASTERVERSION, VERSION, REVISION,
6, XADCF_FILEARCHIVER|XADCF_FREEFILEINFO|XADCF_FREEXADSTRINGS,
0 /* Type identifier. Normally should be zero */, "RAR",
(BOOL (*)()) rar_RecogData, (LONG (*)()) rar_GetInfo,
(LONG (*)()) rar_UnArchive, (void (*)()) rar_Free };

#ifndef __amigaos4__
void main(void)
{
}
#endif
#endif /* XADMASTER_RAR_C */
