/*! $Id: .vimrc,v 1.4 2007/03/22 18:26:06 syritsyn Exp $
 **************************************************************************
 * \file import.c
 *
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 23/08/2007
 *
 ***************************************************************************/
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <string.h>
#include <lhpc-aff.h>
#include "util.h"
#include "common.h"

static
int read_nonspace( unsigned char *buf, size_t max, FILE *fi )
{
    if( 0 == max )
        return 0;
    unsigned char c;
    while( isspace( c = fgetc( fi ) ) )
    { 
        if( feof( fi ) )
        {
            *buf = '\0';
            return 0;
        }
    }
    *(buf++) = c;
    size_t k = max;
    while( --k && !feof( fi ) )
    {
        if( isspace( c = fgetc( fi ) ) )
        {
            ungetc( c, fi );
            break;
        }
        *(buf++) = c;
    }
    *buf = '\0';
    return max - k - 1;
}

static
int read_char_list( unsigned char *buf, size_t max, FILE *fi )
{
    return fread( buf, 1, max, fi );
}

static
int read_int_list( int32_t *buf, size_t max, FILE *fi )
{
    size_t cnt = max;
    while( cnt-- )
    {
        long int a;
        if( fscanf( fi, "%ld", &a ) < 1 )
            return cnt - max - 1;
        *(buf++) = a;
    }
    return max - cnt - 1;
}

static
int read_double_list( double *buf, size_t max, FILE *fi )
{
    size_t cnt = max;
    char strbuf[1024], *ptr;
    while( cnt-- )
    {
        if( read_nonspace( strbuf, sizeof(strbuf), fi ) <= 0 )
            return max - cnt - 1;
        *(buf++) = strtod( strbuf, &ptr );
        if( ptr == strbuf )
        {
            fprintf( stderr, "%s: cannot read as double: \"%s\"\n",
                    __func__, ptr );
            return max - cnt - 1;
        }
        if( '\0' != *ptr )
            fprintf( stderr, "%s: trailing characters after double: \"%s\"\n",
                    __func__, ptr );
    }
    return max - cnt - 1;
}

static
int read_complex_list( double _Complex *buf, size_t max, FILE *fi )
{
    size_t cnt = max;
    char strbuf[1024], *ptr;
    while( cnt-- )
    {
        double rv, iv;

        if( read_nonspace( strbuf, sizeof(strbuf), fi ) <= 0 )
            return max - cnt - 1;
        rv = strtod( strbuf, &ptr );
        if( ptr == strbuf )
        {
            fprintf( stderr, "%s: cannot read as double: \"%s\"\n",
                    __func__, ptr );
            return max - cnt - 1;
        }
        if( '\0' != *ptr )
            fprintf( stderr, "%s: trailing characters after double: \"%s\"\n",
                    __func__, ptr );

        if( read_nonspace( strbuf, sizeof(strbuf), fi ) <= 0 )
        {
            fprintf( stderr, "%s: unpaired real part of a complex number\n", __func__ );
            *buf = rv + I * 0.;
            return max - cnt;
        }
        iv = strtod( strbuf, &ptr );
        if( ptr == strbuf )
        {
            fprintf( stderr, "%s: cannot read as double: \"%s\"\n",
                    __func__, ptr );
            *buf = rv + I * 0.;
            return max - cnt ;
        }
        if( '\0' != *ptr )
            fprintf( stderr, "%s: trailing characters after double: \"%s\"\n",
                    __func__, ptr );
        *(buf++) = rv + I * iv;
    }
    return max - cnt - 1;
}

#if 1
static
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
static
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


static
const char *insert_list( struct AffWriter_s *w, struct AffNode_s *w_node,
        enum AffNodeType_e type, size_t size )
{
    if( NULL == w ||
        NULL == w_node )
        return "insert_list: invalid writer or node";
    int status;
    switch( type )
    {
    case affNodeInvalid:    
        return "insert_list: invalid node type";
    case affNodeVoid:
        break;
    case affNodeChar: 
        {
            unsigned char *buf = (char *)malloc( size );
            read_char_list( buf, size, stdin );
            status = aff_node_put_char( w, w_node, buf, size );
            free( buf );
            if( status )
                return aff_writer_errstr( w );
        } break;
    case affNodeInt:
        {
            uint32_t *buf = (uint32_t *)malloc( size * sizeof(uint32_t) );
            read_int_list( buf, size, stdin );
            status = aff_node_put_int( w, w_node, buf, size );
            free( buf );
            if( status )
                return aff_writer_errstr( w );
        } break;
    case affNodeDouble:
        {
            double *buf = (double *)malloc( size * sizeof(double) );
            read_double_list( buf, size, stdin );
            status = aff_node_put_double( w, w_node, buf, size );
            free( buf );
            if( status )
                return aff_writer_errstr( w );
        } break;
    case affNodeComplex:
        {
            double _Complex *buf = (double _Complex *)
                malloc( size * sizeof(double _Complex) );
            read_complex_list( buf, size, stdin );
            status = aff_node_put_complex( w, w_node, buf, size );
            free( buf );
            if( status )
                return aff_writer_errstr( w );
        } break;
    }
    return NULL;
}


