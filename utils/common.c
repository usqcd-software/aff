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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <lhpc-aff.h>
#include "common.h"

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
void
fprint_path( FILE *stream, struct AffNode_s *root, struct AffNode_s *node )
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
    fprint_path( stream, root, aff_node_parent(node));
    fprintf( stream, "/%s", name );
}

#if 1
char *mk_tmp_filename( const char *mark, const char *fname )
{
    size_t fname_len = strlen( fname );
    size_t mark_len = strlen( mark );
    char *template = "XXXXXX";
    size_t template_len = strlen( template );
    char *res;
    size_t res_max_len = fname_len + mark_len + template_len ;
    if( NULL == ( res = (char *)malloc( res_max_len + 1 ) ) )
    {
        fprintf( stderr, "%s: not enough memory\n", __func__ );
        exit(1);
    }
    size_t res_len;
    strncpy( res, fname, fname_len + 1 ),           res_len = strlen( res );
    strncpy( res + res_len, mark, mark_len + 1 ),   res_len = strlen( res );
    strncpy( res + res_len, template, 
            template_len + 1 ),                     res_len = strlen( res );
    int mkres = mkstemp( res );
    if( mkres < 0 )
    {
        free( res );
        return NULL;
    }
    else
    {
        close( mkres );
        return res;
    }
}
#else 
char *mk_tmp_filename( const char *mark, const char *fname )
{
    size_t fname_len = strlen( fname );
    size_t mark_len = strlen( mark );
    char *res;
    size_t res_max_len = fname_len + mark_len + template_len ;
    if( NULL == ( res = (char *)malloc( res_max_len + 1 ) ) )
    {
        fprintf( stderr, "%s: not enough memory\n", __func__ );
        exit(1);
    }
    size_t res_len;
    strncpy( res, fname, fname_len + 1 ),           res_len = strlen( res );
    strncpy( res + res_len, mark, mark_len + 1 ),   res_len = strlen( res );
    snprintf( res + res_len, res_max_len - res_len, 
            "%ld", (long int)getpid() ),           res_len = strlen( res );
    size_t cnt = 1000;
    long int postfix = time( NULL ) & 0xffffffffUL;
    struct stat st;
    while( cnt-- )
    {
        snprintf( res + res_len, res_max_len - res_len, ".%ld", postfix++ );
        int i = stat( res, &st );
        if( 0 == i )
            continue;
        if( ENOENT == errno )
            return res; 
        break;
    }
    free( res );
    return NULL;
}
#endif

