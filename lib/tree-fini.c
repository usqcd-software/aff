#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stable.h"
#include "node-i.h"
#include "tree-i.h"

void *
aff_tree_fini(struct AffTree_s *tt)
{
    struct Block_s *bl;

    if (tt == 0)
	return 0;
    for (bl = tt->block.next; bl;) {
	struct Block_s *n = bl->next;
	free(bl);
	bl = n;
    }
    free(tt);

    return 0;
}
