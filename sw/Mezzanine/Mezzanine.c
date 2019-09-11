
/*
File:   Mezzanine.c
Date:   2019-04-30
Author: Peter Lapets

Description:
This file implements the API for controlling the mezzanine hardware.

The Mezzanine_...Init() functions initialize their respective hardware.
They have no return value, and will simply fail if the state of the
hardware is not correct (for example, if an NC button connected to
ground does not read as logic 0).

The Mezzanine_...SetState() functions accept values and the ...GetState()
functions accept pointers to data they modify.  Note that the functions
return error values, and you are responsible for checking that the
operation was successful.
*/

#include "Mezzanine.h"


int Mezzanine_HotplateInit( void )
{
    int warning = 0;

    DEBUG_PRINT_LEVEL(LOG_STREAM, "Initializing hotplate.\n");
    DEBUG_PRINT_LEVEL_ENTER();

    DEBUG_PRINT (   LOG_STREAM,
                    "Exporting pin...........",
                    warning = GpioExport(GPIO_HOTPLATE),
                    FAILURE_ALLOWED
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting pin direction...",
                    GpioSetDirection(GPIO_HOTPLATE, GPIO_OUTPUT_LOW),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting pin value.......",
                    GpioSetValue(GPIO_HOTPLATE, GPIO_LOW),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT_LEVEL_EXIT();

    return (warning);
}

int Mezzanine_ConveyorInit( void )
{
    int warning = 0;

    DEBUG_PRINT_LEVEL(LOG_STREAM, "Initializing conveyor.\n");
    DEBUG_PRINT_LEVEL_ENTER();

    DEBUG_PRINT (   LOG_STREAM,
                    "Exporting FWD pin...........",
                    warning = GpioExport(GPIO_CONVEYOR_FWD),
                    FAILURE_ALLOWED
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Exporting REV pin...........",
                    warning = GpioExport(GPIO_CONVEYOR_REV),
                    FAILURE_ALLOWED
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting FWD pin direction...",
                    GpioSetDirection(GPIO_CONVEYOR_FWD, GPIO_OUTPUT_LOW),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting REV pin direction...",
                    GpioSetDirection(GPIO_CONVEYOR_REV, GPIO_OUTPUT_LOW),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting FWD pin value.......",
                    GpioSetValue(GPIO_CONVEYOR_FWD, GPIO_LOW),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting REV pin value.......",
                    GpioSetValue(GPIO_CONVEYOR_REV, GPIO_LOW),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT_LEVEL_EXIT();

    return (warning);
}

int Mezzanine_ButtonsInit( void )
{
    int warning     = 0;
    int stopValue  = -1;
    int startValue = -1;

    DEBUG_PRINT_LEVEL(LOG_STREAM, "Initializing buttons.\n");
    DEBUG_PRINT_LEVEL_ENTER();

    DEBUG_PRINT (   LOG_STREAM,
                    "Exporting STOP pin...........",
                    warning = GpioExport(GPIO_STOP),
                    FAILURE_ALLOWED
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Exporting START pin..........",
                    warning = GpioExport(GPIO_START),
                    FAILURE_ALLOWED
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting STOP pin direction...",
                    GpioSetDirection(GPIO_STOP, GPIO_INPUT),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Setting START pin direction...",
                    GpioSetDirection(GPIO_START, GPIO_INPUT),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Getting STOP pin value.......",
                    GpioGetValue(GPIO_STOP, &stopValue),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Getting START pin value......",
                    GpioGetValue(GPIO_START, &startValue),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Checking STOP pin value.......",
                    !(STOP_PRESSED != stopValue),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   LOG_STREAM,
                    "Checking START pin value......",
                    !(START_PRESSED != startValue),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT_LEVEL_EXIT();

    return (warning);
}

int Mezzanine_TempSensorInit( void )
{
    int warning = 0;
    double celsius;

    DEBUG_PRINT_LEVEL(LOG_STREAM, "Initializing temperature sensor.\n"    );
    DEBUG_PRINT_LEVEL_ENTER();

    /* call once to wake sensor/put in known state*/
    Mezzanine_TempSensorGetTObject(&celsius);

    DEBUG_PRINT (   LOG_STREAM,
                    "Getting object temperature...",
                    Mezzanine_TempSensorGetTObject(&celsius),
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT_LEVEL_EXIT();

    return (warning);
}


int Mezzanine_HotplateGetState( int * state )
{
    return (GpioGetValue(GPIO_HOTPLATE, state));
}

int Mezzanine_HotplateSetState( int state )
{
    return (GpioSetValue(GPIO_HOTPLATE, state));
}

int Mezzanine_ConveyorGetState( int * state )
{
    int forward = 0;
    int reverse = 0;
    int result  = 0;
    
    result |= GpioGetValue(GPIO_CONVEYOR_FWD, &forward);
    result |= GpioGetValue(GPIO_CONVEYOR_FWD, &reverse);
    
    *state = forward + 2 * reverse;
    
    return (result);
}

int Mezzanine_ConveyorSetState( int state )
{
    int result = 0;
    switch (state)
    {
        case CONVEYOR_FORWARD:
            result |= GpioSetValue(GPIO_CONVEYOR_REV, GPIO_LOW);
            result |= GpioSetValue(GPIO_CONVEYOR_FWD, GPIO_HIGH);
            break;

        case CONVEYOR_REVERSE:
            result |= GpioSetValue(GPIO_CONVEYOR_FWD, GPIO_LOW);
            result |= GpioSetValue(GPIO_CONVEYOR_REV, GPIO_HIGH);
            break;

        case CONVEYOR_STOPPED:  /* fall through */
        default:
            result |= GpioSetValue(GPIO_CONVEYOR_FWD, GPIO_LOW);
            result |= GpioSetValue(GPIO_CONVEYOR_REV, GPIO_LOW);
    }
    
    return (result);
}

int Mezzanine_StopButtonGetState( int * state )
{
    return (GpioGetValue(GPIO_STOP, state));
}

int Mezzanine_StartButtonGetState( int * state )
{
    return (GpioGetValue(GPIO_START, state));
}

int Mezzanine_TempSensorGetTObject( double * celsius )
{
    int result;
    int rawTemp;

    result = I2cReadRegister(   I2C_BUS,
                                I2C_SLAVE_ADDRESS,
                                I2C_REGISTER_T_OBJECT,
                                &rawTemp
                            );

    if (0 == result)
    {
        if (0 == (rawTemp & TEMPERATURE_ERROR_FLAG))
            *celsius = (double) rawTemp / 50.0 - 273.15;
        else
            result = -1;
    }

    return (result);
}

