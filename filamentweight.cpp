/**
 *  \file filamentweight.cpp
 *  \brief Class managing all the load cell sensor methods and calculations
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 Release Candidate
 *  Licensed under GNU LGPL 3.0
 */

#include "filamentweight.h"

void FilamentWeight::begin(void) {
  scaleSensor.begin(DOUT, CLK);
  // Assign the LED pint number and initialize the output  
  ledPin = 12;
  pinMode(ledPin, OUTPUT);   // LED reading signal
  scaleCalibration = SCALE_CALIBRATION;
  // Initialise the scale with the model calibration factor then set the initial weight to 0
  scaleSensor.set_scale(scaleCalibration);
  scaleSensor.tare();
  // Initialised the default values for the default filament type
  setDefaults();
  showInfo();
}

void FilamentWeight::reset(void) {
  scaleSensor.set_scale(scaleCalibration);
  scaleSensor.tare();
  setDefaults();
}

void FilamentWeight::readScale(void) {
  float tempPrevRead;
  //! calculate the absolute delta as we don't know 
  //! if the filament is pulled down or up
  //! respect the scale base
  float delta;

  // Save the previous reading  
  tempPrevRead = lastRead;

  prevRead = lastRead; // ***
  
  // Read the new scale value
  lastRead = (scaleSensor.get_units(SCALE_SAMPLES) * - 1);

  // Manage the readings depending on the state
  switch(statID) {
    case STAT_RUN:
    // System running
    delta = abs(lastRead - prevRead);

//    Serial.print("RUN delta = ");
//    Serial.print(delta);
//    Serial.print(" initialWeight = ");
//    Serial.print(initialWeight);
//    Serial.print(" lastRead = ");
//    Serial.print(lastRead);
//    Serial.print(" prevRead = ");
//    Serial.print(prevRead);
//    Serial.print(" tempPrevRead = ");
//    Serial.println(tempPrevRead);
    
    if(delta >= MIN_EXTRUDER_TENSION) {
      // Extruder pull
      currentStatus.filamentNeededFromExtruder = true;
    }
    else {
      currentStatus.filamentNeededFromExtruder = false;
    }
    break;
    
    case STAT_READY:
    // System after initialisation or reset
    prevRead = tempPrevRead;
    break;
    
    case STAT_LOAD:
    if( (tempPrevRead - lastRead) > MAX_DELTA_WEIGHT_IN_RANGE) {
      lastRead = tempPrevRead;
    } // reading invalid
    else {
      prevRead = lastRead;
     } // reading valid
    break;
    
    case STAT_NONE:
    // System not initialiased
    lastRead = prevRead = 0;
    break;
  }
}

void FilamentWeight::setDefaults(void) {
  // Initializes the parameters status
  currentStatus.weightStatus = STATUS_RESET;

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

  // Update the material IDs
  calcMaterialCharacteristics();
}

void FilamentWeight::calcMaterialCharacteristics(void) {
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

#ifdef _USE_MOTOR
  // Add the weight of the motor group to the tare
  rollTare += MOTOR_WEIGHT;
#endif

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

float FilamentWeight::calcConsumedCentimeters(void) {
  return calcGgramsToCentimeters(calcConsumedGrams());
}

float FilamentWeight::calcConsumedGrams(void) {
  return initialWeight - (lastRead - rollTare);
}

float FilamentWeight::valOptimizer(float value) {
  int optimizer;
  // Reduce the precison of a factor 10 (0.01) according with the physical precison of the readings
  optimizer = int(value * 10);
  return optimizer / 10;
}

float FilamentWeight::calcGgramsToCentimeters(float w) {
  return w / gr1cm;
}

float FilamentWeight::calcRemainingPerc(float w) {
  return w * 100 / rollWeight;
}

void FilamentWeight::showInfo(void) {
  Serial.print(material);
  Serial.print("\t");
  Serial.print(diameter);
  Serial.print("\t");
  Serial.print(weight);
  Serial.print(" ");
  Serial.println(UNITS_KG);
  Serial.print("State: ");
  Serial.println(stat);
  Serial.println("");
}

void FilamentWeight::showLoad(void) {
  int netWeight = lastRead - rollTare;

  // until filament has not been loaded
  // no status value should be returned
  if(statID < STAT_LOAD) {
    Serial.println("--");
  } else {
    Serial.print(MSG_REMAINING);
    Serial.print(netWeight);
    Serial.print(" ");
    Serial.print(UNITS_GR);
    Serial.print("\t");
    Serial.print(valOptimizer(calcGgramsToCentimeters(netWeight)/100));
    Serial.print(" ");
    Serial.print(UNITS_MT);
    Serial.print(" (");
    Serial.print(calcRemainingPerc(netWeight));
    Serial.println("%)\n");
  }
}

void FilamentWeight::showConfig(void) {
    int netWeight = lastRead - rollTare;

  // Show load status
  Serial.print(MSG_REMAINING);
  Serial.print(calcRemainingPerc(netWeight));
  Serial.println("%");
  // Show last and previous read values
  Serial.print("Last read: ");
  Serial.println(netWeight);
  Serial.print("Previous read: ");
  Serial.println(prevRead - rollTare);
  // Show internal settings
  Serial.print("Calib.: ");
  Serial.print(scaleCalibration);
  Serial.println("units/gr");
}

float FilamentWeight::getWeight(void) {
  return scaleSensor.get_units(SCALE_SAMPLES) * -1;
}

void FilamentWeight::showStat(void) {
  float consumedGrams;

  // If initialWeight is 0 run mode has not yet started
  if(initialWeight != 0 )
    consumedGrams = abs(calcConsumedGrams());
  else
    consumedGrams = 0;

  // Avoid negative values due to floating values (mostly vibrations)
  if( (consumedGrams < 0) || (consumedGrams < SCALE_RESOLUTION) )
    consumedGrams = lastConsumedGrams;
  else
    lastConsumedGrams = consumedGrams;

  // Used material
  Serial.print(MSG_USED);

  // Select the representation uinit
  if(filamentUnits == _GR) {
    Serial.print(valOptimizer(consumedGrams));
    Serial.print(" ");
    Serial.println(UNITS_GR);
  } // Units in weight
  else {
    // Show the length in centimeters until one meter then show in meters
    float loadedCentimeters;
    // Convert the weight in length
    loadedCentimeters = calcGgramsToCentimeters(consumedGrams);
    // Select the length representation
    if(loadedCentimeters > CENTIMETERS_PER_METER) {
      Serial.print(loadedCentimeters/CENTIMETERS_PER_METER);
      Serial.print(" ");
      Serial.println(UNITS_MT);
    } // ... in meters
    else {
      Serial.print(valOptimizer(loadedCentimeters));
      Serial.print(" ");
      Serial.println(UNITS_CM);
    } // ... in centimeters
  } // Units in length
  Serial.println("");
}

void FilamentWeight::flashLED(void) {
  int j;
  
  for(j = 0; j < 20; j++) {
//    Serial.println("flashLED()");
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}

