/** 
 *  \file motorcontrol.cpp
 *  \brief This file defines functions and predefined instances from motorcontrol.h
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 beta
 */

#include "motorcontrol.h"

void MotorControl::begin(void) {
  // enable tle94112
  tle94112.begin();

  // Disale the unused half bridges
  #ifdef _HIGHCURRENT
    // High current mode, keep in use HB1&2, 3&4
    tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  #else
    // No High current mode use only HB1 & 2
    tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB5, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB6, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB7, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB8, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB9, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB10, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB11, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB12, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  #endif
}

void MotorControl::end(void) {
  tle94112.end();
}

void MotorControl::feedExtruder(long duration) {
  motorRun(DC_MIN_EXTRUDER, DC_MAX_EXTRUDER, ACCELERATION_DELAY, duration, DIRECTION_FEED);
  // motorStop() can be used as an alternative
  motorBrake();
}

void MotorControl::filamentFeed(long duration) {
  motorRun(DC_MIN_MANUAL_FFED, DC_MAX_MANUAL_FFED, ACCELERATION_DELAY, duration, DIRECTION_FEED);
  // motorStop() can be used as an alternative
  motorBrake();
}

void MotorControl::filamentLoad(long duration) {
  motorRun(DC_MIN_MANUAL_LOAD, DC_MAX_MANUAL_LOAD, ACCELERATION_DELAY, duration, DIRECTION_LOAD);
  // motorStop() can be used as an alternative
  motorBrake();
}

void MotorControl::motorRun(int minDC, int maxDC, int accdelay, long duration, int motorDirection) {
  int j;

  // Check for the direction
  if(motorDirection == DIRECTION_FEED) {
#ifdef _HIGHCURRENT
    tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_PWM1);
    tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM1);
    tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_LOW, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_LOW, tle94112.TLE_NOPWM);
#else
    tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_PWM1);
    tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_LOW, tle94112.TLE_NOPWM);
#endif
  }
  else {
#ifdef _HIGHCURRENT
    tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_LOW, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_LOW, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_HIGH, tle94112.TLE_PWM1);
    tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_PWM1);
#else
    tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_LOW, tle94112.TLE_NOPWM);
    tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_PWM1);
#endif
  }

  // Acceleration loop  
  for(j = minDC; j <= maxDC; j++) {
    // Update the speed
    tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ200HZ, j);
  //Check for error
  if(tleCheckDiagnostic()) {
#ifdef _MOTORDEBUG
    Serial.print("from motorRun() acceleration loop, DC = ");
    Serial.println(j);
#endif
    tleDiagnostic();
  }
    delay(accdelay);
  }

  // Wait for the requeste number of ms at the regime speed
  tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ200HZ, maxDC);
  //Check for error
  if(tleCheckDiagnostic()) {
#ifdef _MOTORDEBUG
    Serial.println("from motorRun() regime speed running");
#endif
    tleDiagnostic();
  }
  delay(duration);

  // Deceleration loop  
  for(j = maxDC; j > minDC; j--) {
    // Update the speed
    tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ200HZ, j);
  //Check for error
  if(tleCheckDiagnostic()) {
#ifdef _MOTORDEBUG
    Serial.print("from motorRun() deceleration loop, DC = ");
    Serial.println(j);
#endif
    tleDiagnostic();
  }
    delay(accdelay);
  }
}

void MotorControl::motorStop(void) {
  // Stop the motor
  tle94112.configPWM(tle94112.TLE_PWM1, tle94112.TLE_FREQ200HZ, 0);
#ifdef _HIGHCURRENT
  tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
#else
  tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_FLOATING, tle94112.TLE_NOPWM);
#endif
  //Check for error
  if(tleCheckDiagnostic()) {
#ifdef _MOTORDEBUG
    Serial.println("from motorStop()");
#endif
    tleDiagnostic();
  }
}

void MotorControl::motorBrake(void) {
#ifdef _HIGHCURRENT
  // High current configuration, uses HB1&2 + 3&4
  tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB3, tle94112.TLE_HIGH, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB4, tle94112.TLE_HIGH, tle94112.TLE_NOPWM);
#else
  // No high current mode, use only HB1 & 2
  tle94112.configHB(tle94112.TLE_HB1, tle94112.TLE_HIGH, tle94112.TLE_NOPWM);
  tle94112.configHB(tle94112.TLE_HB2, tle94112.TLE_HIGH, tle94112.TLE_NOPWM);
#endif
//Check for error
  if(tleCheckDiagnostic()) {
#ifdef _MOTORDEBUG
    Serial.println("from motorBrake()");
#endif
    tleDiagnostic();
  }
}

boolean MotorControl:: tleCheckDiagnostic(void) {
  if(tle94112.getSysDiagnosis() == tle94112.TLE_STATUS_OK)
    return false;
  else
    return true;
}

void MotorControl::tleDiagnostic() {
  int diagnosis = tle94112.getSysDiagnosis();

  if(diagnosis == tle94112.TLE_STATUS_OK) {
    Serial.println(TLE_ERROR_HORBAR);
    Serial.println(TLE_NOERROR);
  } // No errors
  else {
    // Open load error can be ignored
    if(tle94112.getSysDiagnosis(tle94112.TLE_LOAD_ERROR)) {
#ifndef _IGNORE_OPENLOAD
      Serial.println(TLE_ERROR_HORBAR);
      Serial.println(TLE_ERROR_MSG);
      Serial.println(TLE_LOADERROR);
      Serial.println("");
#endif
    } // Open load error
    else {
      Serial.println(TLE_ERROR_HORBAR);
      Serial.println(TLE_ERROR_MSG);
      if(tle94112.getSysDiagnosis(tle94112.TLE_SPI_ERROR)) {
        Serial.println(TLE_SPIERROR);
      }
      if(tle94112.getSysDiagnosis(tle94112.TLE_UNDER_VOLTAGE)) {
        Serial.println(TLE_UNDERVOLTAGE);
      }
      if(tle94112.getSysDiagnosis(tle94112.TLE_OVER_VOLTAGE)) {
        Serial.println(TLE_OVERVOLTAGE);
      }
      if(tle94112.getSysDiagnosis(tle94112.TLE_POWER_ON_RESET)) {
        Serial.println(TLE_POWERONRESET);
      }
      if(tle94112.getSysDiagnosis(tle94112.TLE_TEMP_SHUTDOWN)) {
        Serial.println(TLE_TEMPSHUTDOWN);
      }
      if(tle94112.getSysDiagnosis(tle94112.TLE_TEMP_WARNING)) {
        Serial.println(TLE_TEMPWARNING);
      }
      Serial.println("");
    } // Any other error
    // Clear all possible error conditions        
    tle94112.clearErrors();
  } // Error condition
}




