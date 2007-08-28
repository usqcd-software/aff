/*! $Id: .vimrc,v 1.4 2007/03/22 18:26:06 syritsyn Exp $
 **************************************************************************
 * \file chdir_keypath.c
 *      change(make) key directory in AffReader(AffWriter)
 *
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 28/08/2007
 *
 ***************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <lhpc-aff.h>
#include "common.h"

static
char *my_strsep( char *str, char **end, char delim )
{
    if( NULL == str )
        return *end = NULL;
    char *s;
    for( s = str ; delim != *s && '\0' != *s; ++s );
    if( '\0' == *s )
        *end = NULL;
    else
    {
        *s = '\0';
        *end = s + 1;
    }
    return str;
}

/* Rules for chdir_path & mkdir_path (same, except for treatment of absent keys)
   - starting '/' : ignore [rw]_node, start from root
   - double slash '//': ignore repeated slashes
   - empty/NULL key_path -?
   - NULL r_node -?
 */
struct AffNode_s *chdir_path(struct AffReader_s *r, 
        struct AffNode_s *r_node, const char *key_path )
{
    if( NULL == r ||
        NULL == key_path )
        return NULL;
    char *kpath = malloc( strlen( key_path ) );
    strcpy( kpath, key_path );
    char *s, *end = kpath;
    
    if( '/' == *end )
        r_node = aff_reader_root( r );
    if( NULL == r_node )
        return NULL;
    
    while( NULL != ( s = my_strsep( end, &end, '/' ) ) &&
           r_node != NULL )
    {
        if( '\0' == *s )                
            continue;
        r_node = aff_reader_chdir( r, r_node, s );
    }
    free( kpath );
    return r_node;
}

struct AffNode_s *mkdir_path( struct AffWriter_s *w, 
        struct AffNode_s *w_node, const char *key_path )
{
    if( NULL == w ||
        NULL == key_path )
        return NULL;
    char *kpath = malloc( strlen( key_path ) );
    strcpy( kpath, key_path );
    char *s, *end = kpath;
    
    if( '/' == *end )                           
        w_node = aff_writer_root( w );
    while( NULL != ( s = my_strsep( end, &end, '/' ) ) &&
           w_node != NULL )
    {
        if( '\0' == *s )  
            continue;
        w_node = aff_writer_mkdir( w, w_node, s );
    }
    free( kpath );
    return w_node;
}
