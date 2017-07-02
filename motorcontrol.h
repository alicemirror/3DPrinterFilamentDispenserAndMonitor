/**
 *  \file motorcontrol.h
 *  \brief Arduino class to control Infineon's DC Motor Control Shield with TLE94112
 *  for the 3D printewr filament dispenser
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 beta
 */

#include <TLE94112.h>
#include "motor.h"

//!  \brief  Class that represents a TLE94112 controlling the filament motor
class MotorControl {
  public:
    //! \brief Initialization and motor settings
    void begin(void);

    //! \brief stop the motor control
    void end(void);
  
    /**
     * \brief Accelerates to the regime speed for filament release then 
     * keep the regime speed for the needed number of milliseconds
     * to release a lenght of filament then decelerate until motor stop
     * 
     * \param duration the numer of ms to feed at the regime speed
     */
    void feedExtruder(long duration);
  
    /**
     * \brief Accelerates to the regime speed for filament release then 
     * keep the regime speed for the needed number of milliseconds
     * to release a lenght of filament then decelerate until motor stop
     * 
     * \param duration the numer of ms to feed at the regime speed
     */
    void filamentFeed(long duration);

    /**
     * \brief Accelerates to the regime speed for filament load then 
     * keep the regime speed for the needed number of milliseconds
     * to release a lenght of filament then decelerate until motor stop
     * 
     * \param duration the numer of ms to feed at the regime speed
     */
    void filamentLoad(long duration);

    /**
     * \brief Accelerates to the regime speed then 
     * keep the regime speed for the needed number of milliseconds
     * then decelerate until motor stop
     * 
     * \param minDC mnimumn duty cycle value
     * \param maxDC maximum duty cycle value
     * \param accdelay pause ms during the acceleration/deceleration loops
     * \param duration numer of ms at the regime speed
     */
    void motorRun(int minDC, int maxDC, int accdelay, long duration, int motorDirection);

    /**
     * \brief Set the motor in stop condition keeping the half bridges floating
     */
    void motorStop(void);

    /**
     * \brief Brake the motor keelping the half bridges high
     */
    void motorBrake();

    /**
     * Check if an error occured.
     * 
     * \note This method should be used for test the error condition only as it does not
     * reset the error condition. For this scoper use tleDiagnostic() method instead.
     * 
     * \return true if tehre is an error
     */
    boolean tleCheckDiagnostic(void);

    /**
     * Check the error condition and detedt the kind of error (if any) then reset it
     * 
     * \return The error string
     * \todo Check the harfbridge generating the specific error 
     */
    void tleDiagnostic(void);

};


