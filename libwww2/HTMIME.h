/*  */

/*              MIME Parser                     HTMIME.h
**              -----------
**
**   The MIME parser stream presents a MIME document.
**
**
*/

#ifndef HTMIME_H
#define HTMIME_H

#include "HTStream.h"
#include "HTAnchor.h"

extern HTStream * HTMIMEConvert PARAMS((HTPresentation * pres,
                                        HTParentAnchor * anchor,
                                        HTStream *              sink,
                                        HTFormat                format_in,
                                        int                     compressed));

#endif
