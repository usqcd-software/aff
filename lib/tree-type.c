#include <stdint.h>
#include "treap.h"
#include "stable.h"
#include "tree-i.h"


enum AffNodeType_e
aff_n_type(const struct AffTreeNode_s *tn)
{
    if (tn == 0)
	return affNodeInvalid;
    return tn->type;
}
