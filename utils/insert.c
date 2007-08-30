/*! $Id: .vimrc,v 1.4 2007/03/22 18:26:06 syritsyn Exp $
 **************************************************************************
 * \file insert.c
 *      Insert one aff file into another
 *
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 28/08/2007
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lhpc-aff.h>
#include "util.h"
#include "common.h"

// XXX root node data is not copied!
const char *insert_data( struct AffWriter_s *w, const char *fname, const char *keypath )
{
    struct AffNode_s *w_node = mkdir_path( w, aff_writer_root( w ), keypath );
    if( NULL == w_node )
        return aff_writer_errstr( w );
    struct AffReader_s *r = aff_reader( fname );
    if( NULL != aff_reader_errstr( r ) )
        return aff_reader_errstr( r );
    struct AffNode_s *r_root = aff_reader_root( r );
    if( NULL == r_root )
        return aff_reader_errstr( r );
// FIXME no data in the root node
//    if( NULL != ( status = copy_node_data( r, r_root, w, w_node ) ) )
//        return status;
        
    struct copy_nodes_arg arg;
    arg.r   = r;
    arg.w   = w;
    arg.w_parent = w_node;
    arg.weak = COPY_NODE_WEAK;
    arg.errstr = NULL;
    aff_node_foreach( r_root, copy_nodes_recursive, &arg );
    aff_reader_close( r );
    if( NULL != arg.errstr )
        return arg.errstr;
    return NULL;
}

int x_insert( int argc, char *argv[] )
{
    const char *status = NULL,
               *fname = NULL, 
               *out_fname = NULL;
    int start_empty = 0;
    
    for( ; argc ; --argc, ++argv )
    {
        if( '-' != argv[0][0] ) 
            break;
        for( char *p = argv[0] + 1 ; '\0' != *p ; ++p )
        {
            switch( *p )
            {
            case 'e':   start_empty = 1;    break;
            case 'o':
                {
                    if( '\0' != *(p+1) || !(--argc) )
                    {
                        fprintf( stderr, "%s: -o must be followed by a file name\n", __func__ );
                        return 1;
                    }
                    out_fname = *(++argv);
                } break;
            default:
                {
                    fprintf( stderr, "%s: unknown parameter -%c\n", 
                                __func__, *p );
                    return 1;
                }
            }
        }
    }
    if( !start_empty )
    {
        if( !(argc--) )
        {
            fprintf( stderr, "%s: start aff file name must be given; "
                     "try 'lhpc-aff help insert'\n", __func__ );
            return 1;
        }
        else
            fname = *(argv++);
        if( NULL == out_fname )
            out_fname = fname;
    }
    else
        if( NULL == out_fname )
        {
            fprintf( stderr, "%s: for empty start aff file, output file name must be specified; "
                     "try 'lhpc-aff help insert'\n", __func__ );
            return 1;
        }
    if( argc % 2 )
    {
        fprintf( stderr, "%s: unpaired aff-file and keypath; "
                 "try 'lhpc-aff help insert'\n", __func__ );
        return 1;
    }
    char *tmp_fname;
    if( NULL == ( tmp_fname = mk_tmp_filename( ".aff-tmp.", out_fname ) ) )
    {
        fprintf( stderr, "%s: cannot create a unique writable file\n", __func__ );
        perror( __func__ );
        return 1;
    }
    struct AffWriter_s *w = aff_writer( tmp_fname );
    if( NULL != aff_writer_errstr( w ) )
    {
        fprintf( stderr, "%s: %s: %s\n", __func__, tmp_fname, aff_writer_errstr( w ) );
        goto errclean_w;
    }
    // insert data
    for( ; argc ; argc -= 2, argv += 2 )
        if( NULL != ( status = insert_data( w, argv[0], argv[1] ) ) )
        {
            fprintf( stderr, "%s: %s :%s: %s\n", __func__, argv[0], argv[1], status );
            goto errclean_w;
        }
    if( !start_empty )
        if( NULL != ( status = insert_data( w, fname, "" ) ) )
        {
            fprintf( stderr, "%s: %s: %s\n", __func__, fname, status );
            goto errclean_w;
        }
    
    
    if( NULL != ( status = aff_writer_close( w ) ) )
    {
        fprintf( stderr, "%s: %s\n", __func__, status );
        if( remove( tmp_fname ) )
            perror( __func__ );
        free( tmp_fname );
        return 1;
    }
    if( rename( tmp_fname, out_fname ) )
    {
        perror( __func__ );
        fprintf( stderr, "%s: output is saved to %s\n", __func__, tmp_fname );
        return 1;
    }
    free( tmp_fname );
    return 0;
    
errclean_w:
    aff_writer_close( w );
    if( remove( tmp_fname ) )
        perror( __func__ );
    free( tmp_fname );
    return 1;
}

void h_insert(void)
{
    printf( "Usage:\nlhpc-aff insert [-o <output>] {-e|<aff-file>} [<aff-file1> <keypath1>] ...\n" 
            "Insert the contents of aff-fileN into aff-file at keypathN.\n"
            "New data replaces old data, and the data in first file of the list\n"
            "takes precedence."
            "\t-o\toutput file name\n"
            "\t-e\tstart with an empty aff-file;\n"
            "\t\tNOTE: with this key, no aff-file name expected, and\n"
            "\t\tan output file name must be given\n");
}
