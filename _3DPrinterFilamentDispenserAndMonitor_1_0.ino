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

// Define infineon if you are using the XMC1100 board
#define _INFINEON
// Define motor is using the automatic dispenser
#define _MOTOR

#include <Streaming.h>
#include "filament.h"
#include "filamentweight.h"

#ifdef _MOTOR
#include "motorcontrol.h"
MotorControl motor;
#endif

FilamentWeight scale;

#undef DEBUG

// ==============================================
// Initialisation
// ==============================================
void setup() {
  // Serial is initialised at high speed. If your Arduino boards
  // loose characters of show unwanted/unexpected behavior
  // try with a lower communication speed
  Serial.begin(38400);
  
  // Print a message to the LCD.
  Serial << APP_TITLE << endl;
  Serial << CALIBRATING << endl;

  // Set the dip switch pins
  pinMode(READING_PIN, OUTPUT);   // LED reading signal

  scale.begin();

#ifdef _MOTOR
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
#ifdef _MOTOR
  // Feed Extruder
  motor.feedExtruder(FEED_EXTRUDER_DELAY * 3);
  //  tleDiagnostic();
  delay(2000);
  motor.filamentLoad(5000);
  //  tleDiagnostic();
  delay(2000);
  motor.filamentFeed(5000);
  //  tleDiagnostic();
  delay(2000);
#endif
  
  // Get the last reading
  scale.prevRead = scale.lastRead;
  scale.lastRead = scale.readScale();

  digitalWrite(READING_PIN, HIGH); // LED Enable

  // Check the current status of the system
  switch(scale.statID) {
    case STAT_READY:
      // Set the initial weight to calculate the consumed material during a session
      scale.initialWeight = scale.lastRead;
      scale.prevRead = scale.lastRead;
      scale.showLoad();
      break;
    case STAT_LOAD:
      scale.showStat();
      break;
    case STAT_PRINTING:
      // Avoid fluctuations due the extruder tension
      if( (scale.lastRead - scale.prevRead) >= MIN_EXTRUDER_TENSION) {
        // Restore the previous reading
        scale.lastRead = scale.prevRead;
      }
      scale.showStat();
      break;  
    default:
        scale.showInfo();
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
   * \brief Serial interrupt reading commands for parsing
   */
//  void serialEvent() {
//    // Get the byte
//    char inChar = (char)Serial.read();
//    // Queue the command buffer (String)
//    commandString += inChar;
//    // Check if the command is complete and set the flag
//    if(inChar == '\n') {
//      parseCommand();
//    }
//  }

