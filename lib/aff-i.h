#ifndef MARK_6f9650e1_45e8_4695_81a9_8c3672baee90
#define MARK_6f9650e1_45e8_4695_81a9_8c3672baee90
#include "aff.h"

enum {
    AFF_HEADER_SIZE = 144 /* the full header size */
};

#define AFF_SIG       "LHPC AFF version 1.0"

struct WSection_s {
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

    struct WSection_s    stable_hdr;
    struct WSection_s    tree_hdr;
    struct WSection_s    data_hdr;
};

struct RSection_s {
    uint64_t         start;
    uint64_t         size;
    uint8_t          md5[16];
};

struct AffReader_s {
    int                  header_size;
    int                  fatal_error_p;
    const char          *error;

    FILE                *file;
    uint64_t             position;

    struct AffSTable_s  *stable;
    struct AffTree_s    *tree;

    struct RSection_s    stable_hdr;
    struct RSection_s    tree_hdr;
    struct RSection_s    data_hdr;
};

extern uint8_t aff_signature[];

#endif /* !defined(MARK_6f9650e1_45e8_4695_81a9_8c3672baee90) */
