
/*
File:   RunProcessByFormat.c
Date:   2019-02-22
Author: Peter Lapets

Description:
This file implements a variadic function which uses a format string to run
a process.  Based on the format string, some of the arguments are used as
arguments to the process command line, and others are used to store the
scanned output of the process.

The format string has the form:

"[process path] [argument format specifiers] : [output format specifiers]"

For example:

"echo $(( %d + %d )) : %d"

*/
#include "RunProcessByFormat.h"

int RunProcessByFormat( const char * const format, ... )
{
    FILE *  pipe            = NULL;

    char *  formatCopy      = NULL; /* holds a copy for strtok      */
    char *  commandFormat   = NULL; /* unpopulated command string   */
    char *  outputFormat    = NULL; /* used to parse command output */

    FILE *  commandStream   = NULL; /* concatenates arguments       */
    char *  command         = NULL; /* command run by popen         */
    size_t  commandLength   = 0;    /* number of chars streamed     */

    int     result          = RPBF_E_NONE;

    va_list args;                   /* for accessing variadic args  */

    do
    {
        /* need to copy runFormat: strtok modifies the passed string */
        formatCopy = malloc(strlen(format) + 1);
        if (NULL == formatCopy) { result = RPBF_E_MALLOC; break; }

        strcpy(formatCopy, format);

        /* first part of string defines the call */
        commandFormat = strtok(formatCopy, ":");

        /* create a memstream to concatenate arguments */
        commandStream = open_memstream(&command, &commandLength);
        if (NULL == commandStream) { result = RPBF_E_MEMSTREAM; break; }

        /* populate a command with variadic arguments */
        va_start(args, format);
        vfprintf(commandStream, commandFormat, args);

        /* update command data from buffer */
        fclose(commandStream);

        /* consume va_args used to populate command */
        for (   /* no condition */;
                NULL != (commandFormat = strpbrk(commandFormat, "%"));
                commandFormat++
            ) va_arg(args, int);

        /* run the populated command */
        pipe = popen(command, "r");
        if (NULL == pipe) { result = RPBF_E_POPEN; break; }

        /* scan the pipe output into the remaining variadic arguments */
        while (NULL != (outputFormat = strtok(NULL, " ")))
        {
            fscanf(pipe, outputFormat, va_arg(args, void * ));
        }
    } while (0);

    /* release resources, GOTO-style */
    switch (result)
    {
        default:
        case RPBF_E_NONE:       result = pclose(pipe);
        case RPBF_E_POPEN:      va_end(args);
                                free(command);
        case RPBF_E_MEMSTREAM:  free(formatCopy);
        case RPBF_E_MALLOC:     break; /* do nothing */
    };

    return (result);
}

