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
  // Get the last reading
  scale.prevRead = scale.lastRead;
  scale.lastRead = scale.readScale();

  // Check if the material characteristics has changed
  if(scale.currentStatus.filamentMaterialChanged) {
    scale.showInfo();
    scale.currentStatus.filamentMaterialChanged = false;
  }

  // Check if the status has changed
  if(scale.currentStatus.weightStatusChangedShown) {
    scale.showStat();
    scale.currentStatus.weightStatusChangedShown = false;
  }

  // Check for command availability for parsing
  if(Serial.available() > 0) {
    parseCommand(Serial.readString());
  }

  digitalWrite(READING_PIN, HIGH); // LED Enable

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

  // =========================================================
  // Parameters settings
  // =========================================================

  // Set PLA material and recalculate the material characteristics
  // Flag is set to display an update nesxt loop cycle
  if(commandString.equals(SET_PLA)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_SET << "'" << commandString << "'" << endl;
#endif
    scale.materialID = PLA;
    scale.currentStatus.filamentMaterialChanged = true;
    scale.calcMaterialCharacteristics();
  }
  // Set ABS material and recalculate the material characteristics
  // Flag is set to display an update nesxt loop cycle
  else if(commandString.equals(SET_ABS)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_SET << "'" << commandString << "'" << endl;
#endif
    scale.materialID = ABS;
    scale.currentStatus.filamentMaterialChanged = true;
    scale.calcMaterialCharacteristics();
  }
  // Set 1.75 mm filament diameter and recalculate the material characteristics
  // Flag is set to display an update nesxt loop cycle
  else if(commandString.equals(SET_175)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_SET << "'" << commandString << "'" << endl;
#endif
    scale.diameterID = DIAM_175;
    scale.currentStatus.filamentMaterialChanged = true;
    scale.calcMaterialCharacteristics();
  }
  // Set 3.00 mm filament diameter and recalculate the material characteristics
  // Flag is set to display an update nesxt loop cycle
  else if(commandString.equals(SET_300)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_SET << "'" << commandString << "'" << endl;
#endif
    scale.diameterID = DIAM_300;
    scale.currentStatus.filamentMaterialChanged = true;
    scale.calcMaterialCharacteristics();
  }
  // Set 1kg filament spool and recalculate the material characteristics
  // Flag is set to display an update nesxt loop cycle
  else if(commandString.equals(SET_1KG)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_SET << "'" << commandString << "'" << endl;
#endif
    scale.wID = ROLL1KG;
    scale.currentStatus.filamentMaterialChanged = true;
    scale.calcMaterialCharacteristics();
  }
  // Set 2kg filament spool and recalculate the material characteristics
  // Flag is set to display an update nesxt loop cycle
  else if(commandString.equals(SET_2KG)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_SET << "'" << commandString << "'" << endl;
#endif
    scale.wID = ROLL2KG;
    scale.currentStatus.filamentMaterialChanged = true;
    scale.calcMaterialCharacteristics();
  }

  // =========================================================
  // Change current functional status
  // =========================================================

  // Send a reset command and restore the parameters to the defaults
  // The tare is not recalculated to avoid wrong measure (if the spool
  // is already on the scale platform
  // This command had mandatory executi9on and ignore the previous state
  // The flag is set to show an update nextg loop cycle
  else if(commandString.equals(S_RESET)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
#endif
    scale.setDefaults();
    scale.stat = SYS_READY;
    scale.statID = STAT_READY;
    scale.currentStatus.weightStatusChangedShown = true;
  }
  // Send a load command status setting
  // Should be executed after the filament roll has been set 
  // and placed on the scale base or after a reset command
  // The flag is set to show an update nextg loop cycle
  else if(commandString.equals(S_LOAD)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
#endif
    scale.stat = SYS_LOAD;
    scale.statID = STAT_LOAD;
    scale.initialWeight = scale.lastRead;
    scale.currentStatus.weightStatusChangedShown = true;
  }
  // Send a run command status setting
  // Should be sent when a print job is started
  else if(commandString.equals(S_RUN)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
#endif
    scale.stat = SYS_RUN;
    scale.statID = STAT_RUN;
    scale.currentStatus.weightStatusChangedShown = true;
  }
  // Send a default command status setting
  // Should be used to reset the system to the default values 
  // of the material without changing any setting in the weight
  // tare and calculations but the current status is not changed.
  // Use this commmand to reset the material to the internal conditions
  else if(commandString.equals(S_DEFAULT)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_STATUS << "'" << commandString << "'" << endl;
#endif
    scale.setDefaults();
    scale.currentStatus.filamentMaterialChanged = true;
  }  

  // =========================================================
  // Informative commands
  // =========================================================

  else if(commandString.equals(SHOW_SETTINGS)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
#endif
  }
  else if(commandString.equals(SHOW_STATUS)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
#endif
  }
  else if(commandString.equals(SHOW_DUMP)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
#endif
  }
  else if(commandString.equals(SHOW_WEIGHT)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
#endif
  }

  // =========================================================
  // Motor control
  // =========================================================

#ifdef _USE_MOTOR
  else if(commandString.equals(MOTOR_FEED)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
      motor.feedExtruder(FEED_EXTRUDER_DELAY);
      motor.tleDiagnostic();
  }
  else if(commandString.equals(MOTOR_PULL)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
      motor.filamentLoad(FEED_EXTRUDER_DELAY);
      motor.tleDiagnostic();
  }
  else if(commandString.equals(MOTOR_STOP)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
#endif
  }
  else if(commandString.equals(MOTOR_FEED_CONT)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
    motor.filamentFeed(FEED_EXTRUDER_DELAY*10);
    motor.tleDiagnostic();
  }
  else if(commandString.equals(MOTOR_PULL_CONT)) {
    Serial << CMD_EXEC << "'" << commandString << "'" << endl;
    motor.filamentLoad(FEED_EXTRUDER_DELAY*10);
    motor.tleDiagnostic();
  }

  // =========================================================
  // Change behaviour mode
  // =========================================================

  else if(commandString.equals(MODE_AUTO)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_MODE << "'" << commandString << "'" << endl;
#endif
  }
  else if(commandString.equals(MODE_MANUAL)) {
#ifdef _DEBUG_COMMANDS
    Serial << CMD_MODE << "'" << commandString << "'" << endl;
#endif
  }
#endif
  // Not a valid command
  // This is the only command response that is shown also when
  // debug is not set
  else
    Serial << CMD_NOCMD << "'" << commandString << "'" << endl;
 }

