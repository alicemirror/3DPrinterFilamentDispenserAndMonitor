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

#include <HX711.h>
#include <Streaming.h>
#include "filament.h"
#ifdef _MOTOR
#include "motorcontrol.h"
#endif
#include "commands.h"

#define DOUT 5  // load sensor data pin
#define CLK 6   // load sensor clock pin

// Load sensor library initialisation
HX711 scale(DOUT, CLK);
#ifdef _MOTOR
MotorControl motor;
#endif

// filament diameter (descriptive)
String diameter;
// material type (descriptive)
String material;
// roll weight (descriptive)
String weight;
// system status
String stat;
// Status ID
int statID;
// grams for 1 cm material
float gr1cm;
// centimeters for 1 gr material
float length1gr;
// filament weight
float rollWeight;
// roll tare
float rollTare;
// Last read value from the cell
float lastRead;
// Previous read value from the cell
float prevRead;
// Initial read weight from last reset
float initialWeight;
// Last reliable value for consumed grams
float lastConsumedGrams;

// Units display flag. Decide if consume is in grams or cm
float filamentUnits;

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

  // Initialise the scale with the model calibration factor then set the initial weight to 0
  scale.set_scale(SCALE_CALIBRATION);
  scale.tare();

  // Set the dip switch pins
  pinMode(READING_PIN, OUTPUT);   // LED reading signal

  // Initialised the default values for the default filament type
  setDefaults();
  showInfo();

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
  prevRead = lastRead;
  lastRead = readScale();

  digitalWrite(READING_PIN, HIGH); // LED Enable

  // Check the current status of the system
  switch(statID) {
    case STAT_READY:
      // Set the initial weight to calculate the consumed material during a session
      initialWeight = lastRead;
      prevRead = lastRead;
      showLoad();
      break;
    case STAT_LOAD:
      showStat();
      break;
    case STAT_PRINTING:
      // Avoid fluctuations due the extruder tension
      if( (lastRead-prevRead) >= MIN_EXTRUDER_TENSION) {
        // Restore the previous reading
        lastRead = prevRead;
      }
      showStat();
      break;  
    default:
        showInfo();
        break;
    } // switch


  // Check the serial status and eventually read the received bytes
  if(Serial.available() > 0) {
    Serial << "received : " << Serial.readString() << endl;
  }
  
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

// ==============================================
//  SCALE METHODS
// ==============================================

/* 
 * Set the gloabl values depending on the material and filament size parameters
 * 
 * \param filament The filament ID
 * \param wID The rolld weight ID
 */
void setDefaults() {
  int wID;  // Filament weight ID
  int diameterID; // Roll diameter
  int materialID; // Roll material
  int filament; // Filament type

  stat = SYS_STARTED;
  statID = STAT_NONE;
  lastRead = 0;
  prevRead = 0;
  filamentUnits = _GR;  // default filament units
  lastConsumedGrams = 0;

  // You can change these initialisation values to set
  // your defaults
  diameterID = DIAM_175;    // Diameter
  materialID = PLA;         // Material
  wID = ROLL1KG;            // Weight

  // Calculate the material + diameter ID
  if(materialID == PLA) {
    filament = PLA175 + diameterID; 
  }
  else if(materialID == ABS) {
    filament = ABS175 + diameterID;
  }

  // Set the weight filament and tare for the supported
  // roll types
  switch(wID) {
    case ROLL1KG:
      rollWeight = 1000.0;
      rollTare = ROLL1KG_TARE;
      weight = "1";
      break;
    case ROLL2KG:
      rollWeight = 2000.0;
      rollTare = ROLL2KG_TARE;
      weight = "2";
      break;
  }

  // Set the parameters depending on the filament
  // characteristics. Strings for display updates.
  switch(filament) {
    case PLA175:
      diameter = DIAMETER175;
      material = PLA_MAT;
      gr1cm = PLA175_1CM_GR;
      length1gr = PLA175_1GR_CM;
      break;
    case PLA300:
      diameter = DIAMETER300;
      material = PLA_MAT;
      gr1cm = PLA300_1CM_GR;
      length1gr = PLA300_1GR_CM;
      break;
    case ABS175:
      diameter = DIAMETER175;
      material = ABS_MAT;
      gr1cm = ABS175_1CM_GR;
      length1gr = ABS175_1GR_CM;
      break;
    case ABS300:
      diameter = DIAMETER300;
      material = ABS_MAT;
      gr1cm = ABS175_1CM_GR;
      length1gr = ABS300_1GR_CM;
      break;
  }
}

