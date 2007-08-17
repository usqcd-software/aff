#include <stdint.h>
#include "stable-i.h"

uint32_t
aff_sym_id(const struct AffSymbol_s *sym)
{
    if (sym == 0)
	return 0;
    return sym->id;
}
