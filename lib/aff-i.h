#ifndef MARK_6f9650e1_45e8_4695_81a9_8c3672baee90
#define MARK_6f9650e1_45e8_4695_81a9_8c3672baee90
#include "aff.h"

enum {
    AFF_HEADER_SIZE = 39 /* the full header size */
};

#define AFF_SIG       "LHPC AFF version 1.0"

struct Section_s {
    uint64_t         start;
    uint64_t         size;
    struct AffMD5_s  md5;
};

struct AffWriter_s {
    struct AffMD5_s      header_md5;
    int                  header_size;
    const char          *error;

    FILE                *file;
    uint64_t             position;

    struct AffSTable_s  *stable;
    struct AffTree_s    *tree;

    struct Section_s     stable_hdr;
    struct Section_s     tree_hdr;
    struct Section_s     data_hdr;
};

#endif /* !defined(MARK_6f9650e1_45e8_4695_81a9_8c3672baee90) */
