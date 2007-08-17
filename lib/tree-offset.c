#include <stdint.h>
#include "tree-i.h"

uint64_t
aff_n_offset(const struct AffTreeNode_s *tn)
{
    if (tn == 0)
	return 0;
    return tn->offset;
}
