#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stable-i.h"
#include "treap.h"

uint32_t
aff_st_insert(struct AffSTable_s *st, const char *name)
{
    struct Block_s *b;
    struct Symbol_s *sym;
    int len;

    if (st == 0 || name == 0)
	return 0;

    len = strlen(name) + 1;
    sym = aff_h_lookup(st->treap, name, len);
    if (sym)
	return sym->id;

    if (st->last_block->used == BLOCK_SIZE) {
	b = malloc(sizeof (struct Block_s));
	if (b == 0)
	    return 0;
	aff_st_iblock(b, st->size + 1);
	st->last_block->next = b;
	st->last_block = b;
    }
    b = st->last_block;
    sym = &b->symbol[b->used];
    sym->name = strdup(name);
    if (sym->name == 0)
	return 0;
    sym->id = st->size + 1;
    if (aff_h_extend(st->treap, sym->name, len, sym) != 0) {
	free((void *)sym->name);
	return 0;
    }
    b->used++;
    st->size = sym->id;
    st->file_size += len;
    return sym->id;
}
