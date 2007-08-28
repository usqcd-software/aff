/*! $Id: .vimrc,v 1.4 2007/03/22 18:26:06 syritsyn Exp $
 **************************************************************************
 * \file common.c
 *      all auxiliary functions that do not fit to anywhere else
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 28/08/2007
 *
 ***************************************************************************/
#include <stdio.h>
#include <lhpc-aff.h>

/* by AVP */
void
print_path( struct AffNode_s *root, struct AffNode_s *node )
{
    const struct AffSymbol_s *sym;
    const char *name;

    if (node == root)
        return;
    sym = aff_node_name(node);
    name = aff_symbol_name(sym);
    if (node == 0 || sym == 0 || name == 0) {
        fprintf(stderr, "lhpc-aff: internal error in print_path()\n");
        exit(1);
    }
    print_path( root, aff_node_parent(node));
    printf("/%s", name);
}
