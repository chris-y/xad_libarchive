/* Header file for XAD RAR client
 * Chris Young <chris@unsatisfactorysoftware.co.uk>
 */

#ifndef XAD_LIBARCHIVE_RAR_H
#define XAD_LIBARCHIVE_RAR_H 1

/* define these to support RAR4 or RAR5 or both
 * RAR4 is also known as RAR3 and seems to have issues
 * RAR5 needs a newer than 3.3.3 libarchive
 */
#define XAD_RAR4 1 /* we used to call this RAR3 */
//#define XAD_RAR5 1

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

#endif
