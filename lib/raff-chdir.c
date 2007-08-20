#include <stdint.h>
#include <stdio.h>
#include "node.h"
#include "md5.h"
#include "aff-i.h"

struct AffNode_s *
aff_reader_chdir(struct AffReader_s *aff,
		 struct AffNode_s *dir,
		 const char *name)
{
    if (aff == 0 || dir == 0 || name == 0 || aff->error != 0)
	return 0;
    else
	return aff_node_chdir(aff->tree, aff->stable, dir, 0, name);
}
