#include <stdint.h>
#include <stdio.h>
#include "stable.h"

#define SIZE 1024

static char buffer[SIZE];
static char sym[SIZE];

int
main(int argc, char *argv[])
{
    struct AffSTable_s *stable;
    const struct AffSymbol_s *s;
    
    stable = aff_st_init();
    for (;;) {
	printf("> ");
	fflush(stdout);
	if (fgets(buffer, SIZE - 1, stdin) == 0)
	    break;
	if (sscanf(buffer, "%s", sym) != 1)
	    continue;
	s = aff_st_insert(stable, sym);
	printf("insert(%32s) = %16p\n", sym, s);
    }
    aff_st_print(stable);
    stable = aff_st_fini(stable);
    return 0;
}
