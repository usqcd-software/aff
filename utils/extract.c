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
        const char *dst_fname, const char *dst_kpath, const char *src_kpath )
{
    if( NULL == dst_fname )
        return "extract_data: invalid output file name";
    const char *status = NULL;
    struct AffNode_s *r_node = chdir_path( r, aff_reader_root( r ), src_kpath );
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
        if( NULL == ( tmp_fname = mk_tmp_filename( ".tmp-aff.", dst_fname ) ) )
            return "extract_data: cannot generate unique writable filename";
    }
    else
    {
        // start with empty file 'out_fname'
        if( NULL == ( tmp_fname = (char *)malloc( strlen( dst_fname ) + 1 ) ) )
            return "extract_data: not enough memory";
        strcpy( tmp_fname, dst_fname );
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
    struct AffNode_s *w_node = mkdir_path( w, w_root, dst_kpath );
    if( NULL == w_node )
    {
        status = aff_writer_errstr( w );
        goto errclean_w;
    }
        
    struct copy_nodes_arg arg;
    arg.r   = r;
    arg.w   = w;
    arg.w_parent = w_node;
    arg.weak = COPY_NODE_STRONG;
    arg.errstr = NULL;
    aff_node_foreach( r_node, copy_nodes_recursive, &arg );
    if( NULL != arg.errstr ) {
	status = arg.errstr;
	goto errclean_tfn;
    }
    
    struct AffReader_s *r_old;
    if( ! new_file )
    {
        r_old = aff_reader( fname );
        if( NULL != ( status = aff_reader_errstr( r_old ) ) )
            goto errclean_rw;
        struct AffNode_s *r_old_root = aff_reader_root( r_old );
        if( NULL == r_old_root )
        {
            status = aff_reader_errstr( r_old );
            goto errclean_rw;
        }
        arg.r = r_old;
        arg.w = w;
        arg.w_parent = w_root;
        arg.weak = COPY_NODE_WEAK;
        arg.errstr = NULL;
        aff_node_foreach( r_old_root, copy_nodes_recursive, &arg );
        if( NULL != ( status = arg.errstr ) ) 
            goto errclean_rw;
        aff_reader_close( r_old );
    }
    
    if( NULL != ( status = aff_writer_close( w ) ) )
	goto errclean_tfn;
    if( NULL != fname )
        if( rename( tmp_fname, dst_fname ) )
        {
            status = strerror( errno );
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
        perror( tmp_fname );
errclean_tfn:
    free( tmp_fname );
    return status;
}
    
int start_empty = 0;

int extract_keylist( struct AffReader_s *r, const char *list_fname )
{
    char buf[49152];
    char *fargv[3];
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
        if( '\n' != buf[strlen(buf)-1] )
        {
            fprintf( stderr, "%s: line too long, skipping\n", __func__ );
            while( NULL != fgets( buf, sizeof(buf), list ) )
                if( '\n' == buf[strlen(buf)-1] )
                    break;
            continue;
        }
        num = split_farg( buf, 3, fargv );
        if( num < 0 )
        {
            fprintf( stderr, "%s: unexpected result of split_farg; exiting\n",
                    __func__ );
            goto errclean_r;
        }
        if( num == 0 )
            continue;
        if( num < 3 )
        {
            fprintf( stderr, "%s: syntax error: expected 3 names, only %d given\n",
                     __func__, num );
            goto errclean_r;
        }
        if( start_empty )
            status = extract_data( r, NULL, fargv[1], fargv[2], fargv[0] );
        else
            status = extract_data( r, fargv[1], fargv[1], fargv[2], fargv[0] );
        if( NULL != status )
        {
            fprintf( stderr, "%s: %s[%s] <- [%s]: %s\n", 
                    __func__, fargv[1], fargv[2], fargv[0], status );
            goto errclean_r;
        }
    }
    fclose( list );
    return 0;    

errclean_r:
    fclose( list );
    return 1;
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
    if( argc % 3 )
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
    for( ; argc ; argc-=3, argv+=3 )
    {
        if( start_empty )
            status = extract_data( r, NULL, argv[1], argv[2], argv[0] );
        else
            status = extract_data( r, argv[1], argv[1], argv[2], argv[0] );
        if( NULL != status )
        {
            fprintf( stderr, "%s: %s[%s] <- [%s]: %s\n", 
                    __func__, argv[1], argv[2], argv[0], status );
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
            "lhpc-aff extract [-e] [-f <pre-list>] [-F <post-list>] <aff-file>\n"
            "\t\t[<src-kpath> <dst-file> <dst-kpath>] ...\n"
            "Extract recursively all the data entries from <src-kpath> of <aff-file>\n"
            "and put it into <dst-file> at <dst-kpath>:\n"
            "<dst-file>[<dst-kpath>/]  <--  <aff-file>[<src-kpath>/*]\n"
            "Note that the data from <src-kpath> itself is not copied. New data\n"
            "replaces old data. If an output file does not exist, it will be created\n"
            "Options:\n"
            "\t-e\tstart each export with an empty output file\n"
            "\t\t(all preexisting data is discarded)\n"
            "\t-f <pre-list>\n\t\tprint data from key list in file <pre-list> BEFORE\n"
            "\t\tprocessing command line list\n"
            "\t-F <post-list>\n\t\tprint data from key list in file <post-list> AFTER\n"
            "\t\tprocessing command line list\n"
            "\t\teach line of <list> is a separate extract instruction,\n"
            "\t\tand it must have at least three names, in order:\n"
            "\t\t<src-kpath> <dst-file> <dst-kpath>.\n"
            "\t\tparameter to only one of -f, -F options can be '-' (stdin)\n"
          );
}
