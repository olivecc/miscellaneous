/* board.ino
 *
 * Concerns Arduino initialization/execution, calls procedure to process
 * digital input and relay it to the PC program once per frame (
 */

#include <SPI.h>
#include "Arduino.h"
#include <avr/pgmspace.h> 

//PROGMEM specifies consts to be stored in flash memory to preserve SRAM
static const PROGMEM int baudRate = 9600;
static const PROGMEM int framesPerSecond = 60;
static const PROGMEM int dt = 1000/framesPerSecond;	//in ms
//no portable way to standardise no. of frames per second at runtime, as 
//there is no easy portable method of duplex communication between PC & 
//Arduino (bash coprocesses would work in POSIX environments however).


void setup() {
  Serial.begin(baudRate);
}

void loop() {
  processPinInput();
  delay(dt);
}
