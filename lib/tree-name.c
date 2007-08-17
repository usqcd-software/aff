#include <stdint.h>
#include "treap.h"
#include "stable.h"
#include "tree-i.h"

const struct AffSymbol_s *
aff_n_name(const struct AffTreeNode_s *tn)
{
    if (tn == 0)
	return 0;
    return tn->key.name;
}
