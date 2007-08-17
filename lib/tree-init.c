#include <stdint.h>
#include <stdlib.h>
#include "treap.h"
#include "stable.h"
#include "tree-i.h"

struct AffTree_s *
aff_tt_init(void)
{
    struct AffTree_s *tt = malloc(sizeof (struct AffTree_s));

    if (tt == 0)
	return 0;

    tt->treap = aff_h_init();
    if (tt->treap == 0) {
	free(tt);
	return 0;
    }
    tt->size = 0;
    tt->file_size = 0;
    tt->last_block = &tt->block;
    tt->root.type = affNodeVoid;
    tt->root.key.parent = 0;
    tt->root.key.name = 0;
    tt->root.id = 0;
    tt->root.size = 0;
    tt->root.offset = 0;
    tt->root.next = 0;
    tt->root.children = 0;
    aff_tt_iblock(&tt->block, 1);

    return tt;
}
