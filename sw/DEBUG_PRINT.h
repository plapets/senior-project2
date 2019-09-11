
#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#include <stdio.h>
#include <assert.h>

#define FAILURE_FORBIDDEN   (0)
#define FAILURE_ALLOWED     (!FAILURE_FORBIDDEN)

extern FILE * G_SYSTEM_LOG;

int     _RV_GET( void   );
void    _RV_SET( int val);

#define DEBUG_PRINT(FPTR, MESSAGE, RETVAL, BEHAVIOR)                \
    _RV_SET((RETVAL));                                              \
    DEBUG_PRINT_LEVEL((FPTR), (MESSAGE));                           \
    fprintf(                                                        \
            (FPTR),                                                 \
            (0 == _RV_GET())                                        \
                ? "success.\n"                                      \
                : "%s: %s(%d) in %s: process returned %d.\n",       \
            ((BEHAVIOR) != FAILURE_FORBIDDEN)   ? "warning"         \
                                                : "error",          \
            __FILE__,                                               \
            __LINE__,                                               \
            __PRETTY_FUNCTION__,                                    \
            _RV_GET()                                               \
           );                                                       \
    fflush((FPTR));                                                 \
    if (0 != _RV_GET()) assert(FAILURE_FORBIDDEN != (BEHAVIOR))

void DEBUG_PRINT_LEVEL_ENTER( void );
void DEBUG_PRINT_LEVEL_EXIT( void );
void DEBUG_PRINT_LEVEL( FILE * fptr, char * message );

#endif /* DEBUG_PRINT_H */