int x_import( int argc, char *argv[] )
{
    const char *status;
    const char *out_fname = NULL;
    enum AffNodeType_e type = affNodeVoid;
    long int num = -1;

    if( argc < 2 )
    {
        h_import();
        return 1;
    }

    for( ; argc ; argv++, argc-- )
    {
        if( '-' != argv[0][0] )
            break;
        char *p, *q;
        for( p = argv[0] + 1 ; '\0' != *p ; ++p )
        {
            switch(*p)
            {
            case 'c': type = affNodeChar      ; break;
            case 'i': type = affNodeInt       ; break;
            case 'd': type = affNodeDouble    ; break;
            case 'x': type = affNodeComplex   ; break;
            case 'N': 
                {
                    if( '\0' != *(p+1) || !(--argc) )
                    {
                        fprintf( stderr, "%s: -N must be followed by a number\n", __func__ );
                        return 1;
                    }
                    num = strtol( *(++argv), &q, 10 );
                    if( q == *argv || '\0' != *q )
                    {
                        fprintf( stderr, "%s: -N must be followed by a number\n", __func__ );
                        return 1;
                    }
                } break;
            case 'o': 
                {
                    if( '\0' != *(p+1) || !(--argc) )
                    {
                        fprintf( stderr, "%s: -o must be followed by an output file name\n", 
                                __func__ );
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
    
    const char *fname = NULL;
    if( argc-- )
        fname = *(argv++);
    else
    {
        fprintf( stderr, "%s: no aff-file name\n", __func__ );
        return 1;
    }

    const char *key_path = "";
    if( argc-- )
        key_path = *(argv++);
    
    struct AffReader_s *r;
    r = aff_reader( fname );
    if( NULL != ( status = aff_reader_errstr( r ) ) )
    {        
        fprintf( stderr, "%s: %s\n", __func__, aff_reader_errstr( r ) );
        goto err_clean_r;
    }
    struct AffNode_s *r_root = aff_reader_root( r );
    if( NULL == r_root )
    {
        fprintf( stderr, "%s: %s\n", __func__, aff_reader_errstr( r ) );
        goto err_clean_r;
    }
    // TODO check that the key is empty
    
    char *tmp_fname;
    if( NULL == out_fname )
    {
        if( NULL == ( tmp_fname = mk_tmp_filename( ".aff-tmp.", fname ) ) )
        {
            fprintf( stderr, "%s: cannot create a unique writable file\n", __func__ );
            perror( __func__ );
            return 1;
        }
    }
    else
    {
        tmp_fname = (char *)malloc(  strlen( out_fname ) + 1 );
        strcpy( tmp_fname, out_fname );
    }
            
    struct AffWriter_s *w = aff_writer( tmp_fname );
    if( NULL != ( status = aff_reader_errstr( r ) ) )
    {
        fprintf( stderr, "%s: %s\n", __func__, aff_writer_errstr( w ) );
        goto err_clean_rw;
    }
    struct AffNode_s *w_root = aff_writer_root( w );
    if( NULL == w_root )
    {
        fprintf( stderr, "%s: %s\n", __func__, aff_writer_errstr( w ) );
        goto err_clean_rw;
    }

    // copy all nodes
    struct copy_nodes_arg arg;
    arg.r = r;
    arg.w = w;
    arg.w_parent = NULL;
    arg.errstr = NULL;
    copy_nodes_recursive( r_root, (void *)&arg );
    if( NULL != arg.errstr )
    {
        fprintf( stderr, "%s: %s\n", __func__, arg.errstr );
        goto err_clean_rw;
    }
    
    struct AffNode_s *w_node = mkdir_path( w, w_root, key_path );
    if( NULL == w_node )
    {
        fprintf( stderr, "%s: %s\n", __func__, aff_writer_errstr( w ) );
        goto err_clean_rw;
    }
    if( affNodeVoid != aff_node_type( w_node ) )
    {
        fprintf( stderr, "%s: non-void node type in %s\n", __func__, key_path );
        goto err_clean_rw;
    }
    if( NULL != ( status = insert_list( w, w_node, type, num ) ) )
    {
        fprintf( stderr, "%s: %s\n", __func__, status );
        goto err_clean_rw;
    }
    
    /**/fprintf( stderr, "%s: written file %s\n", __func__, tmp_fname );
    if( NULL == out_fname )
    {
        if( rename( tmp_fname, fname ) )
        {
            perror( __func__ );
            fprintf( stderr, "%s: output is saved to %s\n", __func__, tmp_fname );
            return 1;
        }
    }
    aff_writer_close( w ); 
    aff_reader_close( r );
    free( tmp_fname );
    return 0;
    
err_clean_r:
    aff_reader_close( r );
    return 1;
    
err_clean_rw:
    aff_writer_close( w ); 
    aff_reader_close( r ); 
    if( remove( tmp_fname ) )
        perror( __func__ );
    free( tmp_fname );
    return 1;
}

void h_import(void)
{
    printf( "Usage:\nlhpc-aff import -[cidx] [-N <N>] [-o <output>] <aff-file> [key-path]\n"
            "Import blank-separated data from standard input to <aff-file> under key-path.\n"
            "If no output file name given, rewrite the original file.\n"
            "\t-c\tchar array input\n"  
            "\t-i\tinteger array input\n"
            "\t-d\tdouble precision real number array input\n"
            "\t-x\tdouble precision complex number array input\n"
            "\t-N <N>\tarray of length N\n"
            "\t-o <output>\n\t\toutput to file `name'\n"
            );
}
