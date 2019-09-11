
/*
File:   RobotControl.c
Date:   2019-04-30
Author: Peter Lapets

Description:
This file implements an API for controlling a UR3 robot via Modbus TCP/PI.

Before calling any of these functions, you must call RobotControl_Init() to
connect to the robot.  If a connection cannot be made, the function will
abort().

You must call RobotControl_Refresh() periodically during long idle periods
to ensure that the server does not close the connection.

You may then call the following functions to control the robot:

    RobotControl_Home   ( void );
    RobotControl_Photo  ( void );
    RobotControl_Here   ( struct ROBOT_POSE_3D * targetPose );
    RobotControl_Temp   ( struct ROBOT_POSE_3D * targetPose );
    RobotControl_Flip   ( struct ROBOT_POSE_3D * targetPose );
    RobotControl_Deposit( struct ROBOT_POSE_3D * targetPose );

    RobotControl_EStop  ( void );

Coordinates are stored in ROBOT_POSE_3D structures in millimeters as signed
16-bit integers.  As the robot is ignorant of this fact, it is necessary
to convert from unsigned to signed 16-bit values in the program that runs
on the robot.

When you are finished controlling the robot, you must call
RobotControl_Shdn() to close the connection and free associated resources.
*/

#include "RobotControl.h"

static modbus_t * g_modbus = NULL;


static int RobotControl_modbusClaim( void )
{
    uint16_t busy;
    int nRegisters;
    int status = RCTL_E_NO_ERROR;
    int thisErr;

    do
    {
        nRegisters = modbus_read_registers( g_modbus,
                                            RCTL_ADDRESS_MUTEX,
                                            1,
                                            &busy);
        thisErr = errno;
        usleep(RCTL_USLEEP_PERIOD);
        
        if (1 != nRegisters)
        {
            status = RCTL_E_MODBUS_CLAIM_R;
            DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
            fprintf(G_SYSTEM_LOG, "(modbus error: %s)", modbus_strerror(thisErr));
            break;
        }
    } while (busy);
    
    if (1 == nRegisters)
    {
        nRegisters = modbus_write_register( g_modbus,
                                            RCTL_ADDRESS_MUTEX,
                                            1 );
                                            
        /* if everything up to this point ok */
        if (1 != nRegisters)
        {
            status = RCTL_E_MODBUS_CLAIM_W;
        }
    }
    
    return (status);
}

static int RobotControl_modbusRelease( void )
{
    int nRegisters = 0;
    int status = RCTL_E_MODBUS_RELEASE;

    nRegisters = modbus_write_register(g_modbus, RCTL_ADDRESS_MUTEX, 0);
    if (1 == nRegisters)
        status = RCTL_E_NO_ERROR;

    return (status);
}

/* must claim registers first! */
static int RobotControl_getCurrentPose( struct ROBOT_POSE_3D * currentPose )
{
    int nRegisters = 0;
    int status = RCTL_E_MODBUS_READ;

    nRegisters = modbus_read_registers( g_modbus,
                                        RCTL_ADDRESS_CURRENT_POSE,
                                        3,
                                        (uint16_t *) currentPose
                                        );

    if ((nRegisters * 2) == sizeof(struct ROBOT_POSE_3D))
        status = RCTL_E_NO_ERROR;

    return (status);
}

/* must claim registers first! */
static int RobotControl_setTargetPose( struct ROBOT_POSE_3D * targetPose )
{
    int nRegisters = 0;
    int status = RCTL_E_MODBUS_WRITE;

    nRegisters = modbus_write_registers(    g_modbus,
                                            RCTL_ADDRESS_TARGET_POSE,
                                            3,
                                            (uint16_t *) targetPose
                                        );

    if ((nRegisters * 2) == sizeof(struct ROBOT_POSE_3D))
        status = RCTL_E_NO_ERROR;

    return (status);
}

static int RobotControl_setCommand( int command )
{
    int nRegisters = 0;
    int status = RCTL_E_MODBUS_WRITE;

    nRegisters = modbus_write_register( g_modbus,
                                        RCTL_ADDRESS_COMMAND,
                                        command );

    if (1 == nRegisters)
        status = RCTL_E_NO_ERROR;

    return (status);
}

static int RobotControl_getCommand( uint16_t * command )
{
    int nRegisters = 0;
    int status = RCTL_E_MODBUS_READ;

    nRegisters = modbus_read_registers( g_modbus,
                                        RCTL_ADDRESS_COMMAND,
                                        1,
                                        command );

    if (1 == nRegisters)
        status = RCTL_E_NO_ERROR;

    return (status);
}

static int RobotControl_sendCommandInfo(uint16_t command,
                                        struct ROBOT_POSE_3D * target   )
{
    int status = RCTL_E_MODBUS_WRITE;
    uint16_t readCommand;
    
    do
    {
        /* claim registers      */
        status = RobotControl_modbusClaim();
        if (RCTL_E_NO_ERROR != status) break;

        /* write data           */
        status = RobotControl_setTargetPose(target);
        if (RCTL_E_NO_ERROR != status) break;

        status = RobotControl_setCommand(command);
        if (RCTL_E_NO_ERROR != status) break;

        /* release registers    */
        status = RobotControl_modbusRelease();
        if (RCTL_E_NO_ERROR != status) break;

        /* wait for command = 0 */
        do
        {
            RobotControl_modbusClaim();
            RobotControl_getCommand(&readCommand);
            RobotControl_modbusRelease();
        } while (RCTL_COMMAND_WAIT != readCommand);
        
        status = RCTL_E_NO_ERROR;
    } while (0);

    return (status);
}

