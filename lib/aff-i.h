#ifndef MARK_6f9650e1_45e8_4695_81a9_8c3672baee90
#define MARK_6f9650e1_45e8_4695_81a9_8c3672baee90
#include "aff.h"

struct AffWriter_t {
    FILE                *file;
    struct AffSTable_s  *stable;
    struct AffTree_s    *tree;
    const char          *error;
    off_t                position;
    off_t                data_start;
    off_t                data_size;
    struct AffMD5_s      data_md5;
};

#endif /* !defined(MARK_6f9650e1_45e8_4695_81a9_8c3672baee90) */
