#ifndef MARK_9bd92b39_c568_4256_867b_2febdcaed2d2
#define MARK_9bd92b39_c568_4256_867b_2febdcaed2d2
#include "tree.h"

#define BLOCK_SIZE 1024

struct Key_s {
    const struct AffTreeNode_s  *parent;
    const struct AffSymbol_s    *name;
};

struct AffTreeNode_s {
    enum AffNodeType_e    type;
    struct Key_s          key;
    uint64_t              id;
    uint32_t              size;
    uint64_t              offset;
    struct AffTreeNode_s *next;
    struct AffTreeNode_s *children;
};

struct Block_s {
    struct Block_s       *next;
    uint64_t              start;
    int                   used;
    struct AffTreeNode_s  node[BLOCK_SIZE];
};

struct AffTree_s {
    struct AffTreap_s    *treap;
    uint64_t              size;
    uint64_t              file_size;
    struct AffTreeNode_s  root;
    struct Block_s        block;
    struct Block_s       *last_block;    
};

void aff_tt_iblock(struct Block_s *block, int start);

#endif /* !defined(MARK_9bd92b39_c568_4256_867b_2febdcaed2d2) */
