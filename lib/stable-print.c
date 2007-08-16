#include <stdint.h>
#include <stdio.h>
#include "stable-i.h"

static void
print_string(const char *name, uint32_t id, void *arg)
{
    printf(" %08x: %s\n", id, name);
}

void
aff_st_print(struct AffSTable_s *st)
{
    printf("String table at %p:\n", st);
    aff_st_foreach(st, print_string, 0);
    printf("String table end\n");
}
