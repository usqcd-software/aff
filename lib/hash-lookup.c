#include <stdint.h>
#include <string.h>
#include "hash-i.h"

void *
aff_h_lookup(const struct AffHash_s *h, const void *key, int ksize)
{
    struct Node_s *n;

    if (h == 0)
	return 0;

    for (n = h->root; n;) {
	int cmp = memcmp(key, n->key, ksize);

	if (cmp == 0) {
	    return n->value;
	} else if (cmp < 0) {
	    n = n->left;
	} else {
	    n = n->right;
	}
    }
    return 0;
}
