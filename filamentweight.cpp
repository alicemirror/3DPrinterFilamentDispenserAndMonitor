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
  // Used by two pass calibration only
  cal3Pass = 0;
  scaleCalibration = SCALE_CALIBRATION;
  isCalibrating = false;
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
  //! calculate the absolute delta as we don't know if the filament is pulled down or up
  //! respect the scale base
  float delta;

  // Save the previous reading  
  tempPrevRead = lastRead;
  // Read the new scale value
  lastRead = (scaleSensor.get_units(SCALE_SAMPLES) * - 1);

  // Manage the readings depending on the state
  switch(statID) {
    case STAT_RUN:
    // System running
    delta = abs(lastRead - tempPrevRead);
    if(delta >= MIN_EXTRUDER_TENSION) {
      // Extruder pull
      prevRead = tempPrevRead; // Keep old previous read
    }
    else {
      prevRead = lastRead; // Save new reading
    }
    break;
    
    case STAT_READY:
    // System after initialisation or reset
    prevRead = tempPrevRead;
    break;
    
    case STAT_LOAD:
    // Deduct the tare as filament has already been loaded
    // on the base
    lastRead -= rollTare;
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
  currentStatus.weightStatusChangedShown = false;
  currentStatus.weightExtruderTension = false;
  currentStatus.filamentMaterialChanged = false;

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
  return initialWeight - lastRead;
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
  Serial.println(TIT_MATERIAL);
  Serial.print(material);
  Serial.print(" ");
  Serial.print(diameter);
  Serial.print(" ");
  Serial.print(weight);
  Serial.print(" ");
  Serial.println(UNITS_KG);
  Serial.print("State: ");
  Serial.println(stat);
}

void FilamentWeight::showLoad(void) {
  Serial.println(TIT_LOAD);
  Serial.print(MSG_REMAINING);
  Serial.print(calcRemainingPerc(lastRead));
  Serial.println("%");
}

void FilamentWeight::showConfig(void) {
  Serial.println( TIT_CONFIG);
  // Show load status
  Serial.print(MSG_REMAINING);
  Serial.print(calcRemainingPerc(lastRead));
  Serial.println("%");
  // Show last and previous read values
  Serial.print("Last read: ");
  Serial.println(lastRead);
  Serial.print("Previous read: ");
  Serial.println(prevRead);
  // Show internal settings
  Serial.print("Calib.: ");
  Serial.print(scaleCalibration);
  Serial.println("units/gr");
}

float FilamentWeight::getWeight(void) {
  return scaleSensor.get_units(SCALE_SAMPLES) * -1;
}

void FilamentWeight::calibrate3Pass(void) {
  // First pass calibration
  if(cal3Pass == 0) {
    isCalibrating = true;
    scaleSensor.set_scale();
    scaleSensor.tare(); 
    cal3Pass++;
  }
  // Second pass calibration
  else if(cal3Pass == 1) {
    scaleCalibration = scaleSensor.get_units(CALIBRATION_STEPS);
    cal3Pass++;
  }
  // Third pass calibration
  else if(cal3Pass == 2) {
    scaleCalibration = scaleCalibration / knownWeight;
    isCalibrating = false;
    cal3Pass = 0;
    reset();
  }
}

void FilamentWeight::showStat(void) {
  float consumedGrams;
  consumedGrams = calcConsumedGrams();

  Serial.println(TIT_STAT);

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
      Serial.println(UNITS_MT);
    } // ... in meters
    else {
      Serial.print(valOptimizer(loadedCentimeters));
      Serial.println(UNITS_CM);
    } // ... in centimeters
  } // Units in length
}

