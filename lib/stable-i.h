#ifndef MARK_ccee59e4_4306_46b9_a6f0_df315a0233fe
#define MARK_ccee59e4_4306_46b9_a6f0_df315a0233fe
#include "stable.h"

#define BLOCK_SIZE 3 /* 1024 */

struct AffSymbol_s {
    const char *name;
    uint32_t    id;
};

struct Block_s {
    struct Block_s     *next;
    uint32_t            start;
    uint32_t            used;
    struct AffSymbol_s  symbol[BLOCK_SIZE];
};

struct AffSTable_s {
    struct AffTreap_s *treap;
    uint32_t           size;
    struct Block_s     block;
    struct Block_s    *last_block;
};

void aff_stable_iblock(struct Block_s *block, int start);


#endif /* !defined(MARK_ccee59e4_4306_46b9_a6f0_df315a0233fe) */

