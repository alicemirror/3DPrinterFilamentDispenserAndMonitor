/* 
 *  DC motor parameters for the Infineon DC motor controller Arduino shield.
 *  
 */
#ifdef _MOTOR
// Duration in ms motor runs before reading for changes (sensor, serial)
#define MOTOR_PULSESHORT 100 
// Duration in ms motor runs before reading for new command while motor
// runs continuously
#define MOTOR_PULSELONG 500
// Weight reading (grams) delta to start the motor running
// requesting new filament released
#define FILAMENT_REQUEST 25
// Acceleration enable flags
// Acceleration is useful when the roll is more than 60% of filament full
#define ACCELERATION_ON 1
#define ACCELERATION_OFF 0
#endif
