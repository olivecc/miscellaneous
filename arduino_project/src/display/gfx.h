#ifndef ARD_CSWRK_GFX
#define ARD_CSWRK_GFX

#include <SDL2/SDL.h>
#include <stdint.h>

void drawLine(int, int, int, int, SDL_Surface*);
void drawBlob(int, int, SDL_Surface*, uint32_t);
void drawCube(const double[3][3], int, int, SDL_Surface*, int, int);

#endif //ARD_CSWRK_GFX
