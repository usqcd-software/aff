#include <stdint.h>
#include <stdlib.h>
#include "hash-i.h"

struct AffHash_s *
aff_h_init(void)
{
    struct AffHash_s *h = malloc(sizeof (struct AffHash_s));
    if (h == 0)
	return 0;

    h->state = RINIT;
    h->root = 0;

    return h;
}
