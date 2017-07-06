/**
 *  \file filamentweight.h
 *  \brief Class managing all the load cell sensor methods and calculations
 *  
 *  \author Enrico Miglino <balearicdynamics@gmail.com> \n
 *  Balearic Dynamics sl <www.balearicdynamics.com> SPAIN
 *  \date July 2017
 *  \version 1.0 Release Candidate
 *  Licensed under GNU LGPL 3.0
 */

#ifndef _FILAMENTWEIGHT
#define _FILAMENTWEIGHT

#include <HX711.h>
#include "filament.h"
#include "commands.h"

#define DOUT 3  // load sensor data pin
#define CLK 4   // load sensor clock pin

#define STATUS_RESET 0      ///< After initialisation or reset
#define STATUS_READY 1      ///< System ready
#define STATUS_PRINTING 2   ///< Filament in use
#define STATUS_LOAD 3       ///< Roll loaded
#define STATS_STARTED 4     ///< Application started

//! Status structure varoab;es amd flags
struct process {
  //! The status of the entire process
  int weightStatus;   
  //! New status change has been shown
  boolean weightStatusChangedShown;
  //! Material characteristics flag indicator
  boolean filamentMaterialChanged;
  //! Extruder request based on the weight change due to the
  //! filament tension
  boolean weightExtruderTension;
};

/**
 * Class managing the load sensor
 */
class FilamentWeight {

  public:
    //! Application status
    process currentStatus;  
    int wID;            ///< Filament weight ID
    int diameterID;     ///< Roll diameter
    int materialID;     ///< Roll material
    int filament;       ///< Filament type
    //! Last read value from the cell
    float lastRead;
    //! Previous read value from the cell
    float prevRead;

    //! Sensor library instance
    HX711 scaleSensor;

    //! Pass number for the 2 pass calibration method
    int cal3Pass;
    //! The scale calibration value
    //! If is hardcoded on startup but can be further updated with the
    //! calibrate command
    float scaleCalibration;
    //! The calibration flag.
    //! This flag is set until the calibration process does not exit
    //! or is stopped by another command
    boolean isCalibrating;
    //! Calibration known weight in grams
    float knownWeight;

    /**
     * Initializes the sensor library and the initial default setup
     */
    void begin(void);

    /**
     * Full reset the systemn and reinitialize the default values.
     * Should be used when the filament roll is changed during a job
     */
    void reset(void);

    /** 
     * Set the gloabl values depending on the material and filament size parameters
     */
    void setDefaults(void);

    /**
     * Exectues a scale series of readings without the plastic spool
     * (and any other extra weight that is not part of the measure)
     */
    void readScale(void);

   /**
    * Calculate the consumed material after the roll loading in centimeters
    * 
    * `return the lenght in centimeters based on the weight
    */
    float calcConsumedCentimeters(void);

   /**
    * Calculate the consumed material after the roll loading in grams
    * with the formula: initialWeight - lastRead - tare
    * 
    * \return the weight in grams
    */
    float calcConsumedGrams(void);

    /**
     * Optimizes the a floating value reducing the precision to one
     * decimal value
     * 
     * \param value The float value to be treated
     * \return The optimized float value
     */
     float valOptimizer(float value);
    
    /** 
     *  Caclulate the centimeters for the corresponding weight
     *  
     *  The applied formula: (w - tare) / gr1cm
     *  
     *  \param w weight in grams
     *  \return the length in centimeters
    */
    float calcGgramsToCentimeters(float w);
    
    /**
     * Calculate the remaining weight percentage of filament
     * 
     * The applied forumal: (w - tare) * 100 / roll weight
     * 
     *  \param w weight in grams
     *  \return the remaining weight in percentage
     */
    float calcRemainingPerc(float w);

    /** 
     * \brief Show the filament information preset
     */
    void showInfo(void);
    
    /** 
     * \brief Show the filament information after the roll has been loaded
     */
    void showLoad(void);
    
    /** 
     * Show the filament status while the job is running
     */
    void showStat(void);

    /**
     * Show the configuration and system settings
     */
     void showConfig(void);

     /**
      * Return the last value read from the sensor
      */
     float getWeight(void);

    /**
     * Three-pass calibration to get the sdcale sensor calibration value.
     * 
     * For setup use only then the calibration value should be
     * set used by SCALE_CALIBRATION in filament.h
     * 
     * First pass: the scale sensor is initially set without calibration
     * value\n
     * Second pass: a known weight is placed on the scale and the absolute
     * units are read by the sensor with a 10 readings steps\n
     * Third pass: the user should write the weight in grams
     * of the test weight used to calculate the scale calibration
     * 
     * At the end of the calibration process the system is reset
     * 
     * \warning If the calibration is not hardcoded in the SCALE_CALIBRATION constant
     * the manual calibration process should be repeated ever ytime the system start.\n
     * The reset command does not affect the calibration value.
     * 
     * \note The two pass calibration process method is called multiple
     * times by the external and started by tge calibration command
     * that will guid the user to follow the right calibration
     * procedure.
     */
     void calibrate3Pass(void);
     
    /**
     * Update the materials IDs and calculations 
     */
    void calcMaterialCharacteristics(void);

    //! filament diameter (descriptive)
    String diameter;
    //! material type (descriptive)
    String material;
    //! roll weight (descriptive)
    String weight;
    //! system status
    String stat;
    //! Status ID
    int statID;
    //! grams for 1 cm material
    float gr1cm;
    //! centimeters for 1 gr material
    float length1gr;
    //! filament weight
    float rollWeight;
    //! roll tare
    float rollTare;
    //! Initial read weight from last reset
    float initialWeight;
    //! Last reliable value for consumed grams
    float lastConsumedGrams;
    //! Units display flag. Decide if consume is in grams or cm
    float filamentUnits;
};

#endif
