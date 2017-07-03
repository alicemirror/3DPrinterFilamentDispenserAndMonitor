/* 
 *  Serial commands to control the behaviour of the system
 *  
 *  All the commands are single character
 */

 #ifndef _COMMANDS
 #define _COMMANDS

// Execution notification
#define CMD_EXEC "***CMD : executing "
#define CMD_NOCMD "***ERR : unknown "
#define CMD_SET "***CMD : setting "
#define CMD_MODE "***CMD : set mode to "
#define CMD_UNITS "***CMD : set units to "
#define CMD_STATUS "***CMD : set status to "

// Filament setup
#define SET_PLA "PLA"
#define SET_ABS "ABS"
#define SET_175 "1.75"
#define SET_300 "3.00"
#define SET_1KG "1kg"
#define SET_2KG "2kg"

// Status change
#define S_RESET "reset"        // Reset the system with the current filament setup
#define S_LOAD "load"          // Filament roll has been loaded
#define S_RUN "run"            // Print job is running
#define S_DEFAULT "default"    // Reset the system with the default settings

// Running mode
#define MODE_AUTO "auto"        // Run in automatic mode
#define MODE_MANUAL "manual"    // Run in manual mode

// Motor control
#ifdef _USE_MOTOR
#define MOTOR_FEED "feed"       // Feed a length unit
#define MOTOR_PULL "pull"       // Pull back a lenght unit
#define MOTOR_STOP "stop"       // Pull back a lenght unit
#define MOTOR_FEED_CONT "feedcont"    // Feed continuopusly
#define MOTOR_PULL_CONT "pullcont"    // Pull back continuously
#endif

// Information commands
#define SHOW_SETTINGS "config"    // Shows the current configuration settings
#define SHOW_STATUS "status"      // Shows all the weight status values
#define SHOW_DUMP "dump"          // Dump all the current settings
#define SHOW_WEIGHT "weight"      // Show the current read weight

#endif
