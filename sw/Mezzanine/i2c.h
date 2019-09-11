
#ifndef I2C_H
#define I2C_H

/*
File:   i2c.h
Date:   2019-02-22
Author: Peter Lapets

Description:
This file contains function declarations used for I2C communication.
*/

#include "RunProcessByFormat.h"


#define I2C_PATH "./scripts/i2c/"

#define I2C_READ_REGISTER (I2C_PATH "i2c-read-register %d %#x %#x:%x")

int I2cReadRegister( int bus, int addr, int reg, int * val );

#endif /* I2C_H */

