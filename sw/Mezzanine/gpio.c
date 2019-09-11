
/*
File:   gpio.c
Date:   2019-02-22
Author: Peter Lapets

Description:
This file contains functions used for manipulation of general purpose I/O
as exposed by the /sys/class/gpio interface.  These are mostly wrappers for
RunProcessByFormat() which call their corresponding scripts.
*/

#include "gpio.h"


int GpioExport( int gpioNumber )
{
    return (RunProcessByFormat(GPIO_EXPORT, gpioNumber));
}

int GpioUnexport( int gpioNumber )
{
    return (RunProcessByFormat(GPIO_UNEXPORT, gpioNumber));
}

int GpioSetDirection( int gpioNumber, char * direction )
{
    return (RunProcessByFormat(GPIO_SET_DIRECTION, gpioNumber, direction));
}

int GpioGetValue( int gpioNumber, int * value )
{
    return (RunProcessByFormat(GPIO_GET_VALUE, gpioNumber, value));
}

int GpioSetValue( int gpioNumber, int value )
{
    return (RunProcessByFormat(GPIO_SET_VALUE, gpioNumber, value));
}

