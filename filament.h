/**
 *  \file filament.h
 *  \brief Filament material parameters
 *  
 *  PLA
 *  Density: 1.25 g/cm^3
 *  Volume: 0.80 cm^3/g or 800 cm^3/kg
 *  1.75 mm filament length for 1 kg spool: ~ 330 meters
 *  3.00 mm filament length for 1 kg spool: ~ 110 meters
 *  
 *  ABS
 *  Density: 1.04 g/cm^3
 *  Volume: 0.96 cm^3/g or 960 cm^3/kg
 *  1.75 mm filament length for 1 kg spool: ~ 400 meters
 *  3.00 mm filament length for 1 kg spool: ~ 130 meters
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 Release Candidate
 *  Licensed under GNU LGPL 3.0
 */

#ifndef _FILAMENT
#define _FILAMENT

//! Define motor is using the automatic dispenser
#define _USE_MOTOR

#undef _DEBUG_FILAMENT

//! Application title shown on startup
#define APP_TITLE "Filament Control Ver. 1.0 RC"

#define MSG_USED "used: "
#define MSG_REMAINING "remain: "

// Material type IDs
#define PLA 0
#define ABS 1

// Diameter IDs
#define DIAM_175 0
#define DIAM_300 1

// Material filament IDs (calculated during initialisation)
#define PLA175 0
#define PLA300 1
#define ABS175 2
#define ABS300 3

// Roll tare (gr)
#define ROLL1KG_TARE 225.0
#define ROLL2KG_TARE 250.0 // Not real!!! Should be known

// Roll kind ID
#define ROLL1KG 0
#define ROLL2KG 1

#define SYS_READY "Ready"       // System ready
#define SYS_RUN "Running"   // Filament in use
#define SYS_LOAD "Load"         // Roll loaded
#define SYS_STARTED "Started"   // Application started

// Status codes
#define STAT_NONE 0
#define STAT_READY 1
#define STAT_RUN 2
#define STAT_LOAD 3

#define DIAMETER175 "1.75 mm"
#define DIAMETER300 "3.00 mm"

// How mamny centimeters in 1 meter (???)
#define CENTIMETERS_PER_METER 100

// Units
#define UNITS_GR "gr"
#define UNITS_CM "cm"
#define UNITS_MT "m"
#define UNITS_KG "Kg"
#define FILAMENT_ROLL "Roll"

#define PLA_MAT "PLA"
#define ABS_MAT "ABS"

//! Length in cm for 1 gr 1.75 mm PLA filament
#define PLA175_1GR_CM 33.0
// Weight in gr for 1 cm 1.75 mm PLA filament
#define PLA175_1CM_GR 0.03

//! Length in cm for 1 gr 3.00 mm PLA filament
#define PLA300_1GR_CM 11.0
// Weight in gr for 1 cm 3.00 mm PLA filament
#define PLA300_1CM_GR 0.09
 
//! Length in cm for 1 gr 1.75 mm ABS filament
#define ABS175_1GR_CM 40.0
//! Weight in gr for 1 cm 1.75 mm ABS filament
#define ABS175_1CM_GR 0.025

//! Length in cm for 1 gr 3.00 mm ABS filament
#define ABS300_1GR_CM 13.0
//! Weight in gr for 1 cm 3.00 mm ABS filament
#define ABS300_1CM_GR 0.076

//! 1Kg Roll spool average weight in gr (tare)
//! We assume that the plastic empty roll weight is
//! the same for any kind but this parameter should
//! be verified
#define ROLL_WEIGHT 195

//! Weight of the motor group part of the tare
#define MOTOR_WEIGHT 158.50

//! This value is calculated empirycally and is the zero weight for the
//! used 3D printed model
#define SCALE_CALIBRATION 434.50

//! Numer of reading steps used by the three pass manual calibration
#define CALIBRATION_STEPS 10

//! Minimum weight difference between two updates in grams
#define SCALE_RESOLUTION 1.50

//! Max previous-last delta value between two readings before
//! the current reading will be discarded due a noise variation
//! isntead a real weight change
#define MAX_DELTA_WEIGHT_IN_RANGE 10

//! Number of samples read every weight reading. 
//! Multiple samples reading gives more stability to the measure
#define SCALE_SAMPLES 10

//! Minimum number of grams variation between two reading too high
//! to be considered weight change (tension by the Extruder)
#define MIN_EXTRUDER_TENSION 100

//! Filament units IDs
#define _GR 1
#define _CM 2

//! LED pin on when reading
#define READING_PIN 13

#endif
