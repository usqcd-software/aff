#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "md5.h"
#include "node.h"
#include "tree.h"
#include "stable.h"
#include "coding.h"
#include "aff-i.h"

static void
out_string(const struct AffSymbol_s *symbol, void *arg)
{
    struct AffWriter_s *aff = arg;

    if (aff->error == 0) {
	const char *name = aff_symbol_name(symbol);
	int len = strlen(name) + 1;

	if (fwrite(name, len, 1, aff->file) != 1) {
	    aff->error = "Symbol writing error";
	    return;
	}
	aff_md5_update(&aff->stable_hdr.md5, (const uint8_t *)name, len);
    }
}

static void
out_node(struct AffNode_s *node, void *arg)
{
    struct AffWriter_s *aff = arg;

    if (aff->error == 0) {
	uint8_t block[1+8+4+4+8];
	uint8_t *buf;
	uint32_t size;
	enum AffNodeType_e type = aff_node_type(node);
	
	size = 0;
	buf = aff_encode_type(&block[0], sizeof (block), type);
	size = sizeof (block) - (buf - &block[0]);
	buf = aff_encode_u64(buf, size, aff_node_id(aff_node_parent(node)));
	size = sizeof (block) - (buf - &block[0]);
	buf = aff_encode_u32(buf, size, aff_symbol_id(aff_node_name(node)));
	size = sizeof (block) - (buf - &block[0]);
	if (type != affNodeVoid) {
	    buf = aff_encode_u32(buf, size, aff_node_size(node));	
	    size = sizeof (block) - (buf - &block[0]);
	    buf = aff_encode_u64(buf, size, aff_node_offset(node));
	    size = sizeof (block) - (buf - &block[0]);
	}
	if (buf == 0) {
	    aff->error = "Node encoding overrun";
	    return;
	}
	size = buf - &block[0];
	if (fwrite(block, size, 1, aff->file) != 1) {
	    aff->error = "Node writing error";
	    return;
	}
	aff_md5_update(&aff->tree_hdr.md5, block, size);
    }
}

static void
pack(struct AffWriter_s *aff, struct Section_s *section, char *error_msg)
{
    uint8_t block[8+8+16];
    uint8_t *buf;
    uint32_t size;

    if (aff->error)
	return;

    size = sizeof (block);
    buf = aff_encode_u64(&block[0], size, section->start);
    size = sizeof (block) - (buf - &block[0]);
    buf = aff_encode_u64(buf, size, section->size);
    size = (buf - &block[0]);
    if (buf == 0 || sizeof (block) - size < 16) {
	aff->error = "Section header encoding overrun";
	return;
    }

    aff_md5_final(buf, &section->md5);
    size += 16;
    if (fwrite(block, size, 1, aff->file) != 1) {
	aff->error = error_msg;
	return;
    }
    aff_md5_update(&aff->header_md5, block, size);
}


const char *
aff_writer_close(struct AffWriter_s *aff)
{
    const char *sig = AFF_SIG;
    uint8_t buffer[16];

    if (aff == 0)
	return "NULL aff passed to aff_writer_close()";

    if (aff->error)
	goto end;

    aff->stable_hdr.start = aff->position;
    aff->stable_hdr.size = 0;
    aff_md5_init(&aff->stable_hdr.md5);
    aff_stable_foreach(aff->stable, out_string, aff);
    if (aff->error)
	goto end;

    aff->tree_hdr.start = aff->position;
    aff->tree_hdr.size = 0;
    aff_md5_init(&aff->tree_hdr.md5);
    aff_tree_foreach(aff->tree, out_node, aff);
    if (aff->error)
	goto end;

    rewind(aff->file);

    aff_md5_init(&aff->header_md5);
    aff->header_size = strlen(sig) + 1;
    if (fwrite(sig, aff->header_size, 1, aff->file) != 1) {
	aff->error = "AFF Signature writing erorr";
	goto end;
    }
    aff_md5_update(&aff->header_md5, (const uint8_t *)sig, aff->header_size);

    buffer[0] = sizeof (double);
    buffer[1] = DBL_MANT_DIG;
    aff->header_size += 2;
    if (fwrite(buffer, 2, 1, aff->file) != 1) {
	aff->error = "AFF size signature writing error";
	goto end;
    }
    aff_md5_update(&aff->header_md5, buffer, 2);

    pack(aff, &aff->stable_hdr, "Stable header writing error");
    pack(aff, &aff->tree_hdr, "Tree header writing error");
    pack(aff, &aff->data_hdr, "Data header writing error");
    if (aff->error)
	goto end;
    
    aff_md5_final(buffer, &aff->header_md5);
    aff->header_size += 16;
    if (fwrite(buffer, 16, 1, aff->file) != 1) {
	aff->error = "AFF checksum writing error";
	goto end;
    }

    /* This is an assert() really */
    if (aff->header_size != AFF_HEADER_SIZE) {
	aff->error = "AFF INTERNAL ERROR: header size mismatch";
	goto end;
    }

end:
    {
	const char *msg = aff->error;

	fclose(aff->file);
	aff_stable_fini(aff->stable);
	aff_tree_fini(aff->tree);
	free(aff);
	return msg;
    }
}
