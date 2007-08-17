#include <stdint.h>
#include "tree-i.h"

uint32_t
aff_n_name(const struct AffTreeNode_s *tn)
{
    if (tn == 0)
	return 0;
    return tn->key.nId;
}
