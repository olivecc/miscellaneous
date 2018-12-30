/* pininput.ino
 *
 * Source code for handling digital pin input and outputting their sum state
 * via serial. Note that a header is not required as Arduino 1.7 generates
 * function prototypes for all functions within the working directory.
 */
#include "Arduino.h"
#include <avr/pgmspace.h> 

//PROGMEM specifies consts to be stored in flash memory to preserve SRAM
const PROGMEM int axisQuantity = 3;



typedef enum axisPolarity
{
  POL_NEGATIVE = 0,
  POL_NEUTRAL,
  POL_POSITIVE
} axisPolarity_t;

enum rotationAxes
{
  AXIS_ROLL = 0,
  AXIS_YAW,
  AXIS_PITCH
};

typedef enum pinInput
{
  PIN_ROLL_NEG = 5,
  PIN_ROLL_POS = 4,
  PIN_YAW_NEG = 6,
  PIN_YAW_POS = 7,
  PIN_PITCH_NEG = 2,
  PIN_PITCH_POS = 3
} pinInput_t;



/*  Explicit declaration required to avoid Arduino 1.7 generating a function 
 *  prototype and prepending it to the main code body (including the 
 *  definition of the custom types used in the argument list).
    Encodes values of 2 input pins into a single axis polarity.
 */
static axisPolarity_t getAxisPolarity(pinInput_t negative, pinInput_t positive);
static axisPolarity_t getAxisPolarity(pinInput_t negative, pinInput_t positive)
{
	bool posSensor = digitalRead(positive);
	bool negSensor = digitalRead(negative);
 
	return (!(posSensor ^ negSensor)) ? POL_NEUTRAL	
		: ((posSensor) ? POL_POSITIVE : POL_NEGATIVE);
	//if both sensors (dis)engaged, no rotation occurs. Otherwise it occurs
	//in the direction specified by the active sensor.

}

//Integer exponentiation function (positive exponent).
static int ipow(int base, int expo)
{
	int result = 1;
	while (expo > 0)
	{
		result *= base;
		expo--;
	}
	return result;
}

//prints a character uniquely encoding the polarities of all axes
//according to the digital pin input at that frame. Character is in range of
//65 to 91, thus is stdin-safe.
void processPinInput()
{
	axisPolarity_t newPolarity[axisQuantity];

	newPolarity[AXIS_ROLL]  = getAxisPolarity(PIN_ROLL_NEG,  PIN_ROLL_POS);
	newPolarity[AXIS_YAW]   = getAxisPolarity(PIN_YAW_NEG,   PIN_YAW_POS);
	newPolarity[AXIS_PITCH] = getAxisPolarity(PIN_PITCH_NEG, PIN_PITCH_POS);

	char transmitter = 'A';
	for(int axis = 0; axis < axisQuantity; axis++)
	{
		transmitter += ipow(axisQuantity, axis) * newPolarity[axis];
		//can be envisaged by considering a string of ternary digits
	}
	Serial.println(transmitter);
}
