#include <stdint.h>
#include "stable-i.h"

void
aff_stable_iblock(struct Block_s *block, int start)
{
    block->next = 0;
    block->start = start;
    block->used = 0;
}
