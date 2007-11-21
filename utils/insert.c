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
#include <ctype.h>
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

int insert_keylist( struct AffWriter_s *w, const char *list_fname )
{
    char buf[32768];
    char *fargv[2];
    const char * status;
    FILE *list;
    int num;
    if( 0 == strcmp( list_fname, "-" ) )
    {
        list = stdin;
        if( ferror( list ) )
        {
            fprintf( stderr, "%s: bad stdin stream\n", __func__ );
            return 1;
        }
    }
    else
    {
        if( NULL == ( list = fopen( list_fname, "r" ) ) )
        {
            fprintf( stderr, "%s: cannot open %s\n", __func__, list_fname );
            return 1;
        }
    }
    while( NULL != fgets( buf, sizeof(buf), list ) )
    {
        num = split_farg( buf, 2, fargv );
        if( num < 0 )
            return 1;
        if( num == 0 )
            continue;
        if( num == 1 )
        {
            fprintf( stderr, "%s: unpaired file name '%s'\n",
                     __func__, fargv[0] );
            return 1;
        }
//        // FIXME remove test
//        printf( "'%s' -> '%s'\n", fargv[0], fargv[1] );
//        continue;
//        // FIXME end of test
        if( NULL != ( status = insert_data( w, fargv[0], fargv[1] ) ) )
        {
            fprintf( stderr, "%s: %s :%s: %s\n", 
                     __func__, fargv[0], fargv[1], status );
            return 1;
        }
    }
    fclose( list );
    return 0;    
}

int x_insert( int argc, char *argv[] )
{
    const char *status = NULL,
               *out_fname = NULL,
               *list1_fname = NULL,
               *list2_fname = NULL;
    
    for( ; argc ; --argc, ++argv )
    {
        if( '-' != argv[0][0] ) 
            break;
        for( char *p = argv[0] + 1 ; '\0' != *p ; ++p )
        {
            switch( *p )
            {
            case 'o':
                {
                    if( '\0' != *(p+1) || !(--argc) )
                    {
                        fprintf( stderr, "%s: -o must be followed by a file name\n", __func__ );
                        return 1;
                    }
                    out_fname = *(++argv);
                } break;
            case 'f':
                {
                    if( '\0' != *(p+1) || !(--argc) )
                    {
                        fprintf( stderr, "%s: -f must be followed by a file name\n", __func__ );
                        return 1;
                    }
                    list1_fname = *(++argv);
                } break;
            case 'F':
                {
                    if( '\0' != *(p+1) || !(--argc) )
                    {
                        fprintf( stderr, "%s: -F must be followed by a file name\n", __func__ );
                        return 1;
                    }
                    list2_fname = *(++argv);
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
    if( NULL != list1_fname && NULL != list2_fname &&
        0 == strcmp( list1_fname, "-" ) && 0 == strcmp( list2_fname, "-" ) )
    {
        fprintf( stderr, "%s: both pre- and post-list cannot be '-'(stdin)\n",
                 __func__ );
        return 1;
    }
    if( NULL == out_fname )
    {
        fprintf( stderr, "%s: output file name must be specified; try 'lhpc-aff help insert'\n", 
                __func__ );
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
    // insert data from pre-list
    if( NULL != list1_fname )
    {
        if( insert_keylist( w, list1_fname ) )
            goto errclean_w;
    }
    // insert data from cmdline key list
    for( ; argc ; argc -= 2, argv += 2 )
        if( NULL != ( status = insert_data( w, argv[0], argv[1] ) ) )
        {
            fprintf( stderr, "%s: %s :%s: %s\n", __func__, argv[0], argv[1], status );
            goto errclean_w;
        }
    // insert data from post-list
    if( NULL != list2_fname )
    {
        if( insert_keylist( w, list2_fname ) )
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
        free( tmp_fname );
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
    printf( "Usage:\nlhpc-aff insert -o <output> [-f <pre-list>] [-F <post-list>]\n"
            "\t\t[<aff-file1> <keypath1>] ...\n"
            "Join all the files into one aff-file. The contents of <aff-fileN>\n"
            "is inserted at keypathN. New data replaces old data, and the"
            "leftmost data takes precedence. Output file name may be in the list\n"
            "of files to insert. It will be read first, then replaced.\n"
            "\t-o <output>\n\t\twrite result of merge to <output>.\n"
            "\t-f <pre-list>\n\t\tprint data from key list in file <pre-list> BEFORE\n"
            "\t\tprocessing command line list\n"
            "\t-F <post-list>\n\t\tprint data from key list in file <post-list> AFTER\n"
            "\t\tprocessing command line list\n"
            "\t\tparameter to only one of -f, -F options can be '-' (stdin)\n"
          );
}

void h_join(void)
{
    printf( "Usage:\nlhpc-aff join -o <output> [-f <pre-list>] [-F <post-list>]\n"
            "\t\t[<aff-file1> <keypath1>] ...\n"
            "Join all the files into one aff-file. The contents of <aff-fileN>\n"
            "is inserted at keypathN. New data replaces old data, and the"
            "leftmost data takes precedence. Output file name may be in the list\n"
            "of files to insert. It will be read first, then replaced.\n"
            "\t-o <output>\n\t\twrite result of merge to <output>.\n"
            "\t-f <pre-list>\n\t\tprint data from key list in file <pre-list> BEFORE\n"
            "\t\tprocessing command line list\n"
            "\t-F <post-list>\n\t\tprint data from key list in file <post-list> AFTER\n"
            "\t\tprocessing command line list\n"
            "\t\tparameter to only one of -f, -F options can be '-' (stdin)\n"
          );
}
