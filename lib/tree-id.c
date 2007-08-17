#include <stdint.h>
#include "treap.h"
#include "stable.h"
#include "tree-i.h"

uint64_t
aff_n_id(const struct AffTreeNode_s *tn)
{
    if (tn == 0)
	return 0;
    return tn->id;
}
