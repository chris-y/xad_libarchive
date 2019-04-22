/* Header file for XAD RAR client
 * Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_RAR_H
#define XADMASTER_RAR_H

/* define these to support RAR4 or RAR5 or both
 * RAR4 is also known as RAR3 and seems to have issues
 * RAR5 needs a newer than 3.3.3 libarchive
 */
#define XAD_RAR4 1 /* we used to call this RAR3 */
//#define XAD_RAR5 1

/* set to stream data rather than reading all in memory */
#define STREAMED_DATA 1

#ifdef XAD_RAR5
#ifdef XAD_RAR4
#define XAD_MOD_NAME "RAR"
#else
#define XAD_MOD_NAME "RAR5"
#endif
#else
#ifdef XAD_RAR4
#define XAD_MOD_NAME "RAR"
#else
#error One of XAD_RAR4 or XAD_RAR5 must be defined
#endif
#endif

#include "RAR_rev.h"

#include <libarchive/archive.h>

struct callbackuserdata {
#ifdef STREAMED_DATA
	struct xadArchiveInfo *ai;
	struct xadMasterIFace *IxadMaster;
#endif
	char *inbuffer;
};

struct xadrarprivate {
	int idx;
	struct archive *a;
	struct callbackuserdata *cbdata;
};

#endif
