/* Header file for XAD RAR client
 * Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XADMASTER_RAR_H
#define XADMASTER_RAR_H

#include "RAR_rev.h"

#include <libarchive/archive.h>

struct xadrarprivate {
	struct archive *a;
};

struct callbackuserdata {
	struct xadArchiveInfo *ai;
	struct xadMasterIFace *IxadMaster;
	char *inbuffer;
};

#endif
