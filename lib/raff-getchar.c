#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "node.h"
#include "md5.h"
#include "io.h"
#include "aff-i.h"

int
aff_node_get_char(struct AffReader_s *aff,
		  struct AffNode_s *n,
		  char *d,
		  uint32_t s)
{
    uint32_t size;
    uint64_t offset;

    if (aff == 0)
	return 1;
    if (aff->error != 0)
	return 1;
    if (aff->file == 0 || n == 0 || d == 0 || s == 0) {
	aff->error = "Unreasonable parameters to aff_node_get_char()";
	return 1;
    }
    if (aff_node_type(n) != affNodeChar) {
	aff->error = "Reading a char[] from wrong node";
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
    if (fread(d, size, 1, aff->file) != 1) {
	aff->error = strerror(errno);
	return 1;
    }
    return 0;
}
