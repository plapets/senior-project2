
#include "DEBUG_PRINT.h"

static int G_DEBUG_PRINT_LEVEL  = 1;
static int G_DEBUG_PRINT_RETVAL = 0;

void DEBUG_PRINT_LEVEL_ENTER( void )
{
    G_DEBUG_PRINT_LEVEL += 2;
}

void DEBUG_PRINT_LEVEL_EXIT( void )
{
    G_DEBUG_PRINT_LEVEL -= 2;
}

void DEBUG_PRINT_LEVEL( FILE * fptr, char * message )
{
    fprintf(fptr, "%*c%s", G_DEBUG_PRINT_LEVEL, ' ', message);
    fflush(fptr);
}

void _RV_SET( int val )
{
    G_DEBUG_PRINT_RETVAL = val;
}

int _RV_GET( void )
{
    return (G_DEBUG_PRINT_RETVAL);
}

