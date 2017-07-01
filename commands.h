/* 
 *  Serial commands to control the behaviour of the system
 *  
 *  All the commands are single character
 */

// Units selection
#define SHOW_GRAMS "g"    // Set calculation units in grams
#define SHOW_CM "cm"       // Set calc units in cm (meters when value is higher than 100)

// Filament setup
#define SET_PLA "pla"
#define SET_ABS "abs"
#define SET_175 "175"
#define SET_300 "300"
#define SET_1KG "1k"
#define SET_2KG "2k"

// Manual status change
#define STATUS_RESET "reset"    // Reset the system with the current filament setup
#define STATUS_LOAD "load"      // Filament roll has been loaded
#define STATUS_RUN "run"        // Print job is running
#define STATUS_DEFAULT "init"   // Reset the system with the default settings

// Running mode
#define MODE_AUTO "auto"        // Run in automatic mode
#define MODE_MANUAL "manual"    // Run in manual mode

// Motor control
#ifdef _MOTOR
#define MOTOR_FEED "feed"       // Feed a length unit
#define MOTOR_PULL "pull"       // Pull back a lenght unit
#define MOTOR_FEED_CONT "feedcont"    // Feed continuopusly
#define MOTOR_PULL_CONT "pullcont"    // Pull back continuously
#define MOTOR_FEEDLEN "length"        // Centimeters to feed every command
#endif

// Information commands
#define SHOW_SETTINGS "config"    // Shows the current configuration settings
#define SHOW_STATUS "status"      // Shows all the weight status values

// Setting commands
#define SET_FEEDLENGTH "setfeed"    // Set the motor filament lenght. Usage:  setfeed=x [x = cm]

