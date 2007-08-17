#include <stdint.h>
#include <string.h>
#include "tree-i.h"
#include "treap.h"

struct AffTreeNode_s *
aff_tt_lookup(const struct AffTree_s *tt,
	      const struct AffTreeNode_s *parent,
	      uint32_t name)
{
    struct Key_s k;

    if (tt == 0 || parent == 0 || name == 0)
	return 0;

    k.pId = parent->id;
    k.nId = name;

    return aff_h_lookup(tt->treap, &k, sizeof (struct Key_s));
}
