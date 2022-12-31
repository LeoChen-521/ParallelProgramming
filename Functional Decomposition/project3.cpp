#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <math.h>
#define _USE_MATH_DEFINES


unsigned int seed = 0;

//float x = Ranf(&seed, -1.f, 1.f);

// Global states

int	NowYear;		// 2021 - 2026
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
// number of earthworm in the current population, affect Grains height and number of Deers. Also is impacted by them.
int NowNumEarthworm; 


omp_lock_t	Lock;

int NumInThreadTeam;
int	NumAtBarrier;
int	NumGone;

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

// My agent
const float ONE_EARTHWORM_HELPS_GRAIN_GROW_PER_MONTH = 0.03;

void InitBarrier(int);
void WaitBarrier();

float
SQR(float x)
{
    return x * x;
}

// specify how many threads will be in the barrier:
//	(also init's the Lock)

// void
// InitBarrier( int n )
// {
//     NumInThreadTeam = n;
//     NumAtBarrier = 0;
// 	omp_init_lock( &Lock );
// }


// // have the calling thread wait here until all the other threads catch up:

// void
// WaitBarrier()
// {
//     omp_set_lock( &Lock);
//     {
//         NumAtBarrier++;
//         if(NumAtBarrier == NumInThreadTeam)
//         {
//             NumGone = 0;
//             NumAtBarrier = 0;
//             // let all other threads get back to what they were doing
// 			// before this one unlocks, knowing that they might immediately
// 			// call WaitBarrier( ) again:
//             while( NumGone != NumInThreadTeam-1 );
//             omp_unset_lock( &Lock );
//             return;
//         }
//     }
//     omp_unset_lock( &Lock );

//     while( NumAtBarrier != 0 );	// this waits for the nth thread to arrive

//     #pragma omp atomic
//     NumGone++;			// this flags how many threads have returned
// }


float
Ranf( unsigned int *seedp,  float low, float high )
{
    float r = (float) rand_r( seedp );              // 0 - RAND_MAX

    return(low  +  r * ( high - low ) / (float)RAND_MAX);
}


int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
    float low = (float)ilow;
    float high = (float)ihigh + 0.9999f;

    return (int)(  Ranf(seedp, low,high) );
}


// Function that convert height unit.
float
InchToCM(float x)
{
	return (x * 2.54);
}

// Function that convert Temperature unit.
float
FahrenheitToCelsius(float x)
{
	return ( (5./9.) * (x-32) );
}

void
IncrementMonth()
{
	NowMonth += 1;
}

void
IncrementYear()
{
	NowYear += 1;
}

void Deer()
{
	while (NowYear < 2027)
	{
		int nextNumDeer = NowNumDeer;
		int carryingCapacity = (int)( NowHeight );

		// The number of deer depends on the amount of grain available to eat.
		if(nextNumDeer < carryingCapacity)
		{
			nextNumDeer++;
		}
		else
		{
			if(nextNumDeer > carryingCapacity)
			{
				 nextNumDeer--;
			}
		}

		if(nextNumDeer < 0)
		{
			nextNumDeer = 0;
		}

		// DoneComputing barrier at this point

		#pragma omp barrier
		// do nothing, wait for other threads

		// copy the local variable into the global version.
		NowNumDeer = nextNumDeer;
		// hit the DoneAssigning barrier

		#pragma omp barrier

		// Hit DonePrinting barrier

		#pragma omp barrier
	}       
}

void Grain()
{
	while (NowYear < 2027)
	{
		// Compute the next grain height.
		float tempFactor = exp(-SQR(  ( NowTemp - MIDTEMP ) / 10.));
		float precipFactor = exp(-SQR(  ( NowPrecip - MIDPRECIP ) / 10.));

		float nextHeight = NowHeight;

		nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

		// My agent Earthworm helps grain grow.
		nextHeight += (float)NowNumEarthworm * ONE_EARTHWORM_HELPS_GRAIN_GROW_PER_MONTH;

		if( nextHeight < 0. )
		{
			nextHeight = 0.;
		}

		// Hit the DoneComputing barrier.

		#pragma omp barrier

		// Copy the local variable into the global version.

		NowHeight = nextHeight;

		// Hit the DoneAssigning barrier.

		#pragma omp barrier

		// Hit DonePrinting barrier.

		#pragma omp barrier
	}
}

