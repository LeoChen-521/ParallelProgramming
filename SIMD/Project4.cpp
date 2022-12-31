#include <xmmintrin.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define _USE_MATH_DEFINES

#define SSE_WIDTH		4

#ifndef NUMTRIES
#define NUMTRIES		10
#endif

#ifndef ARRAYLENGTH
#define ARRAYLENGTH		10000
#endif


float A[ARRAYLENGTH];
float B[ARRAYLENGTH];

float sum;
float SSEsum;
//float C[SIZE];

void
SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;
	register float *pc = c;
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		_mm_storeu_ps( pc,  _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
		pc += SSE_WIDTH;
	}

	for( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}


float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;

	__m128 ss = _mm_loadu_ps( &sum[0] );
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		ss = _mm_add_ps( ss, _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
	}
	_mm_storeu_ps( &sum[0], ss );

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}



int main(int argc, char * argv[])
{
	// Control group sum
	sum = 0.;
	SSEsum = 0;

	// SSE performance
	double SSEmaxMegaMults = 0.;
	// Control group performance
	double maxMegaMults = 0.;

	for( int t = 0; t < NUMTRIES; t++ )
    {
    	// inialize the arrays:
		for( int i = 0; i < ARRAYLENGTH; i++ )
		{
			A[ i ] = 1.;
			B[ i ] = 2.;
        }

       	//---------------------------------------------------------------------------
        // control group
        double time0 = omp_get_wtime();

        for( int i = 0; i < ARRAYLENGTH; i++ )
        {
            //C[i] = A[i] * B[i];
            sum = sum + A[i] * B[i];
        }

        double time1 = omp_get_wtime( );
        double megaMults = (double)ARRAYLENGTH/(time1-time0)/1000000.;
        if( megaMults > maxMegaMults )
        {
            maxMegaMults = megaMults;
        }

		//------------------------------------------------------------------------
		// SSE group

        double SSEtime0 = omp_get_wtime();

		SSEsum = SimdMulSum(A, B, ARRAYLENGTH);

		double SSEtime1 = omp_get_wtime();

		double SSEmegaMults = (double)ARRAYLENGTH/(SSEtime1-SSEtime0)/1000000.;
        if( SSEmegaMults > SSEmaxMegaMults )
        {
            SSEmaxMegaMults = SSEmegaMults;
        }
    }

    // Calculate speed up. SSE/Non-SSE
    float speedup = SSEmaxMegaMults / maxMegaMults;

    printf( "Array Size: %d\n", ARRAYLENGTH);

    printf( "Non-SSE Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );

    printf( "SSE Peak Performance = %8.2lf MegaMults/Sec\n", SSEmaxMegaMults );

    printf( "Speed-up = %4.2f\n", speedup);



// note: %lf stands for "long float", which is how printf prints a "double"
//        %d stands for "decimal integer", not "double"

    return 0;
}
























