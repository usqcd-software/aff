#include <stdint.h>
#include <string.h>
#include "stable-i.h"
#include "treap.h"

uint32_t
aff_st_lookup(const struct AffSTable_s *st, const char *name)
{
    struct Symbol_s *s;

    if (st == 0 || name == 0)
	return 0;

    s = aff_h_lookup(st->treap, name, strlen(name) + 1);
    if (s == 0)
	return 0;
    return
	s->id;
}
