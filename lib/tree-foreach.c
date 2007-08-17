#include <stdint.h>
#include "tree-i.h"

void
aff_tt_foreach(const struct AffTree_s *tt,
               void (*proc)(const struct AffTreeNode_s *node, void *arg),
	       void *arg)
{
    const struct Block_s *b;
    uint32_t i;
    
    if (tt == 0)
	return;

    for (b = &tt->block; b; b = b->next) {
	for (i = 0; i < b->used; i++) {
	    proc(b->node, arg);
	}
    }
}
