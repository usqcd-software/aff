/*! $Id: .vimrc,v 1.4 2007/03/22 18:26:06 syritsyn Exp $
 **************************************************************************
 * \file extract.c
 *          Extract a section to a separate aff file
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 28/08/2007
 *
 ***************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lhpc-aff.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"
#include "common.h"

// if fname is NULL, start with empty file
// XXX root node data is not copied!
const char *extract_data( struct AffReader_s *r, const char *fname,
        const char *out_fname, const char *keypath, int sub )
{
    if( NULL == out_fname )
        return "extract_data: invalid output file name";
    const char *status = NULL;
    struct AffNode_s *r_node = chdir_path( r, aff_reader_root( r ), keypath );
    if( NULL == r_node )
        return aff_reader_errstr( r );
    struct stat stat_buf;
    int new_file = 1;
    if( NULL != fname && !( stat( fname, &stat_buf ) ) )
        new_file = 0;
    char *tmp_fname = NULL;
    
    if( ! new_file )
    {
        // create tmpfile just for the case 'fname' and 'out_fname' are the same file
        if( NULL == ( tmp_fname = mk_tmp_filename( ".tmp-aff.", out_fname ) ) )
            return "extract_data: cannot generate unique writable filename";
    }
    else
    {
        // start with empty file 'out_fname'
        if( NULL == ( tmp_fname = (char *)malloc( strlen( out_fname ) + 1 ) ) )
            return "extract_data: not enough memory";
        strcpy( tmp_fname, out_fname );
    }
    struct AffWriter_s *w = aff_writer( tmp_fname );
    if( NULL != ( status = aff_writer_errstr( w ) ) )
        goto errclean_w;
    struct AffNode_s *w_root = aff_writer_root( w );
    if( NULL == w_root )
    {
        status = aff_writer_errstr( w );
        goto errclean_w;
    }
        
    struct copy_nodes_arg arg;
    arg.r   = r;
    arg.w   = w;
    arg.w_parent = w_root;
    arg.weak = COPY_NODE_STRONG;
    arg.errstr = NULL;
    if( sub )
        aff_node_foreach( r_node, copy_nodes_recursive, &arg );
    else
        copy_nodes_recursive( r_node, &arg );
    if( NULL != arg.errstr )
        return arg.errstr;
    
    struct AffReader_s *r_old;
    if( NULL != fname )
    {
        r_old = aff_reader( fname );
        if( NULL != ( status = aff_reader_errstr( r_old ) ) )
            goto errclean_rw;
        struct AffNode_s *r_old_node = aff_reader_root( r_old );
        if( NULL == r_old )
        {
            status = aff_reader_errstr( r_old );
            goto errclean_rw;
        }
        arg.r = r_old;
        arg.w = w;
        arg.w_parent = w_root;
        arg.weak = COPY_NODE_WEAK;
        arg.errstr = NULL;
        aff_node_foreach( r_old_node, copy_nodes_recursive, &arg );
        if( NULL != ( status = arg.errstr ) ) 
            goto errclean_rw;
        aff_reader_close( r_old );
    }
    
    if( NULL != ( status = aff_writer_close( w ) ) )
        return status;
    if( NULL != fname )
        if( rename( tmp_fname, out_fname ) )
        {
            status = strerror( errno );
            perror( __func__ );
            fprintf( stderr, "%s: output saved to %s", __func__, tmp_fname );
            free( tmp_fname );
            return status;
        }
    free( tmp_fname );
    return NULL;
    
errclean_rw:
    aff_reader_close( r_old );
errclean_w:
    aff_writer_close( w );
    if( remove( tmp_fname ) )
        perror( __func__ );
    free( tmp_fname );
    return status;
}
    
int start_empty = 0,
    print_all_subnodes = 0;

int extract_keylist( struct AffReader_s *r, const char *list_fname )
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
        if( start_empty )
            status = extract_data( r, NULL, fargv[0], fargv[1], print_all_subnodes );
        else
            status = extract_data( r, fargv[0], fargv[0], fargv[1], print_all_subnodes );
        if( NULL != status )
        {
            fprintf( stderr, "%s: %s: %s: %s\n", __func__, fargv[0], fargv[1], status );
            return 1;
        }
    }
    fclose( list );
    return 0;    

}

int x_extract( int argc, char *argv[] )
{
    const char *fname = NULL,
               *status = NULL,
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
            case 'e': start_empty = 1;          break;
            case 'a': print_all_subnodes = 1;   break;
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
                    fprintf( stderr, "%s: unknown option -%c\n", __func__, *p );
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
    if( !(argc--) )
    {
        fprintf( stderr, "%s: no aff file name given ; try 'lhpc-aff help extract'\n", 
                __func__ );
        return 1;
    }
    else
        fname = *(argv++);
    if( argc % 2 )
    {
        fprintf( stderr, "%s: unpaired key and filename; try 'lhpc-aff help extract'\n", 
                __func__ );
        return 1;
    }
    struct AffReader_s *r = aff_reader( fname );
    if( NULL != aff_reader_errstr( r ) )
    {
        fprintf( stderr, "%s: %s: %s\n", __func__, fname, aff_reader_errstr( r ) );
        goto errclean_r;
    }
    if( NULL != list1_fname )
    {
        if( extract_keylist( r, list1_fname ) )
            goto errclean_r;
    }
    for( ; argc ; argc-=2, argv+=2 )
    {
        if( start_empty )
            status = extract_data( r, NULL, argv[0], argv[1], print_all_subnodes );
        else
            status = extract_data( r, argv[0], argv[0], argv[1], print_all_subnodes );
        if( NULL != status )
        {
            fprintf( stderr, "%s: %s: %s: %s\n", __func__, argv[0], argv[1], status );
            goto errclean_r;
        }
    }
    if( NULL != list2_fname )
    {
        if( extract_keylist( r, list2_fname ) )
            goto errclean_r;
    }
    aff_reader_close( r );
    return 0;

errclean_r:
    aff_reader_close( r );
    return 1;
}



void h_extract(void)
{
    printf( "Usage:\n"
            "lhpc-aff extract [-ae] <aff-file> [-f <pre-list>] [-F <post-list>]\n"
            "\t\t[<output1> <keypath1>] ...\n"
            "Extract keypaths from aff-file and put it into the root of other aff files.\n"
            "New data replaces old data. If an output file does not exist,\n"
            "it will be created\n"
            "Options:\n"
            "\t-a\textract all the subkeys, instead of the keypath itself\n" 
            "\t-e\tstart each output with an empty file (truncate each file)\n"
            "\t-f <pre-list>\n\t\tprint data from key list in file <pre-list> BEFORE\n"
            "\t\tprocessing command line list\n"
            "\t-F <post-list>\n\t\tprint data from key list in file <post-list> AFTER\n"
            "\t\tprocessing command line list\n"
            "\t\teach line in <list> files must be a pair '<output> <keypath>'.\n" 
            "\t\tparameter to only one of -f, -F options can be '-' (stdin)\n"
          );
}
