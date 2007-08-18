#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "treap.h"
#include "stable.h"
#include "node-i.h"
#include "tree-i.h"

struct AffNode_s *
aff_tree_lookup(const struct AffTree_s      *tt,
		const struct AffNode_s      *parent,
		const struct AffSymbol_s    *name)
{
    struct Key_s k;

    if (tt == 0 || parent == 0 || name == 0)
	return 0;

    k.parent = parent;
    k.name = name;

    return aff_treap_lookup(tt->treap, &k, sizeof (struct Key_s));
}
