/* gfx.c
 * Collection of functions for drawing, in raw pixels, to pixel buffers
 * (supplied as a member value of a SDL_Surface struct).
 */

#include "gfx.h"
#include "matrices.h"
#include <stdbool.h>

static const uint32_t colour[8] = {0x00002b54,
				   0x001a86a8,
				   0x00391242,
				   0x00ffe293,
				   0x00ec008c,
				   0x00ffef6c,
				   0x00145a32,
				   0x0082e0aa};
//colours used for the 8 vertices of the cube(s)


static void swap(int* x, int* y)
{
	int temp = *x;
	*x = *y;
	*y = temp;
}

//bresenham's line drawing algorithm, adapted for steep/shallow and 
//positive/negative gradient.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void drawLine(int x1, int y1, int x2, int y2,
			SDL_Surface* screen)
	{
		uint32_t* buffer = (uint32_t*)screen->pixels;
		//uint32_t is portable and 32-bit, for the ARGB colour buffer
		//(byte per channel, 4 * 8 = 32).

		bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
		if(steep)
		{
			swap(&x1, &y1);
			swap(&x2, &y2);
		}

		if(x1 > x2)
		{
			swap(&x1, &x2);
			swap(&y1, &y2);
		}

		double dx = x2 - x1;
		double dy = fabs(y2 - y1);

		double error = dx / 2.0;
		int ystep = (y1<y2) ? 1:-1;
		int y = y1;

		int maxX = x2;
		int width = screen->pitch / sizeof(uint32_t);
		for(int x=x1; x<=maxX; x++)
		{
			if(steep)
			{
				buffer[y + x * width] = 0x00FFFFFF;
			}
			else
			{
				buffer[x + y * width] = 0x00FFFFFF;
			}

			error -= dy;
			if(error < 0)
			{
				y += ystep;
				error += dx;
			}
		}
	}


void drawBlob(int x, int y, SDL_Surface* screen, uint32_t colour)
{
	uint32_t* buffer = (uint32_t*)screen->pixels;
	int screenWidth = screen->pitch / sizeof(uint32_t);
	int blobWidth = 20;	//px
	
	for(int i = -blobWidth / 2; i < blobWidth / 2; i++)
	{
		for(int j = -blobWidth / 2; j < blobWidth / 2; j++)
		{
			buffer[(x + i) + (y + j) * screenWidth] = colour;
		}

	}
}	

void drawCube(const double rotation[3][3], int centreX, int centreY, 
		     SDL_Surface* screen, int width, int height)
{
	//firstly generate the cube vertices ((1,1,1), (1,1,-1) etc.) and apply
	//the transformation matrix to rotate them...

	//consider a 3D matrix of arrays of length 3.
	double coords[2][2][2][3];

	for(int a = 0; a < 2; a++)
	{
		for(int b = 0; b < 2; b++)
		{
			for(int c = 0; c < 2; c++)
			{
				//generate (x,y,z) for (2*2*2=)8 cube vertices,
				//and rotate them.
				coords[a][b][c][0] = (2*a)-1; //0->-1,1->1
				coords[a][b][c][1] = (2*b)-1;
				coords[a][b][c][2] = (2*c)-1;
				applyTransform(rotation, coords[a][b][c]);
			}
		}
	}


	//...then for each vertex draw a line to 3 particular other vertices,
	//and a coloured blob at each vertex.

	int sidelen = width / 10;

	for(int a = 0; a < 2; a++)
	{
		for(int b = 0; b < 2; b++)
		{
			for(int c = 0; c < 2; c++)
			{
				//lines are drawn between vertices that are
				//diametrically opposed along each axis (hence
				//a/!a etc.), note only (x,y) used here (2D).
				drawLine(coords[ a][ b][ c][0] * sidelen + centreX,
					 coords[ a][ b][ c][1] * sidelen + centreY, 
					 coords[!a][ b][ c][0] * sidelen + centreX,
					 coords[!a][ b][ c][1] * sidelen + centreY,
					 screen);

				drawLine(coords[ a][ b][ c][0] * sidelen + centreX,
					 coords[ a][ b][ c][1] * sidelen + centreY, 
					 coords[ a][!b][ c][0] * sidelen + centreX,
					 coords[ a][!b][ c][1] * sidelen + centreY,
					 screen);

				drawLine(coords[ a][ b][ c][0] * sidelen + centreX,
					 coords[ a][ b][ c][1] * sidelen + centreY, 
					 coords[ a][ b][!c][0] * sidelen + centreX,
					 coords[ a][ b][!c][1] * sidelen + centreY,
					 screen);

				drawBlob(coords[a][b][c][0] * sidelen + centreX,
					 coords[a][b][c][1] * sidelen + centreY,	
					 screen, colour[a * 4 + b * 2 + c]);
				//draw a blob with a unique colour for each vertex.
			}
		}
	}
}


