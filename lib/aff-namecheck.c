#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "aff.h"

int
aff_name_check(const char *name)
{
    int ch;
    if (name == 0 || *name == 0)
	return 1;
    ch = name[0];
    if (!(isalpha(ch) || strchr(":_", ch)))
	return 1;
    for (;*++name;) {
	ch = *name;
	if (!(isalnum(ch) || strchr(":_-.", ch)))
	    return 1;
    }
    return 0;
}
