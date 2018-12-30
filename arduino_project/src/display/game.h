#ifndef ARD_CSWRK_GAME
#define ARD_CSWRK_GAME

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct input
{
	int roll;
	int yaw;
	int pitch;
	bool exit;
} input_t;

typedef struct gameState
{
	double 		targetRotation[3][3];
	double 		playerRotation[3][3];
	//3x3 matrices representing 3D rotation required to arrive at the cube. 
	input_t		input;
	int	 	waitTime;
	int		width;
	int		height;

	SDL_Window* 	sdlWindow;
	SDL_Renderer* 	sdlRenderer;
	SDL_Texture*	sdlTexture;
	SDL_Surface* 	sdlSurface;
	SDL_Event	sdlEvent;
} gameState_t;



gameState_t 	initState(int, int, const char* const);
unsigned int 	getTime(void);
void 		integrate(double, gameState_t*);
void 		render(const gameState_t*);
void 		delay(unsigned int);
void 		end(void);

#endif	//ARD_CSWRK_GAME
