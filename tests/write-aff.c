#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include "aff.h"

static char s1[] = "Foo bar zap";
static char s2[] = "Second string is here";
static uint32_t i1[100];
static double d1[100];
static double _Complex c1[100];

int
main(int argc, char *argv[])
{
    struct AffWriter_s *aff;
    struct AffNode_s *dir;
    const char *status;

    if (argc != 2) {
	fprintf(stderr, "usage: write-aff output-name\n");
	return 1;
    }
    aff = aff_writer(argv[1]);
    dir = aff_writer_root(aff);
    dir = aff_writer_mkdir(aff, dir, "top");
    aff_writer_mkdir(aff, dir, "empty-level2");
    aff_node_put_char(aff, aff_writer_mkdir(aff, dir, "stringA"),
		      s1, strlen(s1));
    aff_node_put_char(aff, aff_writer_mkdir(aff, dir, "stringB"),
		      s2, strlen(s2));
    i1[0] = 1; i1[1] = -42; i1[2] = 123456789; i1[3] = -1; i1[4] = 0;
    aff_node_put_int(aff, aff_writer_mkdir(aff, dir, "integers"),
		     i1, 5);
    i1[0] = 0x12345678; i1[1] = 0x98765432; i1[2] = 0x76543210; i1[3] = 0;
    i1[4] = 42; i1[5] = -256; i1[6] = 7; i1[7] = 127;
    aff_node_put_int(aff, aff_writer_mkdir(aff, dir, "moreInts"),
		     i1, 8);

    c1[0] = 1.0+2*I; c1[1] = 3-4*I; c1[2] = -5 +6*I; c1[3] = 1.75+2.3*I;
    aff_node_put_complex(aff, aff_writer_mkdir(aff,
					       aff_writer_mkdir(aff, dir,
								"step-2"),
					       "step-3"),
			 c1, 4);

    d1[0] = -INFINITY; d1[1] = 0; d1[2] = 255; d1[3] = 1.4;
    d1[4] = 1.234e-309; d1[5] = -0.0; d1[6] = NAN; d1[7] = +INFINITY;
    d1[8] = -1;

    aff_node_put_double(aff, aff_writer_mkdir(aff,
					      aff_writer_mkdir(aff, dir,
							       "step-d"),
					      "step-d"),
			 d1, 9);


    status = aff_writer_close(aff);

    if (status)
	printf("Mistakes were made %s: %s\n", argv[1], status);

    return 0;
}

