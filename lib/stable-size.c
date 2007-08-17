#include <stdint.h>
#include "stable-i.h"

uint32_t
aff_stable_size(const struct AffSTable_s *st)
{
    if (st == 0)
	return 0;
    return st->size;
}
