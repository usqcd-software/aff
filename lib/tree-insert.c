#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "treap.h"
#include "stable.h"
#include "tree-i.h"

struct AffTreeNode_s *
aff_tt_insert(struct AffTree_s            *tt,
	      const struct AffTreeNode_s  *parent,
	      const struct AffSymbol_s    *name)
{
    struct Block_s *b;
    struct AffTreeNode_s *n;
    struct Key_s k;
    int len = 0;

    if (tt == 0 || parent == 0 || name == 0)
	return 0;

    k.parent = parent;
    k.name = name;
    n = aff_h_lookup(tt->treap, &k, sizeof (struct Key_s));
    if (n)
	return 0;

    if (tt->last_block->used == BLOCK_SIZE) {
	b = malloc(sizeof (struct Block_s));
	if (b == 0)
	    return 0;
	aff_tt_iblock(b, tt->size + 1);
	tt->last_block->next = b;
	tt->last_block = b;
    }
    b = tt->last_block;
    n = &b->node[b->used];
    n->key = k;
    n->type = affNodeVoid;
    n->size = 0;
    n->offset = 0;
    n->id = tt->size + 1;
    n->next = parent->children;
    ((struct AffTreeNode_s *)parent)->children = n;
    if (aff_h_extend(tt->treap, &n->key, sizeof (struct Key_s), n) != 0) {
	return 0;
    }
    b->used++;
    tt->size = n->id;
    tt->file_size += len
	+ sizeof (uint8_t)  /* type */
	+ sizeof (uint32_t) /* nameId */
	+ sizeof (uint64_t); /* parentId */

    return n;
}
