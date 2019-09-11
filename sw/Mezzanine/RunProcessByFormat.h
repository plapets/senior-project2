
#ifndef RUNPROCESSBYFORMAT_H
#define RUNPROCESSBYFORMAT_H

/*
File:   RunProcessByFormat.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file declares a function RunProcessByFormat, which both starts a
process with parameters and interprets the output based on a provided
format string.  This essentially allows for calling external scripts
as if they were functions.
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

enum RUN_PROCESS_BY_FORMAT_ERROR
{
    RPBF_E_NONE         =  0,
    RPBF_E_MALLOC       = -1,
    RPBF_E_MEMSTREAM    = -2,
    RPBF_E_POPEN        = -3
};

/*  RunProcessByFormat( format , ... )
 */
int RunProcessByFormat( const char * const runFormat, ... );

#endif /* RUNPROCESSBYFORMAT_H */

