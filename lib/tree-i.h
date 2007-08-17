#ifndef MARK_9bd92b39_c568_4256_867b_2febdcaed2d2
#define MARK_9bd92b39_c568_4256_867b_2febdcaed2d2
#include "tree.h"

#define BLOCK_SIZE 1024

struct Block_s {
    struct Block_s       *next;
    uint64_t              start;
    int                   used;
    struct AffNode_s      node[BLOCK_SIZE];
};

struct AffTree_s {
    struct AffTreap_s    *treap;
    uint64_t              size;
    uint64_t              file_size;
    struct AffNode_s      root;
    struct Block_s        block;
    struct Block_s       *last_block;    
};

void aff_tree_iblock(struct Block_s *block, int start);

#endif /* !defined(MARK_9bd92b39_c568_4256_867b_2febdcaed2d2) */
