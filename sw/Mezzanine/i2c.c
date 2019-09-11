
/*
File:   i2c.c
Date:   2019-02-22
Author: Peter Lapets

Description:
This file contains functions used for I2C communication.  These functions 
are wrappers to calls to RunProcessByFormat(), which call their correspond-
ing scripts.  These scripts in turn are wrappers for programs from the
i2c-tools package.
*/

#include "i2c.h"

int I2cReadRegister( int bus, int addr, int reg, int * val )
{
    return (RunProcessByFormat(I2C_READ_REGISTER, bus, addr, reg, val));
}

