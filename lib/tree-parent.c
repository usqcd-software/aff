#include <stdint.h>
#include "treap.h"
#include "stable.h"
#include "tree-i.h"

const struct AffTreeNode_s *
aff_n_parent(const struct AffTreeNode_s *tn)
{
    if (tn == 0)
	return 0;
    return tn->key.parent;
}