static int RobotControl_sendCommandInfo_noPose( int command )
{
    struct ROBOT_POSE_3D currentPose;
    int status;

    do
    {
        status = RobotControl_modbusClaim();
        if (RCTL_E_NO_ERROR != status) break;

        status = RobotControl_getCurrentPose(&currentPose);
        if (RCTL_E_NO_ERROR != status) break;

        status = RobotControl_modbusRelease();
        if (RCTL_E_NO_ERROR != status) break;

        status = RobotControl_sendCommandInfo(command, &currentPose);
        if (RCTL_E_NO_ERROR != status) break;

        status = RCTL_E_NO_ERROR;
    } while (0);

    return (status);
}

void RobotControl_Init( void )
{
    struct timeval responseTimeout;
    
    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Initializing Modbus TCP PI.\n");
    DEBUG_PRINT_LEVEL_ENTER();
    
    g_modbus = modbus_new_tcp_pi(MODBUS_SERVER_IP, MODBUS_SERVER_PORT);
    DEBUG_PRINT (   G_SYSTEM_LOG,
                    "Creating libmodbus context....",
                    NULL == g_modbus,
                    FAILURE_FORBIDDEN
                );

    DEBUG_PRINT (   G_SYSTEM_LOG,
                    "Connecting to Modbus server...",
                    -1 == modbus_connect(g_modbus),
                    FAILURE_FORBIDDEN
                );
    fflush(G_SYSTEM_LOG);

    modbus_set_debug(g_modbus, TRUE);

    DEBUG_PRINT_LEVEL_EXIT();
}

void RobotControl_Refresh( void )
{
    uint16_t temp;
    int nRegisters;

    nRegisters = modbus_read_registers( g_modbus,
                                        RCTL_ADDRESS_MUTEX,
                                        1,
                                        &temp);

    /* if the read failed because the connection dropped, */
    if ((1 != nRegisters) && (5 == errno))
    {
        DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "RobotControl: UR3 dropped connection.  Reconnecting...\n");
        modbus_close(g_modbus);
        modbus_connect(g_modbus);
    }
}

void RobotControl_Shdn( void )
{
    modbus_close(g_modbus);
    modbus_free(g_modbus);
}

int RobotControl_Home( void )
{
    int status;
    
    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Moving robot to home position... ");
    status = RobotControl_sendCommandInfo_noPose(RCTL_COMMAND_HOME);
    fprintf(G_SYSTEM_LOG, "%s (%d)\n",  (RCTL_E_NO_ERROR == status)
                                        ? "succeeded."
                                        : "failed!",
                                        status );
    fflush(G_SYSTEM_LOG);

    return (status);
}

int RobotControl_Photo( void )
{
    int status;
    
    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "Moving robot to photo position... ");
    status = RobotControl_sendCommandInfo_noPose(RCTL_COMMAND_PHOTO);
    fprintf(G_SYSTEM_LOG, "%s (%d)\n",  (RCTL_E_NO_ERROR == status)
                                        ? "succeeded."
                                        : "failed!",
                                        status );
    fflush(G_SYSTEM_LOG);

    return (status);
}

int RobotControl_Here( struct ROBOT_POSE_3D * targetPose )
{
    int status;
    
    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
    fprintf(G_SYSTEM_LOG,
            "Moving robot to position at (%d, %d, %d)... ",
            targetPose->x, targetPose->y, targetPose->z );
    status = RobotControl_sendCommandInfo(RCTL_COMMAND_HERE, targetPose);
    fprintf(G_SYSTEM_LOG, "%s (%d)\n",  (RCTL_E_NO_ERROR == status)
                                        ? "succeeded."
                                        : "failed!",
                                        status );
    fflush(G_SYSTEM_LOG);

    return (status);
}

int RobotControl_Temp( struct ROBOT_POSE_3D * targetPose )
{
    int status;
    
    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
    fprintf(G_SYSTEM_LOG,
            "Moving robot to measure temperature at (%d, %d)... ",
            targetPose->x, targetPose->y);
    status = RobotControl_sendCommandInfo(RCTL_COMMAND_TEMP, targetPose);
    fprintf(G_SYSTEM_LOG, "%s (%d)\n",  (RCTL_E_NO_ERROR == status)
                                        ? "succeeded."
                                        : "failed!",
                                        status );
    fflush(G_SYSTEM_LOG);

    return (status);
}

int RobotControl_Flip( struct ROBOT_POSE_3D * targetPose )
{
    int status;
    
    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
    fprintf(G_SYSTEM_LOG,
            "Moving robot to flip patty at (%d, %d)... ",
            targetPose->x, targetPose->y );
    status = RobotControl_sendCommandInfo(RCTL_COMMAND_FLIP, targetPose);
    fprintf(G_SYSTEM_LOG, "%s (%d)\n",  (RCTL_E_NO_ERROR == status)
                                        ? "succeeded."
                                        : "failed!",
                                        status );
    fflush(G_SYSTEM_LOG);

    return (status);
}

int RobotControl_Deposit( struct ROBOT_POSE_3D * targetPose )
{
    int status;
    
    DEBUG_PRINT_LEVEL(G_SYSTEM_LOG, "");
    fprintf(G_SYSTEM_LOG,
            "Moving robot to deposit patty at (%d, %d)... ",
            targetPose->x, targetPose->y );
    status = RobotControl_sendCommandInfo(RCTL_COMMAND_DEPOSIT, targetPose);
    fprintf(G_SYSTEM_LOG, "%s (%d)\n",  (RCTL_E_NO_ERROR == status)
                                        ? "succeeded."
                                        : "failed!",
                                        status );
    fflush(G_SYSTEM_LOG);

    return (status);
}

void RobotControl_EStop( void )
{
    modbus_write_register( g_modbus, RCTL_ADDRESS_ESTOP, 1 );
}

