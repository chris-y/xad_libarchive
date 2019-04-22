/* Header file for XAD RAR client
 * Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_RAR_H
#define XADMASTER_RAR_H

/* set to stream data rather than reading all in memory */
//#define STREAMED_DATA 1

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
	struct archive *a;
	struct callbackuserdata *cbdata;
};

#endif
