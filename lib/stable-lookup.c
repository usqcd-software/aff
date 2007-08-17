#include <stdint.h>
#include <string.h>
#include "stable-i.h"
#include "treap.h"

const struct AffSymbol_s *
aff_st_lookup(const struct AffSTable_s *st, const char *name)
{
    if (st == 0 || name == 0)
	return 0;

    return aff_h_lookup(st->treap, name, strlen(name) + 1);
}
