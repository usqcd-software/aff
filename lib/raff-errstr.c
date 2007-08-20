#include <stdint.h>
#include <stdio.h>
#include "md5.h"
#include "aff-i.h"

const char *
aff_reader_errstr(struct AffReader_s *aff)
{
    const char *msg;
    if (aff == 0)
	return "AffReader is not allocated";

    msg = aff->error;
    if (!aff->fatal_error_p) {
	aff->error = 0;
    }
    return msg;
}
