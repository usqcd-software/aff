/*! $Id: .vimrc,v 1.4 2007/03/22 18:26:06 syritsyn Exp $
 **************************************************************************
 * \file cat.c
 *
 * \author Sergey N. Syritsyn
 * 
 * \date Created: 24/08/2007
 *
 ***************************************************************************/
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
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

int gnuplot_spacing = 0,
    print_comment_line = 0,
    print_index = 0,
    print_all_subnodes = 0,
    print_recursive = 0;

struct cat_node_arg
{
    struct AffReader_s *r;
    int first;
    const char *errstr;
};


/* For affNodeInvalid, print error
   for affNodeVoid, print nothing */
static
void cat_single_node( struct AffNode_s *r_node, void *arg_ )
{
    if( NULL == r_node ||
        NULL == arg_ )
        return;
    struct cat_node_arg *arg = (struct cat_node_arg *)arg_;
    if( NULL != arg->errstr )
        return;
    struct AffReader_s *r = arg->r;
    size_t size = aff_node_size( r_node );
    enum AffNodeType_e type = aff_node_type( r_node );
    
    if( affNodeInvalid == type )
    {
        arg->errstr = "cat_single_node: invalid node\n";
        return;
    }
    if( affNodeVoid == type )       // FIXME is it right?
        return;
    if( !arg->first && gnuplot_spacing )
        printf( "\n\n" );
    if( print_comment_line )
    {
        arg->first = 0;
        printf( "# " ); 
        const char *type_str = NULL;
        switch( aff_node_type( r_node ) )
        {
        case affNodeInvalid: 
            {
                arg->errstr = "cat_single_node: invalid node\n";
                return;
            }
        case affNodeVoid:       type_str = "void";      break;
        case affNodeChar:       type_str = "char";      break;
        case affNodeInt:        type_str = "int";       break;
        case affNodeDouble:     type_str = "double";    break;
        case affNodeComplex:    type_str = "complex";   break;
        }
        printf( "%s[%d]  ", type_str, size );
        print_path( aff_reader_root( r ), r_node );
        printf( "\n" );
    }
    switch( aff_node_type( r_node ) )
    {
    case affNodeInvalid: 
        {
            arg->errstr = "cat_single_node: invalid node\n";
            return;
        }
    case affNodeVoid: break;
    case affNodeChar: 
        {
            char *buf = (char *)malloc( size );
            if( aff_node_get_char( r, r_node, buf, size ) )
            {
                arg->errstr = aff_reader_errstr( r );
                return;
            }
	    for ( size_t i = 0; i < size; i++) {
		unsigned char p = buf[i];
		if (p < 32 || p >= 127 || p == '\"' || p == '\\')
		    printf("\\x%02x", p);
		else
		    printf("%c", p);
	    }
            printf( "\n" );
            free( buf );
        } break;
    case affNodeInt:
        {
            uint32_t *buf = (uint32_t *)malloc( size *sizeof(uint32_t) );
            if( aff_node_get_int( r, r_node, buf, size ) )
            {
                arg->errstr = aff_reader_errstr( r );
                return;
            }
            for( size_t i = 0 ; i < size ; ++i )
            {
                if( print_index )
                    printf( "%ld\t", (long int)i );
                printf( "%ld\n", (long int)(buf[i]) );
            }
            free( buf );
        } break;
    case affNodeDouble: 
        {
            double *buf = (double *)malloc( size *sizeof(double) );
            if( aff_node_get_double( r, r_node, buf, size ) )
            {
                arg->errstr = aff_reader_errstr( r );
                return;
            }
            for( size_t i = 0 ; i < size ; ++i )
            {
                if( print_index )
                    printf( "%ld\t", (long int)i );
                printf( "%24.16e\n", buf[i] );
            }
            free( buf );
        } break;
    case affNodeComplex: 
        {
            double _Complex *buf = (double _Complex *)
                malloc( size *sizeof(double _Complex) );
            if( aff_node_get_complex( r, r_node, buf, size ) )
            {
                arg->errstr = aff_reader_errstr( r );
                return;
            }
            for( size_t i = 0 ; i < size ; ++i )
            {
                if( print_index )
                    printf( "%ld\t", (long int)i );
                printf( "%24.16e\t%24.16e\n", creal( buf[i] ), cimag( buf[i] ) );
            }
            free( buf );
        } break;
    }
}

