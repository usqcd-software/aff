#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <errno.h>
#include "md5.h"
#include "stable.h"
#include "tree.h"
#include "aff-i.h"

struct AffWriter_s *
aff_writer(const char *file_name)
{
    uint8_t dummy_header[AFF_HEADER_SIZE];
    struct AffWriter_s *aff = malloc(sizeof (struct AffWriter_s));

    if (aff == 0)
	return 0;

    memset(aff, 0, sizeof (struct AffWriter_s));
    remove(file_name);
    aff->file = fopen(file_name, "wb");
    if (aff->file == 0) {
	aff->error = strerror(errno);
	return aff;
    }

    aff->stable = aff_stable_init();
    if (aff->stable == 0) {
	aff->error = "Not enough memory for stable in aff_writer()";
	goto error;
    }

    aff->tree = aff_tree_init(aff->stable);
    if (aff->tree == 0) {
	aff->error = "Not enough memory for tree in aff_writer()";
	goto error;
    }

    memset(dummy_header, 0, AFF_HEADER_SIZE);
    if (fwrite(dummy_header, AFF_HEADER_SIZE, 1, aff->file) != 1) {
	aff->error = "Error writing dummy header in aff_writer()";
	goto error;
    }
    
    if (FLT_RADIX != 2 || sizeof(double) > sizeof (uint64_t)) {
	aff->error = "Unsupported double format in aff_writer()";
	goto error;
    }

    aff->position = AFF_HEADER_SIZE;
    aff_md5_init(&aff->data_hdr.md5);
    aff->data_hdr.size = 0;
    aff->data_hdr.start = AFF_HEADER_SIZE;

    return aff;

error:
    if (aff->file)
	fclose(aff->file);
    aff->file = 0;
    if (aff->tree)
	aff_tree_fini(aff->tree);
    aff->tree = 0;
    if (aff->stable)
	aff_stable_fini(aff->stable);
    aff->stable = 0;

    return aff;
}