/*
 * Exectues a scale series of readings without the plastic spool
 * (and any other extra weight that is not part of the measure)
 */
float readScale(void) {

  return (scale.get_units(SCALE_SAMPLES) * -1) - rollTare;
}

// ==============================================
//  Display to serial
// ==============================================

/* 
 * Show the filament information preset
 */
void showInfo() {
  
  Serial << material << " " << diameter << " " << weight << " " << UNITS_KG << endl;
  Serial << stat << endl;
}

/* 
 * Show the filament information after the roll has been loaded
 */
void showLoad() {
  
  Serial << material << " " << diameter << " " << weight << " " << UNITS_KG << endl;
  Serial << stat << " " << calcRemainingPerc(lastRead) << "%" << endl;
}

/* 
 * Show the filament status while the jobrinting is running
 */
void showStat() {
  float consumedGrams;
  consumedGrams = calcConsumedGrams();

  // Avoid negative values due to floating values (mostly vibrations)
  if(consumedGrams < 0)
    consumedGrams = lastConsumedGrams;
  else
    lastConsumedGrams = consumedGrams;
  
  Serial << calcGgramsToCentimeters(lastRead)/100 << " " << UNITS_MT << " " << calcRemainingPerc(lastRead) << "%" << endl;

  // Select the representation uinit
  if(filamentUnits == _GR) {
    Serial << stat << " " << valOptimizer(consumedGrams) << " " << UNITS_GR << "  ";
  } // Units in weight
  else {
    // Show the length in centimeters until one meter then show in meters
    float loadedCentimeters;
    // Convert the weight in length
    loadedCentimeters = calcGgramsToCentimeters(consumedGrams);
    // Select the length representation
    if(loadedCentimeters > CENTIMETERS_PER_METER) {
      Serial << stat << " " << loadedCentimeters/CENTIMETERS_PER_METER << " " << UNITS_MT << endl;
    } // ... in meters
    else {
      Serial << stat << " " << valOptimizer(loadedCentimeters) << " " << UNITS_CM << endl;
    } // ... in centimeters
  } // Units in length
}

// ==============================================
//  CALC METHODS
// ==============================================

/**
 * Optimizes the a floating value reducing the precision to one
 * decimal value
 * 
 * \param value The float value to be treated
 * \return The optimized float value
 */
 float valOptimizer(float value) {
    int optimizer;
    // Reduce the precison of a factor 10 (0.01) according with the physical precison of the readings
    optimizer = int(value * 10);
    return optimizer / 10;

 }

/** 
 *  Caclulate the centimeters for the corresponding weight
 *  
 *  The applied formula: (w - tare) / gr1cm
 *  
 *  \param w weight in grams
 *  \return the length in centimeters
*/
float calcGgramsToCentimeters(float w) {
 return w / gr1cm;
}

/*
 * Calculate the remaining weight percentage of filament
 * 
 * The applied forumal: (w - tare) * 100 / roll weight
 */
float calcRemainingPerc(float w) {
  return w * 100 / rollWeight;
 }

 /*
  * Calculate the consumed material after the roll loading in grams
  * 
  * the applied formula: initialWeight - lastRead - tare
  */
  float calcConsumedGrams() {
    return initialWeight - lastRead;
  }

 /*
  * Calculate the consumed material after the roll loading in centimeters
  */
  float calcConsumedCentimeters() {
    return calcGgramsToCentimeters(calcConsumedGrams());
  }


