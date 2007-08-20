#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "node.h"
#include "md5.h"
#include "io.h"
#include "coding.h"
#include "aff-i.h"

int
aff_node_get_int(struct AffReader_s *aff,
		 struct AffNode_s *n,
		 uint32_t *d,
		 uint32_t s)
{
    uint8_t buf[4];
    uint32_t i;
    uint32_t size;
    uint64_t offset;

    if (aff == 0)
	return 1;
    if (aff->error != 0)
	return 1;
    if (aff->file == 0 || n == 0 || d == 0 || s == 0) {
	aff->error = "Unreasonable parameters to aff_node_get_int()";
	return 1;
    }
    if (aff_node_type(n) != affNodeInt) {
	aff->error = "Reading a int32_t[] from wrong node";
	return 1;
    }
    size = aff_node_size(n);
    offset = aff_node_offset(n);
    if (aff_file_setpos(aff->file, offset) != 0) {
	aff->error = "AFF positioning error";
	return 1;
    }
    if (size > s)
	size = s;
    for (i = 0; i < size; i++) {
	if (fread(buf, sizeof (buf), 1, aff->file) != 1) {
	    aff->error = strerror(errno);
	    return 1;
	}
	if (aff_decode_u32(&d[i], buf, sizeof(buf)) == 0) {
	    aff->error = "uint32_t decoding error";
	    return 1;
	}
    }
    return 0;
}
