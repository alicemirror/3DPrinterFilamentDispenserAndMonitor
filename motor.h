/* 
 *  \file motor.h
 *  \brief DC motor parameters for the Infineon DC motor controller Arduino shield
 *  TLE94112 from Infineon
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 beta
 */

#include <TLE94112.h>

// Duration in ms motor runs before reading for changes (sensor, serial)
#define MOTOR_PULSESHORT 100 
// Duration in ms motor runs before reading for new command while motor
// runs continuously
#define MOTOR_PULSELONG 500

// Weight reading (grams) delta to start the motor running
// requesting new filament released
#define FILAMENT_REQUEST 25

// For high current coupling half bridges together 1&2, 3&4
#undef _HIGHCURRENT
#undef _MOTORDEBUG
// Avoid too many openload error messages when starting acceleration
#define _IGNORE_OPENLOAD

// Duty cycles with PWM 200 Hz
#define DC_MIN_EXTRUDER 96        // Min duty cycle for acceleration on extruder request (auto feeding)
#define DC_MAX_EXTRUDER 160       // Max duty cycle for acceleration on extruder request (auto feeding)
#define DC_MIN_MANUAL_FFED 96     // Min duty cycle for acceleration on manual feeding
#define DC_MAX_MANUAL_FFED 255    // Max duty cycle for acceleration on manual feeding
#define DC_MIN_MANUAL_LOAD 128    // Min duty cycle for acceleration on manual loading
#define DC_MAX_MANUAL_LOAD 255    // Max duty cycle for acceleration on manual loading

#define INVERT_DIRECTION_DELAY 300  // Delay in ms when the motor should invert direction
#define ACCELERATION_DELAY 5 //25       // Delay between acceleration steps
#define FEED_EXTRUDER_DELAY 1500    // Delay ms for an Extruder feed unit (time related to filament feed length)

#define DIRECTION_FEED 1    // Motor rotates to release filament
#define DIRECTION_LOAD 2    // Motor rotates to load filament

#ifdef _HIGHCURRENT
#define TLE_ERROR_MSG "Errors occured in TLE94112 (High current mode)"
#else
#define TLE_ERROR_MSG "Errors occured in TLE94112"
#endif
#define TLE_NOERROR "No Error"
#define TLE_SPIERROR "SPI"
#define TLE_LOADERROR "Open load"
#define TLE_UNDERVOLTAGE "Under voltage"
#define TLE_OVERVOLTAGE "Over voltage"
#define TLE_POWERONRESET "Power on reset" 
#define TLE_TEMPSHUTDOWN "Temperature shutdown"
#define TLE_TEMPWARNING "Warning high temperature"
#define TLE_ERROR_HORBAR "********************************************"

