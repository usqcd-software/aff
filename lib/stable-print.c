#include <stdint.h>
#include <stdio.h>
#include "stable-i.h"

static void
print_string(const struct AffSymbol_s *sym, void *arg)
{
    printf(" %08x: %s\n", aff_sym_id(sym), aff_sym_name(sym));
}

void
aff_st_print(struct AffSTable_s *st)
{
    if (st == 0) {
	printf("NULL String table\n");
	return;
    }
    printf("String table at %p, size=%d, file-size=%lld:\n",
	   st, st->size, st->file_size);
    aff_st_foreach(st, print_string, 0);
    printf("String table end\n");
}
