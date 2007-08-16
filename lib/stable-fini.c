#include <stdint.h>
#include <stdlib.h>
#include "stable-i.h"
#include "treap.h"

void *
aff_st_fini(struct AffSTable_s *st)
{
    struct Block_s *bl;

    if (st == 0)
	return 0;
    aff_h_fini(st->treap);
    for (bl = st->block.next; bl;) {
	struct Block_s *n = bl->next;
	free(bl);
	bl = n;
    }
    free(st);

    return 0;
}
