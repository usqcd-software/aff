#include <stdint.h>
#include <stdarg.h>
#include "aff.h"

/* This file should be modified with each release to make SVN change the Id
 *
 * 2007/08/21 avp -- working version, playing with format
 * 2007/09/11 avp -- final cut for v 1.0.0
 * 2007/11/28 avp -- preliminary v 2.0.0 
 * 2007/11/29 avp -- final rc1 for v 2.0.0
 * 2007/11/29 avp -- final rc2 for v 2.0.0
 */
const char *
aff_version(void)
{
    return "LHPC AFF v.2.0.0 $Revision$ $Date$"
#ifdef AFF_DEBUG
     "  DEBUG VERSION"
#endif
     ;
}
