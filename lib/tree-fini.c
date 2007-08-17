#include <stdint.h>
#include <stdlib.h>
#include "tree-i.h"
#include "treap.h"

void *
aff_tt_fini(struct AffTree_s *tt)
{
    struct Block_s *bl;

    if (tt == 0)
	return 0;
    aff_h_fini(tt->treap);
    for (bl = tt->block.next; bl;) {
	struct Block_s *n = bl->next;
	free(bl);
	bl = n;
    }
    free(tt);

    return 0;
}
