#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <aff.h>
#include <node.h>

static const char *
normalize(const xmlChar *data)
{
    const char *v = (const char *)data;
    for (;*v; v++) {
	if (!isspace(*v))
	    break;
    }
    return v;
}

static int
do_document(struct AffWriter_s *writer,
	    struct AffNode_s *a_node,
	    xmlDocPtr doc,
	    xmlNodePtr x_node)
{
    const xmlChar *x_name = x_node->name;

    if (xmlStrcmp(x_name, (const xmlChar *)"text") != 0) {
	xmlChar *data = xmlNodeListGetString(doc, x_node, 1);
	const char *v;
	if (data == 0) {
	    fprintf(stderr, "*** xml: Node list string is NULL\n");
	    return 1;
	}
	v = normalize(data);
	if (*v != 0) {
	    if (aff_node_put_char(writer, a_node, v, strlen(v))) {
		xmlFree(data);
		fprintf(stderr, "*** aff error: %s\n",
			aff_writer_errstr(writer));
		return 1;
	    }
	}
	xmlFree(data);
    } else {
	/* not a text element */
	a_node = aff_writer_mkdir(writer, a_node, (const char *)x_name);
	if (a_node == 0) {
	    fprintf(stderr, "*** aff error: %s\n", aff_writer_errstr(writer));
	    return 1;
	}
	for (x_node = x_node->xmlChildrenNode; x_node; x_node = x_node->next) {
	    if (do_document(writer, a_node, doc, x_node))
		return 1;
	}
    }
    return 0;
}

static int
process_xml(const char *out_name,
	    const char *in_name)
{
    struct AffWriter_s *wr = aff_writer(out_name);
    struct AffNode_s *root;
    const char *doc_name = basename(in_name);
    xmlDocPtr doc = xmlParseFile(in_name);
    xmlNodePtr node;
    int status = 1;

    if (doc == 0) {
	fprintf(stderr, "*** error reading %s\n", in_name);
	aff_writer_close(wr);
	return 1;
    }
    node = xmlDocGetRootElement(doc);
    if (node == 0) {
	fprintf(stderr, "*** empty doc?\n");
	goto end;
    }

    root = aff_writer_mkdir(wr, aff_writer_root(wr), doc_name);
    do_document(wr, root, doc, node);
    
end:
    xmlFreeDoc(doc);
    aff_writer_close(wr);
    printf("test-waff status is %d\n", status);
    return status;
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
	fprintf(stderr, "usage: test-waff output input\n");
	return 1;
    }
    return process_xml(argv[1], argv[2]);
}
