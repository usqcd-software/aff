#include <stdint.h>
#include <stdlib.h>
#include "treap.h"
#include "stable-i.h"

struct AffSTable_s *
aff_stable_init(void)
{
    struct AffSTable_s *st = malloc(sizeof (struct AffSTable_s));

    if (st == 0)
	return 0;

    st->treap = aff_treap_init();
    if (st->treap == 0) {
	free(st);
	return 0;
    }
    st->size = 0;
    st->file_size = 0;
    st->last_block = &st->block;
    aff_stable_iblock(&st->block, 0);

    return st;
}
