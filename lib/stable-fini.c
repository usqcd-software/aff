#include <stdint.h>
#include <stdlib.h>
#include "stable-i.h"
#include "treap.h"

void *
aff_stable_fini(struct AffSTable_s *st)
{
    struct Block_s *bl;
    int i;

    if (st == 0)
	return 0;
    aff_treap_fini(st->treap);
    for (i = 0; i < st->block.used; i++)
	free(st->block.symbol[i].name);
    for (bl = st->block.next; bl;) {
	struct Block_s *n = bl->next;
	for (i = 0; i < bl->used; i++)
	    free(bl->symbol[i].name);
	free(bl);
	bl = n;
    }
    free(st);

    return 0;
}
