#include <stdio.h>
#include <lhpc-aff.h>
#include "util.h"

void
h_check(void)
{
    printf("lhpc-aff check [-v] file ...\n"
      "\tChecks integrity of each file. If an error is found, the program\n"
      "\tprints a message on the stderr and exits with a non-zero status.\n"
      "\tIf -v is given, details of the checking are printed on stdout.\n");
}

int
x_check(int argc, char *argv[])
{
    if (argc < 1) {
	fprintf(stderr, "usage: lhpc-aff check [-v] file...\n");
	return 1;
    }
    printf("WRITE ME XXX\n");
    return 1; /* this is an error code */
}
