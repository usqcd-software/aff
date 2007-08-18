#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "treap.h"
#include "stable.h"
#include "node-i.h"
#include "tree-i.h"

struct AffNode_s *
aff_tree_index(const struct AffTree_s *tt, uint64_t index)
{
    const struct Block_s *b;

    if (tt == 0)
	return 0;
    for (b = &tt->block; b; b = b->next) {
	if (index < b->start)
	    return 0;
	if (index < b->start + b->used)
	    return (struct AffNode_s *)&b->node[b->start - index];
    }
    return 0;
}
