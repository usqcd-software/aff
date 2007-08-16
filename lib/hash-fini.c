#include <stdint.h>
#include <stdlib.h>
#include "hash-i.h"

static void
h_free(struct Node_s *n)
{
    if (n) {
	h_free(n->left);
	h_free(n->right);
	free(n);
    }
}

void *
aff_h_fini(struct AffHash_s *h)
{
    if (h) {
	h_free(h->root);
	free(h);
    }

    return NULL;
}
