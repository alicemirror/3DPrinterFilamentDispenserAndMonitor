/**
 *  \file filamentweight.cpp
 *  \brief Class managing all the load cell sensor methods and calculations
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 beta
 */

#include "filamentweight.h"

void FilamentWeight::begin(void) {
  scale.begin(DOUT, CLK);
  // Initialise the scale with the model calibration factor then set the initial weight to 0
  scale.set_scale(SCALE_CALIBRATION);
  scale.tare();
  // Initialised the default values for the default filament type
  setDefaults();
  showInfo();
}

float FilamentWeight::readScale(void) {
  return (scale.get_units(SCALE_SAMPLES) * -1) - rollTare;
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
  Serial << endl << TIT_MATERIAL << endl;
  Serial << material << " " << diameter << " " << weight << " " << UNITS_KG << endl;
  Serial << stat << endl << endl;
}

void FilamentWeight::showLoad(void) {
  Serial << endl << TIT_LOAD << endl;
  Serial << material << " " << diameter << " " << weight << " " << UNITS_KG << endl;
  Serial << stat << " " << calcRemainingPerc(lastRead) << "%" << endl << endl;
}

void FilamentWeight::showConfig(void) {
  Serial << endl << TIT_CONFIG << endl;
  // Show load status
  Serial << FILAMENT_ROLL<< material << endl << diameter << " " << weight << " " << UNITS_KG << endl;
  Serial << stat << " " << calcRemainingPerc(lastRead) << "%" << endl;
  // Show last and previous read values
  Serial << "Last read: " << lastRead << " Previous read:" << prevRead << endl;
  // Show internal settings
  Serial << "Scale calibration: " << SCALE_CALIBRATION << UNITS_GR << " resolution: " << SCALE_RESOLUTION << UNITS_GR << " readings: " << SCALE_SAMPLES << endl;
  Serial << "Spool weight: " << rollTare << UNITS_GR << endl;
  if(filamentUnits == _GR) {
    Serial << "Units in: " << UNITS_GR << endl;
  }
  else {
    Serial << "Units in: " << UNITS_GR << endl;
  }
}

float FilamentWeight::getWeight(void) {
  return lastRead * -1;
}

void FilamentWeight::showStat(void) {
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


