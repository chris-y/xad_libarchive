/* XAD_libarchive common routines
 * (c) 2019 Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#include "sys/types.h"
 
#include "common.h"

#include <proto/xadmaster.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>

struct Library *SysBase;
struct ExecIFace *IExec;
struct Library *newlibbase;
struct Interface *INewlib;
 
 #ifndef MEMF_PRIVATE
#define MEMF_PRIVATE 0
#endif

long arc_to_xad_error(int err)
{
	switch(err) {
		case ARCHIVE_EOF:
			return XADERR_INPUT;
		break;
		case ARCHIVE_OK:
			return XADERR_OK;
		break;
		case ARCHIVE_RETRY:
			return XADERR_UNKNOWN;
		break;
		case ARCHIVE_WARN:
			return XADERR_UNKNOWN;
		break;
		case ARCHIVE_FAILED:
			return XADERR_UNKNOWN;
		break;
		case ARCHIVE_FATAL:
			return XADERR_ILLEGALDATA;
		break;
	}
	
	return XADERR_UNKNOWN;
}

#ifdef STREAMED_DATA
static ssize_t
xad_read(struct archive *a, void *client_data, const void **buff)
{
  struct callbackuserdata *cbdata = client_data;
	struct XadMasterIFace *IXadMaster = cbdata->IxadMaster;

	int size = MAXCHUNKSIZE;
	
  int pos = cbdata->ai->xai_InPos;
  
  if(cbdata->inbuffer == NULL) {
		cbdata->inbuffer = xadAllocVec(MAXCHUNKSIZE, MEMF_CLEAR);
  }

  	if((cbdata->ai->xai_InSize - pos) < size) size = (cbdata->ai->xai_InSize - pos);
  
  xadHookAccess(XADAC_READ, size, cbdata->inbuffer, cbdata->ai);
  *buff = cbdata->inbuffer;  
  
#ifdef DEBUG
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	DebugPrintF("read: %ld, pos: %ld, old pos: %ld\n", cbdata->ai->xai_InPos - pos, cbdata->ai->xai_InPos, pos);
#endif
  
  return (ssize_t)(cbdata->ai->xai_InPos - pos);
}

static la_int64_t xad_skip(struct archive *a, void *client_data, int64_t request)
{
	struct callbackuserdata *cbdata = client_data;
	struct XadMasterIFace *IXadMaster = cbdata->IxadMaster;
	
	int pos = cbdata->ai->xai_InPos;
	
	if((cbdata->ai->xai_InSize - pos) < request) request = (cbdata->ai->xai_InSize - pos);
	
	xadHookAccess(XADAC_INPUTSEEK, (uint32)request, NULL, cbdata->ai);

#ifdef DEBUG
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	DebugPrintF("skip: %ld, pos: %ld, old pos: %ld, orig req: %lld\n", cbdata->ai->xai_InPos - pos, cbdata->ai->xai_InPos, pos, request);
#endif

	return (int64_t)(cbdata->ai->xai_InPos - pos);
	
}

#ifdef SEEK_CALLBACK
static la_int64_t xad_seek(struct archive *a, void *client_data, int64_t offset, int whence)
{
	struct callbackuserdata *cbdata = client_data;
	struct XadMasterIFace *IXadMaster = cbdata->IxadMaster;
	
	int pos = cbdata->ai->xai_InPos;
	
	switch(whence) {
		case SEEK_SET:
			if(offset > cbdata->ai->xai_InSize) return ARCHIVE_FATAL;
			xadHookAccess(XADAC_INPUTSEEK, (uint32)offset - pos, NULL, cbdata->ai);
		break;
		
		case SEEK_CUR:
			if((cbdata->ai->xai_InPos + offset) > cbdata->ai->xai_InSize) return ARCHIVE_FATAL;
			xadHookAccess(XADAC_INPUTSEEK, (uint32)offset, NULL, cbdata->ai);
		break;
		
		case SEEK_END:
			xadHookAccess(XADAC_INPUTSEEK, (uint32)offset + cbdata->ai->xai_InSize - pos, NULL, cbdata->ai);
		break;
		
		default:
			return ARCHIVE_FATAL;
		break;
	}
	
#ifdef DEBUG
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	DebugPrintF("seek: %ld, pos: %ld, old pos: %ld, orig req: %lld / %lx\n", cbdata->ai->xai_InPos - pos, cbdata->ai->xai_InPos, pos, offset, whence);
#endif

	return ARCHIVE_OK; //(int64_t)(cbdata->ai->xai_InPos - pos);
	
}
#endif
#endif

static la_ssize_t
xad_write_data(void *client_data, const void *buff, size_t n, int64_t offset)
{
  struct callbackuserdata *cbdata = client_data;
  struct XadMasterIFace *IXadMaster = cbdata->IxadMaster;
	
  int pos = cbdata->ai->xai_OutPos;
  
  if(pos != offset) {
  	xadHookAccess(XADAC_OUTPUTSEEK, (uint32)(offset - pos), NULL, cbdata->ai);
  }
  
  xadHookAccess(XADAC_WRITE, n, buff, cbdata->ai);
  
  return (ssize_t)(cbdata->ai->xai_OutPos - pos);
}

static int
copy_data(struct archive *ar, void *client_data)
{
	int r;
	const void *buff;
	size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
	int64_t offset;
#else
	off_t offset;
#endif

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r != ARCHIVE_OK)
			return (r);
		xad_write_data(client_data, buff, size, offset);
	}
}

#ifdef __amigaos4__
LONG xad_common_GetInfo(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster, void (*read_support_func)(struct archive *a))
#else
ASM(LONG) rar_GetInfo(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase), void (*read_support_func)(struct archive *a))
#endif
{
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
	
	a = archive_read_new();

	read_support_func(a);

	cbdata = xadAllocVec(sizeof(struct callbackuserdata), MEMF_PRIVATE | MEMF_CLEAR);

#ifdef STREAMED_DATA
	cbdata->ai = ai;
	cbdata->IxadMaster = IXadMaster;
	
	archive_read_set_read_callback(a, xad_read);
	archive_read_set_skip_callback(a, xad_skip);
#ifdef SEEK_CALLBACK
	archive_read_set_seek_callback(a, xad_seek);
#endif
	archive_read_set_callback_data(a, cbdata);
	archive_read_open1(a);
#else
	cbdata->inbuffer = xadAllocVec(ai->xai_InSize, MEMF_CLEAR);
	xadHookAccess(XADAC_READ, ai->xai_InSize, cbdata->inbuffer, ai);

	archive_read_open_memory(a, cbdata->inbuffer, ai->xai_InSize);

#endif

	int i = 0;

	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
	  	fi = (struct xadFileInfo *) xadAllocObjectA(XADOBJ_FILEINFO, NULL);
		if (!fi) return(XADERR_NOMEMORY);
		
		fi->xfi_PrivateInfo = i;
		fi->xfi_DataPos = 0;
		fi->xfi_Size = archive_entry_size(entry);
		if (!(fi->xfi_FileName = xadConvertName(CHARSET_HOST,
						//	XAD_CHARACTERSET, CHARSET_UNICODE_UTF8,
						//	XAD_STRINGSIZE,strlen(archive_entry_pathname(entry)),
							XAD_CSTRING, archive_entry_pathname(entry),
							TAG_DONE))) return(XADERR_NOMEMORY);

		xadConvertDates(XAD_DATEUNIX,archive_entry_mtime(entry),
					XAD_GETDATEXADDATE,&fi->xfi_Date,
					TAG_DONE);

//		fi->xfi_DosProtect = templist->item.FileAttr;    archive_entry_perm???

//		fi->xfi_CrunchSize  = templist->item.PackSize; //(long) (db->Database.PackSizes[i] << 32); //fi->xfi_Size;

		fi->xfi_Flags = 0;

		if(archive_entry_filetype(entry) & AE_IFDIR)
		{
			fi->xfi_Flags |= XADFIF_DIRECTORY;
		}
		
		if(archive_entry_is_encrypted(entry)) {
			fi->xfi_Flags |= XADFIF_CRYPTED;
		}

		if ((err = xadAddFileEntryA(fi, ai, NULL))) return(XADERR_NOMEMORY);
		
		
		i++;
	}

	archive_read_close(a);
	archive_read_free(a);
  
  	if(cbdata) {
		if(cbdata->inbuffer != NULL) {
			xadFreeObjectA(cbdata->inbuffer, NULL);
			cbdata->inbuffer = NULL;
		}
		
		xadFreeObjectA(cbdata, NULL);
		cbdata = NULL;
	}
  
  
	return(err);
}

#ifdef __amigaos4__
LONG xad_common_UnArchive(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster, void (*read_support_func)(struct archive *a))
#else
ASM(LONG) xad_common_UnArchive(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase), void (*read_support_func)(struct archive *a)))
#endif
{
  	struct xadFileInfo *fi = ai->xai_CurFile;
	long err=XADERR_DECRUNCH;
	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int idx = fi->xfi_PrivateInfo;
	int r;
	struct callbackuserdata *cbdata;
	UBYTE *outbuffer = NULL;

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
	
	xadHookAccess(XADAC_INPUTSEEK, - ai->xai_InPos, NULL, ai);
	
	a = archive_read_new();

	read_support_func(a);

	if(ai->xai_Password != NULL) {
		r = archive_read_add_passphrase(a, ai->xai_Password);
		
		if (r != ARCHIVE_OK) {
			err = arc_to_xad_error(r);
			DebugPrintF("xad_libarchive error: %s\n", archive_error_string(a));
			return err;
		}
	}

	cbdata = xadAllocVec(sizeof(struct callbackuserdata), MEMF_PRIVATE | MEMF_CLEAR);
	cbdata->ai = ai;
	cbdata->IxadMaster = IXadMaster;

#ifdef STREAMED_DATA
	archive_read_set_read_callback(a, xad_read);
	archive_read_set_skip_callback(a, xad_skip);
#ifdef SEEK_CALLBACK
	archive_read_set_seek_callback(a, xad_seek);
#endif
	archive_read_set_callback_data(a, cbdata);
	archive_read_open1(a);
#else
	cbdata->inbuffer = xadAllocVec(ai->xai_InSize, MEMF_CLEAR);
	xadHookAccess(XADAC_READ, ai->xai_InSize, cbdata->inbuffer, ai);

	archive_read_open_memory(a, cbdata->inbuffer, ai->xai_InSize);
#endif
	
	int i = 0;

	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		if(i == idx) {
			// extract
			err = XADERR_OK;
			
			r = copy_data(a, cbdata);
			if (r != ARCHIVE_OK) {
				err = arc_to_xad_error(r);
				DebugPrintF("xad_libarchive error: %s\n", archive_error_string(a));
				break;
			}
			
			break;
		}

		i++;
	}
	
	if((fi->xfi_Flags & XADFIF_CRYPTED) && (err != XADERR_OK)) {
		/* This is a bit of a hack because libarchive doesn't set
		 * an error code specifically for "wrong password".  So, if
		 * decrunch fails on an encrypted archive assume this is the
		 * reason otherwise user won't ever get re-prompted.
		 */
		err = XADERR_PASSWORD;
	}

	
	archive_read_close(a);
	archive_read_free(a);
	
#ifndef STREAMED_DATA
	if(outbuffer != NULL) {
		xadFreeObjectA(outbuffer, NULL);
		outbuffer = NULL;
	}
#endif			
	
  	if(cbdata) {
		if(cbdata->inbuffer != NULL) {
			xadFreeObjectA(cbdata->inbuffer, NULL);
			cbdata->inbuffer = NULL;
		}
		
		xadFreeObjectA(cbdata, NULL);
		cbdata = NULL;
	}
  
	return(err);
}

#ifdef __amigaos4__
void xad_common_Free(struct xadArchiveInfo *ai,
struct XadMasterIFace *IXadMaster)
#else
ASM(void) xad_common_Free(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  /* This function needs to free all the stuff allocated in info or
  unarchive function. It may be called multiple times, so clear freed
  entries!
  */

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
