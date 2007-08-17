#include <stdint.h>
#include <string.h>
#include "treap.h"
#include "stable-i.h"

const struct AffSymbol_s *
aff_stable_lookup(const struct AffSTable_s *st, const char *name)
{
    if (st == 0 || name == 0)
	return 0;

    return aff_treap_lookup(st->treap, name, strlen(name) + 1);
}
