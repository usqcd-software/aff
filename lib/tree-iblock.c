#include <stdint.h>
#include "tree-i.h"

void
aff_tt_iblock(struct Block_s *block, int start)
{
    block->next = 0;
    block->start = start;
    block->used = 0;
}
