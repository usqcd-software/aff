#include <stdint.h>
#include <stdlib.h>
#include "treap-i.h"

static void
treap_free(struct Node_s *n)
{
    if (n) {
	treap_free(n->left);
	treap_free(n->right);
	free(n);
    }
}

void *
aff_treap_fini(struct AffTreap_s *h)
{
    if (h) {
	treap_free(h->root);
	free(h);
    }

    return NULL;
}
