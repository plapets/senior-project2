
#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

/*
File:   RobotControl.h
Date:   2019-04-30
Author: Peter Lapets

Description:
This file contains the API for controlling a UR3 robot via Modbus TCP/PI.
It reads to and writes from registers on a Modbus server hosted by the robot
controller.
*/

#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include <modbus.h>

#include "../DEBUG_PRINT.h"

#define MODBUS_SERVER_IP        "192.168.10.10"
#define MODBUS_SERVER_PORT      "502"

#define RCTL_E_NO_ERROR         ( 0)
#define RCTL_E_MODBUS_READ      (-1)
#define RCTL_E_MODBUS_WRITE     (-2)
#define RCTL_E_MODBUS_CONTEXT   (-3)
#define RCTL_E_MODBUS_CONNECT   (-4)
#define RCTL_E_MODBUS_CLAIM_R   (-5)
#define RCTL_E_MODBUS_CLAIM_W   (-6)
#define RCTL_E_MODBUS_RELEASE   (-7)

#define RCTL_USLEEP_PERIOD      10000

/* UR3 general purpose addresses reside in 128-255 */
#define RCTL_ADDRESS_MUTEX          128

#define RCTL_ADDRESS_COMMAND        130
#define RCTL_ADDRESS_TARGET_POSE    132
#define RCTL_ADDRESS_CURRENT_POSE   135
#define RCTL_ADDRESS_ESTOP          141

#define RCTL_COMMAND_WAIT      0
#define RCTL_COMMAND_HOME      1
#define RCTL_COMMAND_PHOTO     2
#define RCTL_COMMAND_HERE      3
#define RCTL_COMMAND_TEMP      4
#define RCTL_COMMAND_FLIP      5
#define RCTL_COMMAND_DEPOSIT   6

struct ROBOT_POSE_3D
{
    int16_t x;
    int16_t y;
    int16_t z;
};

void    RobotControl_Init   ( void );
void    RobotControl_Refresh( void );
void    RobotControl_Shdn   ( void );

int     RobotControl_Home   ( void );
int     RobotControl_Photo  ( void );
int     RobotControl_Here   ( struct ROBOT_POSE_3D * targetPose );
int     RobotControl_Temp   ( struct ROBOT_POSE_3D * targetPose );
int     RobotControl_Flip   ( struct ROBOT_POSE_3D * targetPose );
int     RobotControl_Deposit( struct ROBOT_POSE_3D * targetPose );

void    RobotControl_EStop  ( void );

#endif /* ROBOT_CONTROL_H */

