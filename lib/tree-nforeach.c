#include <stdint.h>
#include "tree-i.h"

void
aff_n_foreach(const struct AffTreeNode_s *node,
	      void (*proc)(const struct AffTreeNode_s *node, void *arg),
	      void *arg)
{
    const struct AffTreeNode_s *ch;
    
    if (node == 0)
	return;

    for (ch = node->children; ch; ch = ch->next) {
	proc(ch, arg);
    }
}
