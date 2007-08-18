#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "node.h"
#include "md5.h"
#include "aff-i.h"

int
aff_node_put_char(struct AffWriter_s *aff,
		  struct AffNode_s *n,
		  const char *d,
		  uint32_t s)
{
    if (aff == 0 || n == 0 || d == 0 || aff->error != 0)
	return 1;
    if (aff_node_type(n) != affNodeVoid) {
	aff->error = "Changing the type of the node";
	return 1;
    }
    aff_node_assign(n, affNodeChar, s, aff->position);
    if (fwrite(d, s, 1, aff->file) != 1) {
	aff->error = strerror(errno);
	return 1;
    }
    aff_md5_update(&aff->data_hdr.md5, (const uint8_t *)d, s);
    aff->data_hdr.size += s;
    aff->position += s;

    return 0;
}
