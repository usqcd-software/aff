#ifndef MARK_47ef6581_add5_4c96_8d24_9b3f64cb5baa
#define MARK_47ef6581_add5_4c96_8d24_9b3f64cb5baa
#include "node.h"

struct Key_s {
    const struct AffNode_s      *parent;
    const struct AffSymbol_s    *name;
};

struct AffNode_s {
    enum AffNodeType_e    type;
    struct Key_s          key;
    uint64_t              id;
    uint32_t              size;
    uint64_t              offset;
    struct AffNode_s     *next;
    struct AffNode_s     *children;
};

#endif /* !defined(MARK_47ef6581_add5_4c96_8d24_9b3f64cb5baa) */
