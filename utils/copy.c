/*! $Id: .vimrc,v 1.9 2007/08/29 18:08:37 syritsyn Exp $
 **************************************************************************
 * \file copy.c
 *
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 30/08/2007
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <lhpc-aff.h>
#include "util.h"
#include "common.h"

int x_copy( int argc, char **argv )
{
    int copy_recursive = 0;
    const char *src_fname = NULL,
               *src_keypath = NULL,
               *dst_fname = NULL,
               *dst_keypath = NULL,
               *status = NULL;
    for( ; argc ; --argc, ++argv )
    {
        if( '-' != argv[0][0] )
            break;
        for( char *p = argv[0] + 1 ; '\0' != *p ; ++p )
        {
            switch( *p )
            {
            case 'R': copy_recursive = 1;       break;
            default:
                fprintf( stderr, "%s: unknown option -%c\n", __func__, *p );
                return 1;
            }
        }
    }
    if( !(argc--) )
    {
        fprintf( stderr, "%s: source aff file name should be given\n", __func__ );
        return 1;
    }
    else
        src_fname = *(argv++);
    if( !(argc--) )
    {
        fprintf( stderr, "%s: source key name should be given\n", __func__ );
        return 1;
    }
    else
        src_keypath = *(argv++);
    if( !(argc--) )
    {
        fprintf( stderr, "%s: target aff file name should be given\n", __func__ );
        return 1;
    }
    else
        dst_fname = *(argv++);
    if( !(argc--) )
    {
        fprintf( stderr, "%s: target key name should be given\n", __func__ );
        return 1;
    }
    else
        dst_keypath = *(argv++);

    struct AffReader_s *r = aff_reader( src_fname );
    if( NULL != aff_reader_errstr( r ) )
    {
        fprintf( stderr, "%s: %s: %s\n", __func__, src_fname, aff_reader_errstr( r ) );
        goto errclean_r;
    }
    struct AffNode_s *r_node = chdir_path( r, aff_reader_root( r ), src_keypath  );
    if( NULL == r_node )
    {
        fprintf( stderr, "%s: %s: %s: %s\n", 
                __func__, src_fname, src_keypath, aff_reader_errstr( r ) );
        goto errclean_r;
    }
    char *tmp_fname = mk_tmp_filename( ".aff-tmp.", dst_fname );
    if( NULL == tmp_fname )
    {
        fprintf( stderr, "%s: could not create a unique writable file\n", __func__ );
        goto errclean_r;
    }
    struct AffWriter_s *w = aff_writer( tmp_fname );
    if( NULL != aff_writer_errstr( w ) )
    {
        fprintf( stderr, "%s: %s: %s\n", __func__, tmp_fname, aff_writer_errstr( w ) );
        goto errclean_rw;
    }
    struct AffNode_s *w_node = mkdir_path( w, aff_writer_root( w ), dst_keypath );
    if( NULL == w_node )
    {
        fprintf( stderr, "%s: %s: %s: %s\n", 
                __func__, tmp_fname, dst_keypath, aff_writer_errstr( w ) );
        goto errclean_rw;
    }
    if( aff_writer_root( w ) == w_node )
    {
        fprintf( stderr, "%s: %s: cannot replace the root node; use extract instead\n",
                __func__, dst_keypath );
        goto errclean_rw;
    }
    
    if( NULL != ( status = copy_node_data( r, r_node, w, w_node, COPY_NODE_STRONG ) ) )
    {
        fprintf( stderr, "%s: %s", __func__, status );
        goto errclean_rw;
    }
    struct copy_nodes_arg arg;
    if( copy_recursive )
    {
        arg.r = r;
        arg.w = w;
        arg.w_parent = w_node;
        arg.weak = COPY_NODE_STRONG;
        arg.errstr = NULL;
        aff_node_foreach( r_node, copy_nodes_recursive, (void *)&arg );
        if( NULL != arg.errstr )
        {
            fprintf( stderr, "%s: %s\n", __func__, arg.errstr );
            goto errclean_rw;
        }
    }
    aff_reader_close( r );

    r = aff_reader( dst_fname );
    if( NULL != aff_reader_errstr( r ) )
    {
        fprintf( stderr, "%s: %s\n", __func__, aff_reader_errstr( r ) );
        goto errclean_rw;
    }
    arg.r = r;
    arg.w = w;
    arg.w_parent = aff_writer_root( w );
    arg.weak = COPY_NODE_WEAK;
    arg.errstr = NULL;
    aff_node_foreach( aff_reader_root( r ), copy_nodes_recursive, (void *)&arg );
    if( NULL != arg.errstr )
    {
        fprintf( stderr, "%s: %s\n", __func__, arg.errstr );
        goto errclean_rw;
    }

    if( NULL != ( status = aff_writer_close( w ) ) )
    {
        fprintf( stderr, "%s: %s: %s\n", __func__, tmp_fname, status );
        goto errclean_r;
    }
    if( rename( tmp_fname, dst_fname ) )
    {    
        perror( dst_fname );
        if( remove( tmp_fname ) )
            perror( tmp_fname );
        goto errclean_r;
    }
    free( tmp_fname );
    aff_reader_close( r );
    return 0;

errclean_rw:
    aff_writer_close( w );
    if( remove( tmp_fname ) )
        perror( tmp_fname );
    free( tmp_fname );
errclean_r:
    aff_reader_close( r );
    return 1;
}

void h_copy(void)
{
    printf( "Usage:\n"
            "lhpc-aff cp <src> <src-keypath> <dst> <dst-keypath>\n" 
            "Copy data (and all descendants, if necessary) of key src-keypath\n"
            "from the aff file src to dst-keypath of the aff file dst.\n"
            "Key dst-keypath cannot refer to the root node, use 'extract' instead.\n"
            "New data replaces old data. Both files must be valid aff files.\n"
            "Options:\n"
            "\t-R\tcopy all descendants as well\n"
            );
}
