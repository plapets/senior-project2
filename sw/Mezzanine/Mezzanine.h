
#ifndef MEZZANINE_H
#define MEZZANINE_H

/*
File:   Mezzanine.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file contains the API for controlling the mezzanine hardware.
*/

#include "../DEBUG_PRINT.h"

#include "gpio.h"
#include "i2c.h"

#define LOG_STREAM stderr


#define GPIO_HOTPLATE       (68)
#define GPIO_CONVEYOR_FWD   (66)
#define GPIO_CONVEYOR_REV   (67)
#define GPIO_START          (44)
#define GPIO_STOP           (45)

#define I2C_BUS                 2
#define I2C_SLAVE_ADDRESS       0x5a
#define I2C_REGISTER_T_AMBIENT  0x06
#define I2C_REGISTER_T_OBJECT   0x07
#define TEMPERATURE_ERROR_FLAG  0x8000
#define E_BAD_TEMPERATURE       -1


#define HOTPLATE_OFF        GPIO_LOW
#define HOTPLATE_ON         GPIO_HIGH

#define CONVEYOR_STOPPED    0
#define CONVEYOR_FORWARD    1
#define CONVEYOR_REVERSE    2

#define STOP_PRESSED        GPIO_HIGH
#define START_PRESSED       GPIO_LOW

int Mezzanine_HotplateInit( void );
int Mezzanine_ConveyorInit( void );
int Mezzanine_ButtonsInit( void );
int Mezzanine_TempSensorInit( void );

int Mezzanine_HotplateGetState( int * state );
int Mezzanine_HotplateSetState( int state );
int Mezzanine_ConveyorGetState( int * state );
int Mezzanine_ConveyorSetState( int state );
int Mezzanine_StopButtonGetState( int * state );
int Mezzanine_StartButtonGetState( int * state );
int Mezzanine_TempSensorGetTObject( double * celsius );

#endif /* MEZZANINE_H */

