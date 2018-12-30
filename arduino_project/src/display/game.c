/* game.c
 *
 * collection of functions relating to game processing.
 */

#include "game.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "matrices.h"
#include "gfx.h"


static const double DIFFICULTY = 0.5;
//see playerRotationAcceptable for mechanics of how this is used, but 
//a lower value increases difficulty.

static const double rotationPerSecond = 4 * M_PI / 3;


//get line from stdin
static void getLine (char *buff, size_t sz) 
{
    if (fgets (buff, sz, stdin) == NULL)
        return;

    if (buff[strlen(buff)-1] != '\n') {
        return;
    }

    buff[strlen(buff)-1] = '\0';
}

//Integer exponentiation function (positive exponent).
static int ipow(int base, int expo)
{
	int result = 1;
	while(expo > 0)
	{
		result *= base;
		expo--;
	}
	return result;
}

static void handleInput(SDL_Event* sdlEvent, input_t* input)
{
	
	char buff[3]; 
	//Arduino Serial.println prints the intended character, followed by 
	//'\r', and '\n'.

	do
		getLine(buff, sizeof(buff));
	while(buff[0] == '\0');

	char inputChar = buff[0] - 'A';

	input->roll =  ((inputChar / ipow(3, 0)) % 3) - 1;
	input->yaw =   ((inputChar / ipow(3, 1)) % 3) - 1;
	input->pitch = ((inputChar / ipow(3, 2)) % 3) - 1;
	//decodes input char into axes's polarities, see pininput.ino


	while(SDL_PollEvent(sdlEvent))	//poll the queue of events (e.g. keydowns)
	{
		switch(sdlEvent->type)
		{
			case SDL_QUIT:
				input->exit = true;
				break;
			case SDL_KEYDOWN:
				switch(sdlEvent->key.keysym.sym)
				{
					case SDLK_q:
						input->roll = -1;
						break;
					case SDLK_a:
						input->roll = 1;
						break;
					case SDLK_w:
						input->yaw = -1;
						break;
					case SDLK_s:
						input->yaw = 1;
						break;
					case SDLK_e:
						input->pitch = -1;
						break;
					case SDLK_d:
						input->pitch = 1;
						break;
				}
				break;
			case SDL_KEYUP:
				switch(sdlEvent->key.keysym.sym)
				{
					case SDLK_q:
					case SDLK_a:
						input->roll = 0;
						break;
					case SDLK_w:
					case SDLK_s:
						input->yaw = 0;
						break;
					case SDLK_e:
					case SDLK_d:
						input->pitch = 0;
						break;
				}
				break;

		}
	}
}





bool playerRotationAcceptable(double matrix1[3][3],
			      double matrix2[3][3])
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			if(fabs(matrix1[i][j] - matrix2[i][j]) > DIFFICULTY)
			{
				return false;
			}
		}
	}
	return true;	
	//reached only if the respective elements of each matrices are suitably 
	//similar (difference less than DIFFICULTY)
}

//where dt is in milliseconds
void integrate(double dt, gameState_t* gameState)
{
	handleInput(&(gameState->sdlEvent), &(gameState->input));
	if(gameState->waitTime > 0)
	{
		//ensure that waitTime is the greater of 0 and (waitTime - dt)
		if(dt <= gameState->waitTime)
		{
			gameState->waitTime -= dt;
		}
		else
		{
			gameState->waitTime = 0;
		}
		return;
	}
	
	if(playerRotationAcceptable(gameState->playerRotation, 
				    gameState->targetRotation))
	{
		gameState->waitTime = 1000;		//ms
		
		//setToIdentity(gameState->playerRotation);
		//uncomment to reset player cube to default after 'winning'

		randomMatrix(gameState->targetRotation);
		
		return;
	}
	
	double deltaAngle = rotationPerSecond * dt / 1000;
	double yawTransform[3][3], rollTransform[3][3], pitchTransform[3][3];
        
	getYawTransform(  yawTransform,   deltaAngle * gameState->input.yaw);
	getRollTransform( rollTransform,  deltaAngle * gameState->input.roll);
	getPitchTransform(pitchTransform, deltaAngle * gameState->input.pitch);
	//2nd argument simplifies to one of deltaAngle, -deltaAngle and 0


	updateTransform(yawTransform, gameState->playerRotation);
	updateTransform(rollTransform, gameState->playerRotation);
	updateTransform(pitchTransform, gameState->playerRotation);
}

void render(const gameState_t* gameState)
{
	drawCube(gameState->targetRotation, gameState->width / 4, gameState->height / 2, 
			gameState->sdlSurface, gameState->width, gameState->height);
	drawCube(gameState->playerRotation, gameState->width * 3 / 4, gameState->height / 2, 
			gameState->sdlSurface, gameState->width, gameState->height);


	//boilerplate for pushing the pixel buffer to the screen
	SDL_UpdateTexture(gameState->sdlTexture, NULL,
			gameState->sdlSurface->pixels, 
			gameState->sdlSurface->pitch);

	SDL_RenderClear(gameState->sdlRenderer);
	SDL_RenderCopy(gameState->sdlRenderer, 
			gameState->sdlTexture, NULL, NULL);

	SDL_RenderPresent(gameState->sdlRenderer);
	SDL_FillRect(gameState->sdlSurface, NULL, 0x000000);	

}

unsigned int getTime()
{
	return SDL_GetTicks();
}

void delay(unsigned int t) 
{
	SDL_Delay(t);
}

void end()
{
	SDL_Quit();
}

gameState_t initState(int width, int height, const char* const title)
{
	input_t tempInput = {.roll=0, .yaw=0, .pitch=0, .exit = false};

	gameState_t gameState = {
		.input = tempInput,
		.waitTime = 0,
		.width = width,
		.height = height
	};

	setToIdentity(gameState.playerRotation);
	do
	{
		randomMatrix(gameState.targetRotation);
	} while(playerRotationAcceptable(gameState.targetRotation, 
					 gameState.playerRotation));
	//ensure that the initial player/target cubes are suitably different

	//lots of boilerplate for initializing the 
	//SDL window/renderer/surface/texture
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
	if(SDL_Init(	SDL_INIT_TIMER |
			SDL_INIT_AUDIO |
			SDL_INIT_VIDEO) != 0)
	{
		printf("error: could not init");
		exit(EXIT_FAILURE);
	}

	gameState.sdlWindow = SDL_CreateWindow(title, 0, 0, width, height, 
			SDL_WINDOW_FULLSCREEN_DESKTOP);
	if(gameState.sdlWindow == NULL)
	{
		printf("error: could not create window");
		exit(EXIT_FAILURE);
	}

	gameState.sdlRenderer = SDL_CreateRenderer(gameState.sdlWindow, -1, 0);
	if(gameState.sdlRenderer == NULL)
	{
		printf("error: could not create renderer");
		exit(EXIT_FAILURE);
	}

	SDL_SetRenderDrawColor(gameState.sdlRenderer, 255, 255, 255, 255);
	gameState.sdlTexture = SDL_CreateTexture(gameState.sdlRenderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			width, height);
	if(gameState.sdlTexture == NULL)
	{
		printf("error: could not create texture");
		exit(EXIT_FAILURE);
	}

	gameState.sdlSurface = SDL_CreateRGBSurface(0, width, height, 32,
			0x00FF0000,
			0x0000FF00,
			0x000000FF,
			0xFF000000);

	if(gameState.sdlSurface == NULL)
	{
		printf("error: could not create surface");
		exit(EXIT_FAILURE);
	}

	return gameState;
}

