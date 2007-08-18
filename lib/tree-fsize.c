#include <stdint.h>
#include <stdarg.h>
#include "treap.h"
#include "stable.h"
#include "node-i.h"
#include "tree-i.h"

uint64_t
aff_tree_file_size(const struct AffTree_s *tt)
{
    if (tt == 0)
	return 0;
    return tt->file_size;
}
