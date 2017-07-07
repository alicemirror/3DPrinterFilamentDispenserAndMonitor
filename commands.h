/**
 *  \file commands.cpp
 *  \brief Serial commands to control the behaviour of the system.
 *  All the commands are case sensitive
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 Release Candidate
 *  Licensed under GNU LGPL 3.0
 */

#ifndef _COMMANDS
#define _COMMANDS

#undef _DEBUG_COMMANDS

// Execution notification, debug only
#define CMD_EXEC "executing "
#define CMD_NOCMD "unknown "
#define CMD_SET "setting "
#define CMD_MODE "set mode to "
#define CMD_UNITS "set units to "
#define CMD_STATUS "set status to "
#define CMD_WEIGHT "Weight "
#define CMD_WRONGCMD "wrong value "
#define CMD_EXTRUDERPULL "WARNING!!!"

// Filament setup
#define SET_PLA "PLA"
#define SET_ABS "ABS"
#define SET_175 "1.75"
#define SET_300 "3.00"
#define SET_1KG "1kg"
#define SET_2KG "2kg"

// Units settings
#define SET_WEIGHT "gr"
#define SET_CENTIMETERS "cm"

// Calibration process
#define MANUAL_CALIBRATION "calibration"

// Status change
#define S_RESET "reset"        // Reset the system with the current filament setup
#define S_LOAD "load"          // Filament roll has been loaded
#define S_RUN "run"            // Print job is running
#define S_DEFAULT "default"    // Reset the system with the default settings

// Running mode
#define MODE_AUTO "auto"        // Run in automatic mode
#define MODE_MANUAL "man"    // Run in manual mode

// Motor control
#ifdef _USE_MOTOR
#define MOTOR_FEED "feed"       // Feed a length unit
#define MOTOR_PULL "pull"       // Pull back a lenght unit
#define MOTOR_STOP "stop"       // Pull back a lenght unit
#define MOTOR_FEED_CONT "feedc"    // Feed continuopusly
#define MOTOR_PULL_CONT "pullc"    // Pull back continuously
#endif

// Information commands
#define SHOW_INFO "info"          // Shows roll current info
#define SHOW_STATUS "stat"      // Shows weight status values
#define SHOW_DUMP "conf"        // Dump the current settings
#define SHOW_WEIGHT "weight"      // Show the current read weight

#endif
