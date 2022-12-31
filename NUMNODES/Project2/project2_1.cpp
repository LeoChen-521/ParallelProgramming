#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

#define N	0.70

#ifndef NUMNODES
#define NUMNODES 1000
#endif

#ifndef NUMT
#define NUMT 8
#endif

#ifndef NUMTRIES
#define NUMTRIES 10
#endif

float Height( int, int );	// function prototype

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r <= 0. )
	        return 0.;
	float height = pow( r, 1./(float)N );
	return height;
}


int main( int argc, char *argv[ ] )
{
	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for loop and a reduction:
	double volume = 0.;
	omp_set_num_threads(NUMT);

	double maxPerformance = 0.;

	for (int tries = 0; tries < NUMTRIES; tries++)
	{
		volume = 0.;
		double time0 = omp_get_wtime();

	#pragma omp parallel for collapse(2) default(none) shared(fullTileArea) reduction(+: volume)
		for(int iv = 0; iv < NUMNODES; iv++ )
		{
			for(int iu = 0; iu < NUMNODES; iu++ )
			{
				float currentHeight = 2 * Height(iu, iv); //Get the height of the current node.


				//if the node is at the cornors
				if ((iu == 0 && iv == 0) 
					|| (iu == NUMNODES-1 && iv == 0) 
					|| (iu == NUMNODES-1 && iv == NUMNODES-1) 
					|| (iu == 0 && iv == NUMNODES-1))
				{
					volume = volume + (currentHeight * fullTileArea * 0.25);
				}
				//if the node is on the edges
				else if (iu == 0
					|| (iu == NUMNODES-1)
					|| (iv == 0)
					|| (iu == NUMNODES-1))
				{
					volume = volume + (currentHeight * fullTileArea * 0.5);
				}
				else
				{
					volume = volume + (currentHeight * fullTileArea);
				}
			}
		}
		double time1 = omp_get_wtime( );
		double megaHeightsPerSecond = (double)NUMNODES * (double)NUMNODES / ( time1 - time0 ) / 1000000.;
		if (megaHeightsPerSecond > maxPerformance)
		{
			maxPerformance = megaHeightsPerSecond;
		}
	}


	


	fprintf(stderr, "%2d threads : %8d NUMNODES ; megaHeights/sec = %6.2lf\n", NUMT, NUMNODES, maxPerformance);
	printf("Volume = %f \n", volume);
	return 0;
}







