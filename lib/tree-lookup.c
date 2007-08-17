#include <stdint.h>
#include <string.h>
#include "treap.h"
#include "stable.h"
#include "tree-i.h"

struct AffTreeNode_s *
aff_tt_lookup(const struct AffTree_s      *tt,
	      const struct AffTreeNode_s  *parent,
	      const struct AffSymbol_s    *name)
{
    struct Key_s k;

    if (tt == 0 || parent == 0 || name == 0)
	return 0;

    k.parent = parent;
    k.name = name;

    return aff_h_lookup(tt->treap, &k, sizeof (struct Key_s));
}
