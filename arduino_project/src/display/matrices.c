/* matrices.c
 * 
 * Source code for handling (3x3, 3x1) matrices. These functions work on the 
 * arrays pointed to by pointers supplied in the argument list, 
 * thus the public (non-static) functions have no return type.
 */

#include "matrices.h"
#include <stdbool.h>
#include <math.h>
#include <string.h> 	//for memcpy
#include "matrices.h"
#include <math.h>
#include <stdlib.h>

//identity 3x3 matrix
static const double identity[3][3] = {{1,0,0},
				      {0,1,0},
				      {0,0,1}};

//returns a double from a uniform probability distribution between two bounds.
static double randomDouble(double bound1, double bound2)
{
	if(bound1 > bound2)
	{
		double temp = bound1;
		bound1 = bound2;
		bound2 = temp;
	}

	double randInt = (double)abs(rand());
	
	return fmod(randInt, bound2 - bound1) + (bound1);
}


void applyTransform(const double transform[3][3], double point[3])
{
	double temp[3];
	for(int i = 0; i < 3; i++)
	{
		temp[i] = 0;
		for(int j = 0; j < 3; j++)
		{
			temp[i] += transform[i][j] * point[j];
		}
	}
	memcpy(point, temp, sizeof(double) * (3 * 1));
}

//multiplies two matrices (e.g. a new transformation matrix and a matrix
//representing all sum transformations so far)
void updateTransform(const double apply[3][3], double old[3][3])
{
	double temp[3][3];
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			temp[i][j] = 0;
			for(int k = 0; k < 3; k++)
			{
				temp[i][j] += apply[i][k] * old[k][j];
			}
		}
	}
	memcpy(old, temp, sizeof(double) * (3 * 3));
}

inline void setToIdentity(double matrix[3][3])
{
	memcpy(matrix, identity, sizeof(double) * (3 * 3));
}

void getYawTransform(double matrix[3][3], double theta)
{
	setToIdentity(matrix);
	matrix[0][0] = cos(theta);
	matrix[1][1] = cos(theta);
	matrix[0][1] = sin(theta);
	matrix[1][0] = -sin(theta);
	/* (cos(x)	sin(x)		0)
	 * (sin(x)	cos(x)		0)
	 * (0		0		1)
	 */
}

void getRollTransform(double matrix[3][3], double theta)
{
	setToIdentity(matrix);
	matrix[1][1] = cos(theta);
	matrix[2][2] = cos(theta);
	matrix[1][2] = sin(theta);
	matrix[2][1] = -sin(theta);
	/* (1		0		0     )
	 * (0		cos(x)		sin(x))
	 * (0		-sin(x)		cos(x))
	 */
}

void getPitchTransform(double matrix[3][3], double theta)
{
	setToIdentity(matrix);
	matrix[0][0] = cos(theta);
	matrix[2][2] = cos(theta);
	matrix[0][2] = -sin(theta);
	matrix[2][0] = sin(theta);
	/* (cos(x)	0	-sin(x))
	 * (0		1	0      )
	 * (sin(x)	0	cos(x) )
	 */
}	

void randomMatrix(double transform[3][3])
{
	setToIdentity(transform);

	double yawTransform[3][3], rollTransform[3][3], pitchTransform[3][3];
	getYawTransform(  yawTransform,   randomDouble(0, 2 * M_PI));
	getRollTransform( rollTransform,  randomDouble(0, 2 * M_PI));
	getPitchTransform(pitchTransform, randomDouble(0, 2 * M_PI));
	
	//merge the new random transformations into one
	updateTransform(yawTransform, 	transform);
	updateTransform(rollTransform, 	transform);
	updateTransform(pitchTransform, transform);
}
