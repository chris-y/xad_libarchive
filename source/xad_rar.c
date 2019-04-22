/* XAD RAR client
 * by Chris Young <chris@unsatisfactorysoftware.co.uk>
 * and Stephan Matzke <stephan.matzke.adev@gmx.de>
 *
 * This client is built on a modified version of RAR_Extractor
 * by Shay Green which is based on the UnRAR source code
 * by Alexander L. Roshal.
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

#ifdef __amigaos4__
BOOL rar_RecogData(ULONG size, STRPTR data,
struct XadMasterIFace *IXadMaster)
#else
ASM(BOOL) rar_RecogData(REG(d0, ULONG size), REG(a0, STRPTR data),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  if((data[0]=='R') & (data[1]=='a') & (data[2]=='r') & (data[3]=='!') & (data[4]==0x1A) & (data[5]==0x07) & (data[6]==0x00))
    return 1; /* known file */
  else
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
	struct xadrarprivate *xadrar;
	struct xadFileInfo *fi;
	long err=XADERR_OK,filecounter;
	ArchiveList_struct *rarlist = NULL;
	ArchiveList_struct *templist = NULL;
	int i;

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

#ifdef __amigaos4__
	filecounter = urarlib_list(&rarlist, IXadMaster, ai );
#else
	filecounter = urarlib_list(&rarlist, xadMasterBase, ai );
#endif

	xadrar->List = rarlist;  // might need this later
	templist = rarlist;

	for (i = 0; i < filecounter; i++)
	{
		fi = (struct xadFileInfo *) xadAllocObjectA(XADOBJ_FILEINFO, NULL);
		if (!fi) return(XADERR_NOMEMORY);

		fi->xfi_PrivateInfo = templist;
		fi->xfi_DataPos = 0;
		fi->xfi_Size = templist->item.UnpSize;
		if (!(fi->xfi_FileName = xadConvertName(CHARSET_HOST,
							XAD_STRINGSIZE,templist->item.NameSize,
							XAD_CSTRING,templist->item.Name,
							TAG_DONE))) return(XADERR_NOMEMORY);

		xadConvertDates(XAD_DATEMSDOS,templist->item.FileTime,
					XAD_GETDATEXADDATE,&fi->xfi_Date,
					TAG_DONE);

		fi->xfi_DosProtect = templist->item.FileAttr;

		fi->xfi_CrunchSize  = templist->item.PackSize; //(long) (db->Database.PackSizes[i] << 32); //fi->xfi_Size;

		fi->xfi_Flags = 0;

		if(templist->item.FileAttr == 16)
		{
			fi->xfi_Flags |= XADFIF_DIRECTORY;
		}

		if ((err = xadAddFileEntryA(fi, ai, NULL))) return(XADERR_NOMEMORY);
		templist = (ArchiveList_struct*)templist->next;
    }

	return(err);
}

#ifdef __amigaos4__
LONG rar_UnArchive(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster)
#else
ASM(LONG) rar_UnArchive(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	struct xadrarprivate *xadrar = (struct xadrarprivate *)ai->xai_PrivateClient;
  	struct xadFileInfo *fi = ai->xai_CurFile;
	long err=XADERR_OK;
	ULONG data_size = 0;
	ArchiveList_struct *templist = (ArchiveList_struct *)fi->xfi_PrivateInfo;

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
	
#ifdef __amigaos4__
	if(!urarlib_get(&data_size, templist->item.Name, IXadMaster, ai))
#else
	if(!urarlib_get(&data_size, templist->item.Name, xadMasterBase, ai))
#endif
	{
		err=XADERR_UNKNOWN;
	}

	return(err);
}

#ifdef __amigaos4__
void rar_Free(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster)
#else
ASM(void) rar_Free(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  /* This function needs to free all the stuff allocated in info or
  unarchive function. It may be called multiple times, so clear freed
  entries!
  */

	struct xadrarprivate *xadrar = (struct xadrarprivate *)ai->xai_PrivateClient;

	urarlib_freelist(xadrar->List);

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
