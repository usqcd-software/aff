#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "md5.h"
#include "stable.h"
#include "tree.h"
#include "aff-i.h"

struct AffWriter_s *
aff_writer(const char *file_name)
{
    uint8_t dummy_header[AFF_HEADER_SIZE];
    struct AffWriter_s *w = malloc(sizeof (struct AffWriter_s));

    if (w == 0)
	return 0;

    memset(w, 0, sizeof (struct AffWriter_s));
    w->stable = aff_stable_init();
    if (w->stable == 0)
	goto no_stable;
    w->tree = aff_tree_init(w->stable);
    if (w->tree == 0)
	goto no_tree;

    remove(file_name);
    w->file = fopen(file_name, "wb");
    if (w->file == 0)
	goto no_file;
    
    memset(dummy_header, 0, AFF_HEADER_SIZE);
    if (fwrite(dummy_header, AFF_HEADER_SIZE, 1, w->file) != 1)
	goto no_hdr;
    
    if (FLT_RADIX != 2 || sizeof(double) > sizeof (uint64_t))
	goto bad_radix;

    w->position = AFF_HEADER_SIZE;
    aff_md5_init(&w->data_hdr.md5);
    w->data_hdr.size = 0;
    w->data_hdr.start = AFF_HEADER_SIZE;

    return w;
bad_radix:
no_hdr:
    fclose(w->file);
    remove(file_name);
no_file:
    free(w->tree);
no_tree:
    free(w->stable);
no_stable:
    free(w);
    return 0;
}