void cat_nodes_recursive( struct AffNode_s *r_node, void *arg_ )
{
    if( NULL == r_node ||
        NULL == arg_ )
        return;
    cat_single_node( r_node, arg_ );
    struct cat_node_arg *arg = (struct cat_node_arg *)arg_;
    if( NULL != arg->errstr )
        return;
    aff_node_foreach( r_node, cat_nodes_recursive, arg_ );
}

int x_cat( int argc, char *argv[] )
{
// -g print for gnuplot
// -c comments
// -n numbering
// -a all immediate subnodes
// -R recursive //TODO
    if( argc < 2 )
    {
        h_cat();
        return 1;
    }
    for( ; argc ; --argc, ++argv )
    {
        if( '-' != argv[0][0] )
            break;
        for( char *p = argv[0] + 1; '\0' != *p ; ++p )
        {
            switch(*p)
            {
            case 'g':   gnuplot_spacing = 1;        break;
            case 'c':   print_comment_line = 1;     break;
            case 'n':   print_index = 1;            break;
            case 'a':   print_all_subnodes = 1;     break;
            case 'R':   print_recursive = 1;        break;
            default:    
                fprintf( stderr, "%s: unknown option -%c\n", __func__, *p );
                return 1;
            }
        }
    }
    const char *fname = NULL;
    if( argc-- )
        fname = *(argv++);
    else
    {
        fprintf( stderr, "%s: aff file name missing\n", __func__ );
        return 1;
    }
    struct AffReader_s *r = aff_reader( fname );
    if( NULL != aff_reader_errstr( r ) )
    {
        fprintf( stderr, "%s: %s\n", __func__, aff_reader_errstr( r ) );
        goto errclean_r;
    }
    struct AffNode_s *r_root = aff_reader_root( r );
    if( NULL == r_root )
    {
        fprintf( stderr, "%s: %s\n", __func__, aff_reader_errstr( r ) );
        goto errclean_r;
    }
    
    int first = 1;
    // TODO print root for empty key list
    for( ; argc ; --argc, ++argv, first = 0 )
    {
        struct AffNode_s *r_node = chdir_path( r, r_root, *argv );
        if( NULL == r_node )
        {
            fprintf( stderr, "%s: %s\n", __func__, aff_reader_errstr( r ) );
            goto errclean_r;
        }
        struct cat_node_arg arg;
        arg.r = r;
        arg.first = first;
        arg.errstr = NULL;
        if( print_recursive )
            cat_nodes_recursive( r_node, &arg );
        else if( print_all_subnodes )
            aff_node_foreach( r_node, cat_single_node, &arg );
        else
            cat_single_node( r_node, &arg );
        if( NULL != arg.errstr )
        {
            fprintf( stderr, "%s: %s", __func__, arg.errstr );
            return 1;
        }
    }

    aff_reader_close( r );
    return 0;

    errclean_r:
    aff_reader_close( r );
    return 1;
}

void h_cat(void)
{
    printf( "Usage:\n"
            "lhpc-aff cat [-acgnR] <aff-file> <keypath> ...\n"
            "Print the data associated with keypath in the file aff-file\n" 
            "\t-a\tprint data of all immediate subkeys instead of given keypath itself\n"
            "\t-c\tprint a comment line starting with #\n" 
            "\t-g\tput gnuplot-style double new-line separators between data\n"
            "\t\tof different keys\n"
            "\t-n\tput the array index in a first column\n"
            "\t-R\tprint the keypath all its subkeys recursively; \n"
            "\t\tthis key takes precedence over `-a'\n");
}
