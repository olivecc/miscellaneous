#include "game.h"
//Supplies 'game' library: game functions and a game state pseudoclass.
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

static const int	framesPerSecond = 120;		
//double that of the arduino in order to be able to process all input in time.
static const int  	screenWidth = 640;
static const int 	screenHeight = 480;
static const char* 	windowTitle = "Cube Rotater";

int main()
{
	srand(time(NULL));	//initial seed for rand()
	double dt = 1000 / framesPerSecond;

	gameState_t gameState = initState(screenWidth, screenHeight, windowTitle);
	
	while(!gameState.input.exit)
	{
		integrate(dt, &gameState);
		render(&gameState);
		delay(dt);	
	}
	end();
}
