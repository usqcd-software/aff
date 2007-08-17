#include <stdint.h>
#include "tree-i.h"

const struct AffTreeNode_s *
aff_tt_root(const struct AffTree_s *tt)
{
    if (tt == 0)
	return 0;
    return &tt->root;
}
