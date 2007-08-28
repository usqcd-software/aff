/*! $Id: .vimrc,v 1.4 2007/03/22 18:26:06 syritsyn Exp $
 **************************************************************************
 * \file common.h
 *
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 28/08/2007
 *
 ***************************************************************************/
#ifndef UTIL_COMMON_H_
#define UTIL_COMMON_H_
#include <lhpc-aff.h>
struct AffNode_s *chdir_path(struct AffReader_s *r,
               struct AffNode_s *r_node, const char *key_path );
struct AffNode_s *mkdir_path( struct AffWriter_s *w,
                struct AffNode_s *w_node, const char *key_path );

const char *copy_node( struct AffReader_s *r, struct AffNode_s *r_node,
                struct AffWriter_s *w, struct AffNode_s *w_parent,
                        struct AffNode_s **w_new );
struct copy_nodes_arg
{
    struct AffReader_s  *r;
    struct AffWriter_s  *w;
    struct AffNode_s    *w_parent;
    const char          *errstr;
};
void copy_nodes_recursive( struct AffNode_s *r_node, void *arg_ );

    void
print_path( struct AffNode_s *root, struct AffNode_s *node );
#endif/*UTIL_COMMON_H_*/
