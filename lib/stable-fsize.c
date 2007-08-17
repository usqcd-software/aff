#include <stdint.h>
#include "stable-i.h"

uint64_t
aff_stable_file_size(const struct AffSTable_s *st)
{
    if (st == 0)
	return 0;
    return st->file_size;
}