void Watcher()
{
	while(NowYear < 2027) // 2027, 6 years, 72 months
	{
		// do nothing
		#pragma omp barrier

		// Hit DoneComputing barrier

		#pragma omp barrier

		// Hit DoneAssigning barrier

		// print the current set of global state variables
		// printf("CurrentYear:", NowYear);
		// printf("CurrentMonth:", NowMonth);

		// convert units of the current set of global state variables
		
		float current_temperature = FahrenheitToCelsius(NowTemp);
		float current_precipitation = InchToCM(NowPrecip);
		float current_grainHeight = InchToCM(NowHeight);

		//printf("NowNumDeer:", NowNumDeer);
		//printf("NowGrainHeight", NowHeight);

		printf ("Year: %d\n", NowYear);
		printf ("Month: %d\n", NowMonth);

		printf ("current Number of Deers %d\n", NowNumDeer);
		printf ("current GrainHeight: %4.2f\n", current_grainHeight);
		printf ("current Temperature: %4.2f\n", current_temperature);
		printf ("current Precipitation: %4.2f\n", current_precipitation);
		printf ("current Number of earthworms %d\n", NowNumEarthworm);
	



		// printf("%s\n", );

		//fprintf(stderr, "%2d Year : %8d Month\n", NowYear, NowMonth);
		// increment the month count

		IncrementMonth();

		if (NowMonth == 12)
		{
			NowMonth = 0;
			IncrementYear();
		}
		// use the new month to compute the new Temperature and Precipitation.

		float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);

		float temp = AVG_TEMP - AMP_TEMP * cos(ang);

		NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
		NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );

		if (NowPrecip < 0.)
		{
			NowPrecip = 0.;
		}

		// Hit the DonePrinting barrier


		#pragma omp barrier
	}
}

void Earthworm()
{
	while(NowYear < 2027)
	{
		int nextNumEarthworm = NowNumEarthworm;

		// if the tempurature in Celsius, is larger than or equal to the precipitation in centimeters
		// Too hot, reduce the number of Earthworm by 3 per month.


		if (FahrenheitToCelsius(NowTemp) >= InchToCM(NowPrecip))
		{
			nextNumEarthworm -= 3;
		}
		// good condition for earthworms, increase by 1.
		else
		{
			if(FahrenheitToCelsius(NowTemp) < InchToCM(NowPrecip))
			{
				nextNumEarthworm += 1;
			}
		}

		// if Earthworm down to less than 0, no changes.
		if (nextNumEarthworm < 0)
		{
			nextNumEarthworm = 0;
		}

		// DoneComputing barrier at this point
		#pragma omp barrier

		// copy the local variable into the global version.
		NowNumEarthworm = nextNumEarthworm;
		// hit the DoneAssigning barrier

		#pragma omp barrier

		// Hit DonePrinting barrier

		#pragma omp barrier
	}
}

//----------------------------------------------------------------------------------------

int main(int argc, char * argv[])
{
	NowYear = 2021;
	NowMonth = 0;

	// Deer starting value.
	NowNumDeer = 1;

	// Grain starting height value.
	NowHeight = 1.;

	// My agent starts at 50.
	NowNumEarthworm = 50;

	omp_init_lock(&Lock);

	omp_set_num_threads(4); // 4 threads
	//InitBarrier(4);

	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );

	if(NowPrecip < 0.)
	{
		NowPrecip = 0.;
	}

	omp_set_num_threads(4);

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			Deer();
		}

		#pragma omp section
		{
			Grain();
		}

		#pragma omp section
		{
			Watcher();
		}

		#pragma omp section
		{
		 	Earthworm();
		}
	}    
}



























