#include <stdint.h>
#include "tree-i.h"

uint64_t
aff_tt_file_size(const struct AffTree_s *tt)
{
    if (tt == 0)
	return 0;
    return tt->file_size;
}
