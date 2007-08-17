#include <stdint.h>
#include "tree-i.h"

struct AffTreeNode_s *
aff_tt_root(const struct AffTree_s *tt)
{
    if (tt == 0)
	return 0;
    return (struct AffTreeNode_s *)&tt->root;
}
