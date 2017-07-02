/**

  3D Printer FIlament Dispense - USB to serial control

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
 Thie is the motorised verion of the 3D printer filament monitor based on the Infineon brushed
 DC motor controller Arduino shield.
 The software has been developed on the Infineon XMC110 mciro controller board Arduino compatible

 Licensed under GNU GPL 4.0
 Author:  Enrico Miglino. (c) 2017
          Balearic Dynamics SL balearicdynamic@gmail.com
 Updated sources on GitHUB: https://github.com/alicemirror/3DPrinterFilamentMonitor-LCD/

*/

#include <Streaming.h>
#include "filament.h"
#include "filamentweight.h"
#include "commands.h"
#ifdef _USE_MOTOR
#include "motorcontrol.h"
#endif


#ifdef _USE_MOTOR
MotorControl motor;
#endif

//! The weight control class
FilamentWeight scale;

// ==============================================
// Initialisation
// ==============================================
void setup() {
  // Serial is initialised at high speed. If your Arduino boards
  // loose characters of show unwanted/unexpected behavior
  // try with a lower communication speed
  Serial.begin(9600);

  // Print a message to the LCD.
  Serial << APP_TITLE << endl;
  Serial << CALIBRATING << endl;

  // Set the dip switch pins
  pinMode(READING_PIN, OUTPUT);   // LED reading signal
  // Initialize the weight class
  scale.begin();
#ifdef _USE_MOTOR
  // initialize the motor class
  motor.begin();  
#endif
}

// ==============================================
// Main loop
// ==============================================

/* 
 * The main loop role is execturing the service functions; display update, 
 * calculations, button checking
 * The scale reading is done at a specific frequence and is interrupt-driven
 */
void loop() {
//#ifdef _USE_MOTOR
//  // Feed Extruder
//  motor.feedExtruder(FEED_EXTRUDER_DELAY * 3);
//  //  tleDiagnostic();
//  delay(2000);
//  motor.filamentLoad(5000);
//  //  tleDiagnostic();
//  delay(2000);
//  motor.filamentFeed(5000);
//  //  tleDiagnostic();
//  delay(2000);
//#endif
  
  // Get the last reading
  scale.prevRead = scale.lastRead;
  scale.lastRead = scale.readScale();

  // Check for command availability for parsing
  if(Serial.available() > 0) {
    parseCommand(Serial.readString());
  }

  digitalWrite(READING_PIN, HIGH); // LED Enable

  // Check the current status of the system
  switch(scale.statID) {
    case STAT_READY:
      // Set the initial weight to calculate the consumed material during a session
      scale.initialWeight = scale.lastRead;
      scale.prevRead = scale.lastRead;
//      scale.showLoad();
      break;
    case STAT_LOAD:
//      scale.showStat();
      break;
    case STAT_PRINTING:
      // Avoid fluctuations due the extruder tension
      if( (scale.lastRead - scale.prevRead) >= MIN_EXTRUDER_TENSION) {
        // Restore the previous reading
        scale.lastRead = scale.prevRead;
      }
//      scale.showStat();
      break;  
    default:
//        scale.showInfo();
        break;
    } // switch

//  // Manage the status change button
//    if(digitalRead(SETZERO_PIN)) {
//      delay(100); // Barbarian debouncer
//        if(statID == STAT_NONE) {
//          statID = STAT_READY;
//          stat = SYS_READY;
//          return;
//        }
//        if(statID == STAT_READY) {
//          stat = SYS_LOAD;
//          statID = STAT_LOAD;
//          lcd.clear();
//          return;
//        } 
//        if(statID == STAT_LOAD) {
//          // Change from load to running mode
//          stat = SYS_PRINTING;
//          statID = STAT_PRINTING;
//          lcd.clear();
//          return;
//        } 
//        if(statID == STAT_PRINTING) {
//          // Change from load to running mode
//          stat = SYS_LOAD;
//          statID = STAT_LOAD;
//          // Set the initial weight to calculate the consumed material during a session
//          initialWeight = lastRead;
//          lcd.clear();
//          return;
//        }
//  }
//
//  // Manage the partial consumption button. Only when STAT_PRINTING
//  if(digitalRead(CHANGE_UNIT_PIN)) {
//    delay(250); // Barbarian debouncer
//    if(filamentUnits == _GR) {
//      filamentUnits = _CM;
//    }
//    else {
//      filamentUnits = _GR;
//    }
//  }

  digitalWrite(READING_PIN, LOW);
}


/**
 * Parse the command string and echo the executing message or command unknown error.
 * 
 * \param commandString the string coming from the serial
 */
 void parseCommand(String commandString) {
  boolean cmdOk = false;

  // Parameters settings
  if(commandString.equals(SET_PLA)) {
    Serial << CMD_SET << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SET_ABS)) {
    Serial << CMD_SET << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SET_175)) {
    Serial << CMD_SET << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SET_300)) {
    Serial << CMD_SET << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SET_1KG)) {
    Serial << CMD_SET << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SET_2KG)) {
    Serial << CMD_SET << "'" << commandString << "'" << endl;
  }
  // Change behaviour mode
  else if(commandString.equals(MODE_AUTO)) {
    Serial << CMD_MODE << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(MODE_MANUAL)) {
    Serial << CMD_MODE << "'" << commandString << "'" << endl;
  }
  // Change current functional status
  else if(commandString.equals(STATUS_RESET)) {
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(STATUS_LOAD)) {
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(STATUS_RUN)) {
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(STATUS_DEFAULT)) {
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
  }  
  // Informative commands
  else if(commandString.equals(SHOW_SETTINGS)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SHOW_STATUS)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SHOW_DUMP)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(SHOW_WEIGHT)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
#ifdef _USE_MOTOR
  // Motor control
  else if(commandString.equals(MOTOR_FEED)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(MOTOR_PULL)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(MOTOR_STOP)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(MOTOR_FEED_CONT)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
  else if(commandString.equals(MOTOR_PULL_CONT)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
  }
#endif
  // Not a valid command
  else
    Serial << CMD_NOCMD << "'" << commandString << "'" << endl;
 }

