#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include "node.h"
#include "stable.h"
#include "tree.h"
#include "md5.h"
#include "coding.h"
#include "io.h"
#include "aff-i.h"

#define STRING_BUF0  1024
#define STRING_BUFx  1024

static void
unpack(struct AffReader_s *aff, struct RSection_s *section,
       uint8_t buf[AFF_HEADER_SIZE], int off,
       const char *error_msg)
{
    uint32_t size = AFF_HEADER_SIZE - off;
    uint8_t *start = buf + off;
    uint8_t *ptr;
    
    if (aff->error)
	return;

    ptr = aff_decode_u64(&section->start, start, size);
    if (ptr == 0)
	goto error;
    ptr = aff_decode_u64(&section->size, ptr, size - (ptr - start));
    if (ptr == 0)
	goto error;
    if (AFF_HEADER_SIZE - (buf - ptr) < 16)
	goto error;
    memcpy(&section->md5, ptr, 16);

    return;
error:
    aff->error = error_msg;
}

struct AffReader_s *
aff_reader(const char *file_name)
{
    struct AffReader_s *aff = malloc(sizeof (struct AffReader_s));
    uint8_t buf[AFF_HEADER_SIZE];
    struct AffMD5_s md5;
    uint8_t md5_read[16];
    uint32_t data;
    int size;
    uint8_t *sb;
    int sb_size;
    int sb_used;
    uint64_t string_size;
    uint64_t tree_size;
    
    if (aff == 0)
	return 0;

    memset(aff, 0, sizeof (struct AffReader_s));

    aff->file = fopen(file_name, "rb");
    if (aff->file == 0) {
	aff->error = strerror(errno);
	return aff;
    }
    aff->stable = aff_stable_init();
    if (aff->stable == 0) {
	aff->error = "Not enough memory for stable";
	goto error;
    }
    aff->tree = aff_tree_init(aff->stable);
    if (aff->tree == 0) {
	aff->error = "Not enough memory for tree";
	goto error;
    }
    if (fread(buf, sizeof (buf), 1, aff->file) != 1) {
	aff->error = "Reading AFF header failed";
	goto error;
    }
    aff_md5_init(&md5);
    aff_md5_update(&md5, buf, sizeof (buf) - 16);
    aff_md5_final(md5_read, &md5);
    if (memcmp(md5_read, buf + sizeof (buf) - 16, 16) != 0) {
	aff->error = "AFF header failed checksum";
	goto error;
    }

    /* NB: This should match writer_fini() */
    buf[sizeof(buf) - 1] = 0; /* Paranoia? I don't think so */
    if (strcmp((const char *)buf, (const char *)aff_signature) != 0) {
	aff->error = "AFF signature mismatch";
	goto error;
    }
    size = strlen((const char *)aff_signature) + 1;
    if (buf[size] != sizeof (double) * CHAR_BIT) {
	aff->error = "AFF size if double mismatch";
	goto error;
    }
    if (buf[size+1] != FLT_RADIX || FLT_RADIX != 2) {
	aff->error = "AFF double radix mismatch";
	goto error;
    }
    if (buf[size+2] != DBL_MANT_DIG) {
	aff->error = "AFF double mantissa size mismatch";
	goto error;
    }
    if (aff_decode_u32(&data, buf + size + 3, 4) == 0) {
	aff->error = "AFF error decoding double exponent sizes";
	goto error;
    }
    if ((data >> 16 != DBL_MAX_EXP) || ((data & 0xffff) != (- DBL_MIN_EXP))) {
	aff->error = "AFF exponent limits mismatch";
	goto error;
    }
    if (aff_decode_u32(&data, buf + size + 7, 4) == 0 ||
	data != AFF_HEADER_SIZE) {
	aff->error = "AFF header size check failed";
	goto error;
    }
    unpack(aff, &aff->data_hdr,   buf, size+11, "data header unpack failed");
    unpack(aff, &aff->stable_hdr, buf, size+43, "stable header unpack failed");
    unpack(aff, &aff->tree_hdr,   buf, size+75, "tree header unpack failed");
    if (aff->error)
	goto error;

    if (aff_file_setpos(aff->file, aff->stable_hdr.start) != 0) {
	aff->error = "Positioning on the string table failed";
	goto error;
    }
    sb_size = STRING_BUF0;
    sb_used = 0;
    sb = malloc(STRING_BUF0);
    if (sb == 0) {
	aff->error = "Not enough memory for the initial string buffer";
	goto error;
    }
    aff_md5_init(&md5);
    for (string_size = aff->stable_hdr.size; string_size--;) {
	uint8_t ch;

	if (sb_used == sb_size) {
	    uint8_t *b = realloc(sb, sb_size += STRING_BUFx);
	    if (b == 0) {
		free(sb);
		aff->error = "Not enough memory to grow string buffer";
		goto error;
	    }
	    sb = b;
	}
	if (fread(&ch, 1, 1, aff->file) != 1) {
	    aff->error = "Error reading string table";
	    free(sb);
	    goto error;
	}
	aff_md5_update(&md5, &ch, 1);
	sb[sb_used++] = ch;
	if (ch == 0) {
	    if (aff_stable_insert(aff->stable, (const char *)sb) == 0) {
		free(sb);
		aff->error = "String table construction error";
		goto error;
	    }
	    sb_used = 0;
	}
    }
    free(sb);
    if (sb_used != 0) {
	aff->error = "Illformed string table";
	goto error;
    }
    aff_md5_final(md5_read, &md5);
    if (memcmp(md5_read, aff->stable_hdr.md5, 16) != 0) {
	aff->error = "String table checksum mismatch";
	goto error;
    }

    aff_md5_init(&md5);
    for (tree_size = aff->tree_hdr.size; tree_size >= 1 + 8 + 4; ) {
	const struct AffSymbol_s *n_name;
	struct AffNode_s *n_parent;
	struct AffNode_s *node;
	enum AffNodeType_e n_type;
	uint8_t tnode[1 + 8 + 4 + 4 + 8];
	uint64_t f_parent;
	uint32_t f_name;
	uint64_t f_offset;
	uint32_t f_size;

	if (fread(tnode, 1 + 8 + 4, 1, aff->file) != 1) {
	    aff->error = "Tree node reading error";
	    goto error;
	}
	aff_md5_update(&md5, tnode, 1 + 8 + 4);
	tree_size -= 1 + 8 + 4;
	if (aff_decode_type(&n_type, tnode, 1) == 0 ||
	    aff_decode_u64(&f_parent, tnode + 1, 8) == 0 ||
	    aff_decode_u32(&f_name, tnode + 9, 4) == 0) {
	    aff->error = "Error decoding the node record";
	    goto error;
	}
	n_parent = aff_tree_index(aff->tree, f_parent);
	if (n_parent == 0) {
	    aff->error = "Broken tree: missing parent";
	    goto error;
	}
	n_name = aff_stable_index(aff->stable, f_name);
	if (n_name == 0) {
	    aff->error = "Broken tree: missing name";
	    goto error;
	}
	if (n_type != affNodeVoid) {
	    if (fread(tnode, 4 + 8, 1, aff->file) != 1) {
		aff->error = "Tree node data reading error";
		goto error;
	    }
	    aff_md5_update(&md5, tnode, 4 + 8);
	    tree_size -= 4 + 8;
	    if (aff_decode_u32(&f_size, tnode, 4) == 0 ||
		aff_decode_u64(&f_offset, tnode + 4, 8) == 0) {
		aff->error = "Tree node data decoding error";
		goto error;
	    }
	} else {
	    f_size = 0;
	    f_offset = 0;
	}
	node = aff_node_chdir(aff->tree, aff->stable,
			      n_parent, 1,
			      aff_symbol_name(n_name));
	if (node == 0) {
	    aff->error = "Node rebuilding error";
	    goto error;
	}
	if (aff_node_assign(node, n_type, f_size, f_offset) != 0) {
	    aff->error = "Node asignment error";
	    goto error;
	}
    }
    if (tree_size != 0) {
	aff->error = "Mismatch in the tree size";
	goto error;
    }
    aff_md5_final(md5_read, &md5);
    if (memcmp(md5_read, aff->tree_hdr.md5, 16) != 0) {
	aff->error = "Tree table checksum mismatch";
	goto error;
    }

    return aff;
error:
    fclose(aff->file);
    aff->file = 0;
    return aff;
}
