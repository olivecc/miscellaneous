# Arduino-Controlled 3D Game

A coursework project implemented as part of the UCL COMP101P course on *Principles 
of Programming in C*, consisting of a simple game controlled by an Arduino 
microcontroller.  
The original documentation is provided below (note that the 'arduino-serial' 
tool referenced below is intended for use by UCL only, and so is not provided, 
preventing building of the project; to see how it would work, see the demo 
videos).

## Summary

The premise of the project is that the Arduino board acts as a controller, 
providing input via serial to a game (C program) running on a connected 
computer. The synopsis of the game is that two cubes are shown on the left and 
right of the screen; the player controls the orientation of the cube on the 
right, and their objective is to match that of the cube on the left. When 
this is achieved, the orientation of the ‘target cube’ resets and the player 
must continue to re-orient the ‘player cube’ to match the target cube until 
the game (window) is closed.  
The breadboard attached to the Arduino features 3 pairs of inputs where each 
pair controls the roll, yaw, and pitch of the player cube; these consist of a 
pair of push buttons on the left of the breadboard, a pair of push buttons on 
the right of the breadboard, and tilt sensors on either end of the breadboard, 
summing to six sensors. For each pair, engaging one causes the player cube to 
rotate in the positive direction of that axis (roll/yaw/pitch), while engaging 
the other causes it to rotate in the other direction.   
The display program is written in C, using the Simple DirectMedia Layer (SDL) 
cross-platform development library for keyboard input and pixel output to the 
screen.(functionality more complex than individual pixel drawing is implemented 
in the program; this includes drawing lines and rendering wireframe cubes 
(using 3x3 matrix  multiplication to calculate positions of vertices, then 
drawing lines between vertices accordingly).



## Execution

- [Install](https://wiki.libsdl.org/Installation) the SDL2 library.  
- Build the display project inside *arduino*/*display*  
 - MacOS/Unix: ‘gcc -o cube main.c game.c gfx.c matrices.c -lSDL2 -lm’  
 - Windows: Add contents of directory as project files to Visual Studio, compile as release  
- Compile/upload the Arduino program inside Arduino/Board/ to the Arduino board 
  (Arduino IDE 1.7)  
- Connect computer to Arduino via USB  
- Execute the game program and ‘arduino-serial’, piping the result of 
  ‘arduino-serial’ to the stdin of the game program (e.g. `./arduino-serial 
  -q -b 9600 -p /dev/ttyACM0 -l | cube`)  
 - If the game needs to be debugged without Arduino serial input,
   comment out lines 52-66 of ‘game.c’ and use keyboard input.  
- Controls: Buttons/tilt sensors as described above, or Q/A, W/S, and E/D 
  for controlling positive/negative directions of each axis respectively. 
  Close window by exiting/entering exit sequence (e.g. CTRL+SHIFT+Q on 
  Linux). 
