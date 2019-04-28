/* Header file for XAD_libarchive client
 * (c) 2019 Chris Young <chris@unsatisfactorysoftware.co.uk>
 */
 
#ifndef XAD_LIBARCHIVE_COMMON_H
#define XAD_LIBARCHIVE_COMMON_H 1

/* set to stream data rather than reading all in memory */
#define STREAMED_DATA 1

/* also use seek callback */
//#define SEEK_CALLBACK 1

/* maximum amount of data to read at a time */
#define MAXCHUNKSIZE 1024

#include <proto/xadmaster.h>
#include <exec/types.h>
#include <libarchive/archive.h>

struct callbackuserdata {
	struct xadArchiveInfo *ai;
	struct xadMasterIFace *IxadMaster;
	char *inbuffer;
};

#if 0
struct xadlibarchiveprivate {
	int idx;
	struct archive *a;
	struct callbackuserdata *cbdata;
};
#endif

LONG xad_common_GetInfo(struct xadArchiveInfo *ai, struct XadMasterIFace *IXadMaster, void (*read_support_func)(struct archive *a));
LONG xad_common_UnArchive(struct xadArchiveInfo *ai, struct XadMasterIFace *IXadMaster, void (*read_support_func)(struct archive *a));
void xad_common_Free(struct xadArchiveInfo *ai, struct XadMasterIFace *IXadMaster);
#endif

